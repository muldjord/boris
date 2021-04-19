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

#include "item.h"
#include "scripthandler.h"
#include "soundmixer.h"
#include "settings.h"

#include "SFML/Audio.hpp"

#include <stdio.h>
#include <math.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QDate>
#include <QTime>
#include <QElapsedTimer>
#include <QRandomGenerator>

extern SoundMixer soundMixer;

Item::Item(const int &x, const int &y, const int &size, const QString &item, Settings &settings, const bool &ignore) : ignore(ignore), settings(settings), size(size)
{
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background:transparent");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  
  for(int a = 0; a < settings.itemBehaviours.count(); ++a) {
    if(settings.itemBehaviours.at(a).file == item) {
      curItem = a;
      break;
    }
  }

  setScene(new QGraphicsScene);
  scene()->setSceneRect(0.0, 0.0, 32, 32 + 1); // + 1 to make room for shadow

  origShadow = settings.getPixmap("shadow.png");
  shadowSprite = this->scene()->addPixmap(origShadow);
  shadowSprite->setOpacity(0.35);

  itemSprite = this->scene()->addPixmap(QPixmap());
  itemSprite->setPos(0, 0);

  scriptSprite = this->scene()->addPixmap(QPixmap());
  scriptSprite->setPos(0, 0); // To make room for shadow

  setCursor(settings.getCursor("hover.png"));

  setFixedSize(size, size + (size / 32)); // To make room for shadow
  scale(size / 32.0, size / 32.0);
  move(x, y);
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

  physicsTimer.setInterval(30);
  connect(&physicsTimer, &QTimer::timeout, this, &Item::handlePhysics);
  physicsTimer.start();

  ignoreTimer.setInterval(60000);
  ignoreTimer.setSingleShot(true);
  connect(&ignoreTimer, &QTimer::timeout, this, &Item::dontIgnore);
  
  animTimer.start(0, Qt::PreciseTimer, this);

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

QPixmap Item::getShadow(const QPixmap &sprite)
{
  QImage image = sprite.toImage();
  int firstLeft = sprite.width();
  int bottom = 0;
  for(int row = 0; row < sprite.height(); ++row) {
    QRgb *rowBits = (QRgb *)image.constScanLine(row);
    for(int col = 0; col < sprite.width(); ++col) {
      if(qAlpha(rowBits[col]) != 0) {
        if(bottom < row) {
          bottom = row;
        }
        if(firstLeft > col) {
          firstLeft = col;
          break;
        }
      }
    }
  }
  shadowSprite->setPos(firstLeft, bottom - 30);
  int firstRight = 0;
  for(int row = 0; row < sprite.height(); ++row) {
    QRgb *rowBits = (QRgb *)image.constScanLine(row);
    for(int col = sprite.width() - 1; col >= 0; --col) {
      if(qAlpha(rowBits[col]) != 0 && firstRight < col) {
        firstRight = col;
        break;
      }
    }
  }
  if(firstRight == 0) {
    firstRight = sprite.width();
  }
  if(firstLeft == sprite.width()) {
    firstLeft = 0;
  }
  int shadowWidth = firstRight - firstLeft;
  QImage shadow = origShadow.toImage().scaled(shadowWidth + 1, origShadow.height());
  return QPixmap::fromImage(shadow);
}

void Item::runScript(int &stop, const bool &init)
{
  // Update current stat variables for scripting use
  QPoint p = QCursor::pos();
  scriptVars["mousex"] = p.x();
  scriptVars["mousey"] = p.y();
  scriptVars["mdist"] = getDistance(QCursor::pos());
  scriptVars["msec"] = getSector(QCursor::pos());
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

  ScriptHandler scriptHandler(&scriptImage, &drawing, settings, nullptr, settings.itemBehaviours.at(curItem).labels, settings.itemBehaviours.at(curItem).defines, scriptVars, pos(), size);
  connect(&scriptHandler, &ScriptHandler::setCurFrame, this, &Item::setCurFrame);
  if(init) {
    scriptHandler.runScript(stop, settings.itemBehaviours.at(curItem).defines["init"]);
  } else {
    scriptHandler.runScript(stop, settings.itemBehaviours.at(curItem).frames.at(curFrame).script);
  }

  scriptSprite->setPixmap(QPixmap::fromImage(scriptImage));
  if(scriptVars["xvel"] != 0 &&
     scriptVars["yvel"] != 0) {
    falling = true;
    hVel = scriptVars["xvel"];
    vVel = scriptVars["yvel"];
    altitude = pos().y();
  }
}

void Item::timerEvent(QTimerEvent *)
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
    QImage flipped = settings.itemBehaviours.at(curItem).frames.at(curFrame).sprite.toImage().mirrored(true, false);
    itemSprite->setPixmap(QPixmap::fromImage(flipped));
    shadowSprite->setPixmap(getShadow(QPixmap::fromImage(flipped)));
  } else {
    itemSprite->setPixmap(settings.itemBehaviours.at(curItem).frames.at(curFrame).sprite);
    shadowSprite->setPixmap(getShadow(settings.itemBehaviours.at(curItem).frames.at(curFrame).sprite));
  }

  if(settings.sound && settings.itemBehaviours.at(curItem).frames.at(curFrame).soundBuffer != nullptr) {
    if(settings.itemBehaviours.at(curItem).pitchLock) {
      soundMixer.playSound(settings.itemBehaviours.at(curItem).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings.desktopWidth * 2.0 - 1.0, 1.0);
    } else {
      soundMixer.playSound(settings.itemBehaviours.at(curItem).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings.desktopWidth * 2.0 - 1.0,
                           0.95 + QRandomGenerator::global()->bounded(100) / 1000.0);
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
  if(frameTime <= 5) {
    frameTime = 5;
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

  animTimer.start(frameTime, Qt::PreciseTimer, this);
}

void Item::moveItem(int dX, int dY, const bool &flipped)
{
  sanityCheck();
  
  int maxX = QApplication::desktop()->width() - size;
  int minY = 0;
  int maxY = QApplication::desktop()->height() - size;

  if(dX == 666) {
    dX = QRandomGenerator::global()->bounded(maxX);
  } else {
    if(flipped) {
      dX *= -1;
    }
    dX = pos().x() + (dX * ceil(size / 32.0));
  }
  if(dY == 666) {
    dY = QRandomGenerator::global()->bounded(maxY);
  } else {
    dY = pos().y() + (dY * ceil(size / 32.0));
  }
  
  move(dX, dY);
  // if item is outside borders
  if(falling && (pos().y() > maxY || pos().y() < minY)) {
    hVel *= 0.2;
    vVel = 0.0;
  }
}

void Item::sanityCheck()
{
  int minX = - size;
  int maxX = QApplication::desktop()->width();
  int minY = 0;
  int maxY = QApplication::desktop()->height() - height();

  // Make sure Item is not located outside boundaries
  if(pos().y() < minY) {
    move(pos().x(), minY);
  }
  if(pos().y() > maxY) {
    move(pos().x(), maxY);
  }
  if(pos().x() > maxX) {
    move(minX, pos().y());
  }
  if(pos().x() < minX) {
    move(maxX, pos().y());
  }
}

int Item::getDistance(const QPoint &p)
{
  int xA = p.x();
  int yA = p.y();
  int xB = pos().x() + (size / 2);
  int yB = pos().y() + (size / 2);

  return sqrt((yB - yA) * (yB - yA) + (xB - xA) * (xB - xA));
}

int Item::getSector(const QPoint &p)
{
  // Center coordinate of Item
  QPoint b(pos().x() + (size / 2), pos().y() + (size / 2));

  // First find seg coordinate on x
  int xScale = abs(b.y() - p.y()) * 2;
  double xScaleSeg = xScale / 3.0;
  double xZero = b.x() - (xScale / 2.0);
  int xSeg = -1;
  if(p.x() < xZero + xScaleSeg) {
    xSeg = 0;
  } else if(p.x() < xZero + (xScaleSeg * 2)) {
    xSeg = 1;
  } else {
    xSeg = 2;
  }
  // Then find seg coordinate on y
  int yScale = abs(b.x() - p.x()) * 2;
  double yScaleSeg = yScale / 3.0;
  double yZero = b.y() - (yScale / 2.0);
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
  } else if(event->button() == Qt::MiddleButton) {
    destroy();
  }
}

void Item::mouseMoveEvent(QMouseEvent* event)
{
  if(grabbed && event->buttons().testFlag(Qt::LeftButton)) {
    this->move(event->globalPos().x() - size / 32.0 * 16.0, 
               event->globalPos().y() - size / 32.0 * 20.0);
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
    altitude = QCursor::pos().y();
  }
}

void Item::setCurFrame(const int &frame)
{
  curFrame = frame;
}

QPoint Item::getGlobalCenter()
{
  return QPoint(pos().x() + (width() / 2),
                pos().y() + (height() / 2));
}

QString Item::getReactionBehaviour()
{
  if(!settings.itemBehaviours.at(curItem).reactions.isEmpty()) {
    return settings.itemBehaviours.at(curItem).reactions.at(QRandomGenerator::global()->bounded(settings.itemBehaviours.at(curItem).reactions.count()));
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
    if(shadowSprite->isVisible()) {
      shadowSprite->hide();
    }
    moveItem(hVel, vVel);
    vVel += 0.5;
    if(pos().y() >= altitude) {
      move(pos().x(), altitude);
      if(vVel < 1.0) {
        falling = false;
      } else {
        hVel *= 0.5;
        vVel = (vVel * 0.5) * -1;
      }
    }
  } else {
    if(!shadowSprite->isVisible()) {
      shadowSprite->show();
    }
  }
  mouseHVel = (QCursor::pos().x() - oldCursor.x()) / 4.0;
  mouseVVel = (QCursor::pos().y() - oldCursor.y()) / 4.0;
  oldCursor = QCursor::pos();
}

void Item::interact(const Boris *boris)
{
  borisHyper = boris->getHyper();
  if(settings.itemBehaviours.at(curItem).interactLabel.isEmpty()) {
    destroy();
    return;
  }
  move(boris->pos().x() + settings.itemBehaviours.at(curItem).moveTo.x() * (size / 32),
       boris->pos().y() + settings.itemBehaviours.at(curItem).moveTo.y() * (size / 32));
  curFrame = settings.itemBehaviours.at(curItem).labels[settings.itemBehaviours.at(curItem).interactLabel];
  animTimer.start(0, Qt::PreciseTimer, this);
  setCursor(settings.getCursor("hover.png"));
  falling = false;
  grabbed = false;
  hVel = 0.0;
  vVel = 0.0;
  ignore = true;
  ignoreTimer.start();
}
