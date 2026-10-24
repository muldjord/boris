/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            item.cpp
 *
 *  Mon Nov 11 12:54:00 CEST 2019
 *  Copyright 2019 Lars Muldjord
 *  muldjordlars@gmail.com
 ****************************************************************************/

/*
 *  This file is part of Boris.
 *
 *  Boris is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Boris is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Boris; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include "mainwindow.h"
#include "item.h"
#include "scripthandler.h"
//#include "soundmixer.h"
#include "settings.h"

#include <stdio.h>
#include <math.h>
#include <QApplication>
#include <QTimer>
#include <QDate>
#include <QTime>
#include <QElapsedTimer>
#include <QRandomGenerator>

extern MainWindow *mainWindow;

Item::Item(const QString &item, Settings &settings, const bool &ignore)
  : ignore(ignore), settings(settings)
{
  for(int a = 0; a < settings.itemBehaviours.count(); ++a) {
    if(settings.itemBehaviours.at(a).file == item) {
      curItem = a;
      break;
    }
  }

  origShadow = settings.getPixmap("shadow.png");
  shadowSprite = origShadow;

  setCursor(settings.getCursor("hover.png"));

  if(settings.itemTimeout > 0) {
    QTimer::singleShot(settings.itemTimeout * 1000, this, &Item::destroy);
  }

  if(settings.itemBehaviours.at(curItem).allowFlip && QRandomGenerator::global()->bounded(2)) {
    flipFrames = true;
  } else {
    flipFrames = false;
  }

  // Reset all script variables
  scriptVars.clear();

  // Clear script image canvas
  scriptImage.fill(Qt::transparent);
  drawing = false;

  // Look if a 'define init' exists in behaviour. If so, run it before starting first frame.
  if(settings.itemBehaviours.at(curItem).defines.contains("init")) {
    int stop = 0;
    runScript(stop, true);
  }

  ignoreTimer.setInterval(60000);
  ignoreTimer.setSingleShot(true);
  connect(&ignoreTimer, &QTimer::timeout, this, &Item::dontIgnore);

  animTimer.setInterval(0);
  animTimer.setSingleShot(true);
  connect(&animTimer, &QTimer::timeout, this, &Item::updateItem);
  animTimer.start();

  if(settings.itemBehaviours.at(curItem).noIgnore) {
    this->ignore = false;
  } else {
    ignoreTimer.start();
  }
  show();
}

Item::~Item()
{
}

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->drawPixmap(0, 0, rendered);
}

QRectF Item::boundingRect() const
{
  return QRectF(0, 0, rendered.width(), rendered.height());
}

QPixmap Item::getShadow(const QPixmap &sprite)
{
  QImage image = sprite.toImage();
  int bottom = image.height() - 1;
  QRgb *pixelBits = (QRgb *)image.constBits();
  for(int pixel = (image.width() * image.height()) - 1; pixel >= 0; --pixel) {
    if(qAlpha(pixelBits[pixel]) != 0) {
      bottom = (pixel / sprite.width());
      break;
    }
  }

  int firstX = image.width();
  int lastX = 0;
  int shadowYDelta = 4;
  for(int y = (bottom - shadowYDelta + 1 < 0?0:bottom - shadowYDelta + 1); y <= bottom; ++y) {
    QRgb *yBits = (QRgb *)image.constScanLine(y);
    int rowLastX = 0;
    for(int x = 0; x < sprite.width(); ++x) {
      if(qAlpha(yBits[x]) != 0) {
        if(x < firstX) {
          firstX = x;
        }
        rowLastX = x;
      }
    }
    if(rowLastX > lastX) {
      lastX = rowLastX;
    }
  }
  // In case no non-transparent pixels where found at all
  QImage shadow(32, 32, QImage::Format_ARGB32);
  shadow.fill(Qt::transparent);
  if(firstX < lastX) {
    int shadowWidth = lastX - firstX + 3;
    QPainter painter(&shadow);
    painter.drawImage(firstX - 1, bottom - image.height() + 1, origShadow.toImage().scaled(shadowWidth, origShadow.height()));
    painter.end();
  }
  //= origShadow.toImage().scaled(shadowWidth + 1, origShadow.height());
  return QPixmap::fromImage(shadow);
}

void Item::runScript(int &stop, const bool &init)
{
  // Update current stat variables for scripting use
  QPointF p = mainWindow->mapToScene(QCursor::pos());
  scriptVars["mx"] = p.x();
  scriptVars["my"] = p.y();
  scriptVars["mxvel"] = mouseHVel;
  scriptVars["myvel"] = mouseVVel;
  scriptVars["mdist"] = getDistance(mainWindow->mapToScene(QCursor::pos()));
  scriptVars["msec"] = getDirectionTo(mainWindow->mapToScene(QCursor::pos()));
  QDate date = QDate::currentDate();
  QTime time = QTime::currentTime();
  scriptVars["day"] = date.day();
  scriptVars["month"] = date.month();
  scriptVars["year"] = date.year();
  scriptVars["hour"] = time.hour();
  scriptVars["minute"] = time.minute();
  scriptVars["second"] = time.second();
  scriptVars["xvel"] = 0;
  scriptVars["yvel"] = 0;

  if(!drawing) {
    scriptImage.fill(Qt::transparent);
  }

  ScriptHandler scriptHandler(&scriptImage, &drawing, settings, nullptr, settings.itemBehaviours.at(curItem).labels, settings.itemBehaviours.at(curItem).defines, scriptVars, pos());
  connect(&scriptHandler, &ScriptHandler::setCurFrame, this, &Item::setCurFrame);
  if(init) {
    scriptHandler.runScript(stop, settings.itemBehaviours.at(curItem).defines["init"]);
  } else {
    scriptHandler.runScript(stop, settings.itemBehaviours.at(curItem).frames.at(curFrame).script);
  }

  scriptSprite = QPixmap::fromImage(scriptImage);
  if(scriptVars["xvel"] != 0 &&
     scriptVars["yvel"] != 0) {
    falling = true;
    hVel = scriptVars["xvel"];
    vVel = scriptVars["yvel"];
    altitude = pos().y();
  }
}

void Item::updateItem()
{
  if(stopAndDestroy) {
    stopAndDestroy = false;
    destroy();
    return;
  }

  sanityCheck();

  if(curFrame >= settings.itemBehaviours.at(curItem).frames.count()) {
    curFrame = 0;
  }

  if(flipFrames) {
    QImage flipped = settings.itemBehaviours.at(curItem).frames.at(curFrame).sprite.toImage().flipped(Qt::Horizontal);
    itemSprite = QPixmap::fromImage(flipped);
    shadowSprite = getShadow(QPixmap::fromImage(flipped));
  } else {
    itemSprite = settings.itemBehaviours.at(curItem).frames.at(curFrame).sprite;
    shadowSprite = getShadow(settings.itemBehaviours.at(curItem).frames.at(curFrame).sprite);
  }

  if(settings.sound && settings.itemBehaviours.at(curItem).frames.at(curFrame).soundBuffer != nullptr) {
    if(settings.itemBehaviours.at(curItem).pitchLock) {
      /*
      soundMixer.playSound(settings.itemBehaviours.at(curItem).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings.desktopWidth * 2.0 - 1.0, 1.0);
      */
    } else {
      /*
      soundMixer.playSound(settings.itemBehaviours.at(curItem).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings.desktopWidth * 2.0 - 1.0,
                           0.95 + QRandomGenerator::global()->bounded(100) / 1000.0);
      */
    }
  }

  if(settings.itemBehaviours.at(curItem).frames.at(curFrame).dx != 0 ||
     settings.itemBehaviours.at(curItem).frames.at(curFrame).dy != 0) {
    moveItem(settings.itemBehaviours.at(curItem).frames.at(curFrame).dx,
              settings.itemBehaviours.at(curItem).frames.at(curFrame).dy,
              flipFrames);
  }

  int frameTime = settings.itemBehaviours.at(curItem).frames.at(curFrame).time;
  frameTime -= (frameTime / 100.0 * borisHyper);
  if(frameTime != 0) { // Allow frameTime at 0 if it is specifically set in the script frame. This is to allow 'invisible' frames, such as used in 'mazegame'
    if(frameTime <= 5) {
      frameTime = 5;
    }
  }

  int stop = 0; // Will be > 0 if a goto, behav or break command is run
  runScript(stop);
  if(stop == 1) {
    // In case of 'goto' curFrame has been set in scriptHandler
  } else if(stop == 2) {
    // In case of 'break' it will destroy the item when the frame timer times out
    stopAndDestroy = true;
  } else if(stop == 3) {
    // In case of 'stop' it will cease any frame and animation progression
    animTimer.stop();
    return;
  } else {
    curFrame++;
  }

  // Assemble the Item sprite from parts
  prepareGeometryChange(); // Needed to make sure the parent scene has its geometry index updated or something... Otherwise boundingRect might report faulty size.
  rendered = QPixmap(itemSprite.width(), itemSprite.height());
  rendered.fill(Qt::transparent);
  QPainter painter(&rendered);
  painter.setOpacity(0.3);
  painter.drawPixmap(0, 0, shadowSprite);
  painter.setOpacity(1.0);
  painter.drawPixmap(0, 0, itemSprite);
  painter.drawPixmap(0, 0, scriptSprite);
  painter.end();

  // Make sure 'paint()' is called
  update();

  animTimer.setInterval(frameTime);
  animTimer.start();
}

