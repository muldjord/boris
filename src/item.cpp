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

extern QList<Item*> itemList;
extern QList<Behaviour> itemBehaviours;
extern SoundMixer soundMixer;

Item::Item(const int &x, const int &y, const int &size, const QString &item, Settings *settings, const bool &ignore) :
  size(size)
{
  this->settings = settings;
  this->ignore = ignore;
  
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background:transparent");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  
  for(int a = 0; a < itemBehaviours.count(); ++a) {
    if(itemBehaviours.at(a).file == item) {
      curItem = a;
      break;
    }
  }

  setScene(new QGraphicsScene);
  scene()->setSceneRect(0.0, 0.0, 32, 32 + 1); // + 1 to make room for shadow

  origShadow.load(":shadow.png");
  shadowSprite = this->scene()->addPixmap(origShadow);
  shadowSprite->setOpacity(0.35);

  itemSprite = this->scene()->addPixmap(QPixmap());
  itemSprite->setPos(0, 0);

  scriptSprite = this->scene()->addPixmap(QPixmap());
  scriptSprite->setPos(0, 0); // To make room for shadow

  setCursor(Qt::OpenHandCursor);

  setFixedSize(size, size + (size / 32)); // To make room for shadow
  scale(size / 32.0, size / 32.0);
  move(x, y);
  if(settings->itemTimeout > 0) {
    QTimer::singleShot(settings->itemTimeout * 1000, this, &Item::destroy);
  }

  if(itemBehaviours.at(curItem).allowFlip && QRandomGenerator::global()->bounded(2)) {
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
  if(itemBehaviours.at(curItem).defines.contains("init")) {
    int stop = 0;
    runScript(stop, true);
  }

  animTimer.start(0, Qt::PreciseTimer, this);

  QTimer::singleShot(60000, this, &Item::dontIgnore);
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
  QImage shadow = origShadow.toImage().scaled(shadowWidth, origShadow.height());
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

  if(!drawing) {
    scriptImage.fill(Qt::transparent);
  }

  ScriptHandler scriptHandler(&scriptImage, &drawing, settings, nullptr, itemBehaviours.at(curItem).labels, itemBehaviours.at(curItem).defines, scriptVars, pos(), size);
  connect(&scriptHandler, &ScriptHandler::setCurFrame, this, &Item::setCurFrame);
  if(init) {
    scriptHandler.runScript(stop, itemBehaviours.at(curItem).defines["init"]);
  } else {
    scriptHandler.runScript(stop, itemBehaviours.at(curItem).frames.at(curFrame).script);
  }

  scriptSprite->setPixmap(QPixmap::fromImage(scriptImage));
}

void Item::timerEvent(QTimerEvent *)
{
  if(stopAndDestroy) {
    stopAndDestroy = false;
    destroy();
    return;
  }

  sanityCheck();
  
  if(curFrame >= itemBehaviours.at(curItem).frames.count()) {
    curFrame = 0;
  }

  if(flipFrames) {
    QImage flipped = itemBehaviours.at(curItem).frames.at(curFrame).sprite.toImage().mirrored(true, false);
    itemSprite->setPixmap(QPixmap::fromImage(flipped));
    shadowSprite->setPixmap(getShadow(QPixmap::fromImage(flipped)));
  } else {
    itemSprite->setPixmap(itemBehaviours.at(curItem).frames.at(curFrame).sprite);
    shadowSprite->setPixmap(getShadow(itemBehaviours.at(curItem).frames.at(curFrame).sprite));
  }

  if(settings->sound && itemBehaviours.at(curItem).frames.at(curFrame).soundBuffer != nullptr) {
    if(itemBehaviours.at(curItem).pitchLock) {
      soundMixer.playSound(itemBehaviours.at(curItem).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings->desktopWidth * 2.0 - 1.0, 1.0);
    } else {
      soundMixer.playSound(itemBehaviours.at(curItem).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings->desktopWidth * 2.0 - 1.0,
                           0.95 + QRandomGenerator::global()->bounded(100) / 1000.0);
    }
  }

  if(itemBehaviours.at(curItem).frames.at(curFrame).dx != 0 ||
     itemBehaviours.at(curItem).frames.at(curFrame).dy != 0) {
    moveItem(itemBehaviours.at(curItem).frames.at(curFrame).dx,
              itemBehaviours.at(curItem).frames.at(curFrame).dy,
              flipFrames);
  }

  int frameTime = itemBehaviours.at(curItem).frames.at(curFrame).time;
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
}

void Item::sanityCheck()
{
  int minX = - size;
  int maxX = QApplication::desktop()->width();
  int minY = 0 - (size / 2);
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
  } else if(p.x() >= xZero + (xScaleSeg * 2)) {
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
  } else if(p.y() >= yZero + (yScaleSeg * 2)) {
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
    setCursor(Qt::ClosedHandCursor);
  } else if(event->button() == Qt::MiddleButton) {
    destroy();
  }

}

void Item::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons().testFlag(Qt::LeftButton)) {
    this->move(event->globalPos().x() - size / 32.0 * 16.0, 
               event->globalPos().y() - size / 32.0 * 20.0);
  }

}

void Item::mouseReleaseEvent(QMouseEvent* event)
{
  grabbed = false;
  if(event->button() == Qt::LeftButton) {
    setCursor(Qt::OpenHandCursor);
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
  if(!itemBehaviours.at(curItem).reactions.isEmpty()) {
    return itemBehaviours.at(curItem).reactions.at(QRandomGenerator::global()->bounded(itemBehaviours.at(curItem).reactions.count()));
  }
  return QString();
}

QString Item::getCategory()
{
  return itemBehaviours.at(curItem).category;
}

void Item::destroy()
{
  // Remove from collide list
  for(int a = 0; a < itemList.count(); ++a) {
    if(itemList.at(a) == this) {
      itemList.removeAt(a);
    }
  }
  delete this;
}

void Item::dontIgnore()
{
  ignore = false;
}