void Item::moveItem(int dX, int dY, const bool &flipped)
{
  sanityCheck();

  int maxX = settings.desktopWidth - getWidth();
  int minY = 0 - getHeight();
  int maxY = settings.desktopHeight - getHeight();

  if(dX == 666) {
    dX = QRandomGenerator::global()->bounded(maxX);
  } else {
    if(flipped) {
      dX *= -1;
    }
    dX = pos().x() + dX;
  }
  if(dY == 666) {
    dY = QRandomGenerator::global()->bounded(maxY);
  } else {
    dY = pos().y() + dY;
  }

  setPos(dX, dY);

  // if item is outside borders
  if(falling && (pos().y() > maxY || pos().y() < minY)) {
    hVel *= 0.2;
    vVel = 0.0;
  }
}

void Item::sanityCheck()
{
  int minX = - getWidth();
  int maxX = settings.desktopWidth;
  int minY = 0;
  int maxY = settings.desktopHeight - getHeight();

  // Make sure Item is not located outside boundaries
  if(pos().y() < minY) {
    setPos(pos().x(), minY);
  }
  if(pos().y() > maxY) {
    setPos(pos().x(), maxY);
  }
  if(pos().x() > maxX) {
    setPos(minX, pos().y());
  }
  if(pos().x() < minX) {
    setPos(maxX, pos().y());
  }
}

int Item::getWidth() const
{
  return boundingRect().width();
}

int Item::getHeight() const
{
  return boundingRect().height();
}

int Item::getDistance(const QPointF &p)
{
  int xA = p.x();
  int yA = p.y();
  QPoint globalCenter = getGlobalCenter();
  int xB = globalCenter.x();
  int yB = globalCenter.y();

  return sqrt((yB - yA) * (yB - yA) + (xB - xA) * (xB - xA));
}

int Item::getDirectionTo(const QPointF &p)
{
  // Center coordinate of Item
  QPoint center = getGlobalCenter();

  // First find seg coordinate on x
  int xScale = abs(center.y() - p.y()) * 2;
  double xScaleSeg = xScale / 3.0;
  double xZero = center.x() - (xScale / 2.0);
  int xSeg = -1;
  if(p.x() < xZero + xScaleSeg) {
    xSeg = 0;
  } else if(p.x() < xZero + (xScaleSeg * 2)) {
    xSeg = 1;
  } else {
    xSeg = 2;
  }
  // Then find seg coordinate on y
  int yScale = abs(center.x() - p.x()) * 2;
  double yScaleSeg = yScale / 3.0;
  double yZero = center.y() - (yScale / 2.0);
  int ySeg = -1;
  if(p.y() < yZero + yScaleSeg) {
    ySeg = 0;
  } else if(p.y() < yZero + (yScaleSeg * 2)) {
    ySeg = 1;
  } else {
    ySeg = 2;
  }
  int pointSector = -1;
  if(xSeg == 0) {
    if(ySeg == 0) {
      pointSector = Direction::NorthWest;
    } else if(ySeg == 1) {
      pointSector = Direction::West;
    } else if(ySeg == 2) {
      pointSector = Direction::SouthWest;
    }
  } else if(xSeg == 1) {
    if(ySeg == 0) {
      pointSector = Direction::North;
    } else if(ySeg == 1) {
      pointSector = Direction::None;
    } else if(ySeg == 2) {
      pointSector = Direction::South;
    }
  } else if(xSeg == 2) {
    if(ySeg == 0) {
      pointSector = Direction::NorthEast;
    } else if(ySeg == 1) {
      pointSector = Direction::East;
    } else if(ySeg == 2) {
      pointSector = Direction::SouthEast;
    }
  }
  return pointSector;
}
/*
void Item::mouseDoubleClickEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton) {
    destroy();
  }
}

void Item::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton) {
    grabbed = true;
    ignore = false;
    ignoreTimer.stop();
    setCursor(settings.getCursor("grab.png"));
    oldCursor = QCursor::pos();
    if(!settings.itemBehaviours.at(curItem).reactions.isEmpty()) {
      curFrame = 0;
      animTimer.start(0, Qt::PreciseTimer, this);
    }
  } else if(event->button() == Qt::MiddleButton) {
    destroy();
  }
}

void Item::mouseMoveEvent(QMouseEvent* event)
{
  if(grabbed && event->buttons().testFlag(Qt::LeftButton)) {
    this->setPos(event->globalPosition().x() - settings.sizeFactor * 16.0, 
               event->globalPosition().y() - settings.sizeFactor * 20.0);
  }

}

void Item::mouseReleaseEvent(QMouseEvent* event)
{
  if(grabbed && event->button() == Qt::LeftButton) {
    setCursor(settings.getCursor("hover.png"));
    grabbed = false;
    falling = true;
    hVel = mouseHVel;
    vVel = mouseVVel;
    altitude = pos().y();
  }
}
*/
void Item::setCurFrame(const int &frame)
{
  curFrame = frame;
}

QPoint Item::getGlobalCenter()
{
  return QPoint(pos().x() + (getWidth() / 2),
                pos().y() + (getHeight() / 2));
}

QString Item::getReactionBehaviour()
{
  if(!settings.itemBehaviours.at(curItem).reactions.isEmpty()) {
    interactReaction = QRandomGenerator::global()->bounded(settings.itemBehaviours.at(curItem).reactions.count());
    return settings.itemBehaviours.at(curItem).reactions.at(interactReaction).getBehaviour();
  }
  return QString();
}

QString Item::getCategory()
{
  return settings.itemBehaviours.at(curItem).category;
}

void Item::destroy()
{
  // Remove from collide list
  for(int a = 0; a < settings.itemList.count(); ++a) {
    if(settings.itemList.at(a) == this) {
      settings.itemList.removeAt(a);
    }
  }
  delete this;
}

void Item::dontIgnore()
{
  ignore = false;
  borisHyper = 0;
}

void Item::handlePhysics()
{
  if(falling && !grabbed) {
    if(showShadow) {
      showShadow = false;
    }
    if(pos().y() + (vVel * settings.sizeFactor) <= altitude) {
      moveItem(hVel, vVel);
    } else {
      int distanceLeft = (altitude - pos().y()) / settings.sizeFactor;
      moveItem(hVel, vVel - ((vVel - distanceLeft) * 2));
      if(vVel <= 2.0) {
        falling = false;
      } else {
        hVel *= settings.itemBehaviours.at(curItem).bounce * 0.1;
        /*
        soundMixer.playSound(&soundMixer.soundFxs["bounce.wav"],
                             (float)pos().x() / (float)settings.desktopWidth * 2.0 - 1.0, 1.0);
        */
        vVel = (vVel * (settings.itemBehaviours.at(curItem).bounce * 0.1)) * -1;
      }
    }
    vVel += settings.itemBehaviours.at(curItem).weight * 0.1;
  } else {
    if(!showShadow) {
      showShadow = true;
    }
  }
  mouseHVel = (QCursor::pos().x() - oldCursor.x()) / 4.0;
  mouseVVel = (QCursor::pos().y() - oldCursor.y()) / 4.0;
  oldCursor = QCursor::pos();
}

void Item::interact(Boris *boris)
{
  scriptVars["energy"] = boris->getEnergy();
  scriptVars["health"] = boris->getHealth();
  scriptVars["hyper"] = boris->getHyper();
  scriptVars["hunger"] = boris->getHunger();
  scriptVars["toilet"] = boris->getToilet();
  scriptVars["social"] = boris->getSocial();
  scriptVars["fun"] = boris->getFun();
  scriptVars["hygiene"] = boris->getHygiene();

  borisHyper = boris->getHyper();

  setPos(boris->pos().x() + settings.itemBehaviours.at(curItem).reactions.at(interactReaction).getXCoord(), boris->pos().y() + settings.itemBehaviours.at(curItem).reactions.at(interactReaction).getYCoord() + (getWidth() * 0.5));

  if(settings.itemBehaviours.at(curItem).reactions.at(interactReaction).getLabel().isEmpty()) {
    destroy();
    return;
  }
  curFrame = settings.itemBehaviours.at(curItem).labels[settings.itemBehaviours.at(curItem).reactions.at(interactReaction).getLabel()];
  animTimer.setInterval(0);
  animTimer.start();
  setCursor(settings.getCursor("hover.png"));
  falling = false;
  grabbed = false;
  hVel = 0.0;
  vVel = 0.0;
  ignore = true;
  ignoreTimer.start();
}
