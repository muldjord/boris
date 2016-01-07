/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            boris.cpp
 *
 *  Tue Nov 26 16:56:00 CEST 2013
 *  Copyright 2013 Lars Muldjord
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

#include <stdio.h>
#include <math.h>
#include <QTimer>
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>

#include "boris.h"

//#define DEBUG

extern QSettings *settings;

Boris::Boris(QList<Behaviour> *behaviours, QWidget *parent) : QGraphicsView(parent)
{
  this->behaviours = behaviours;

  soundEnabled = settings->value("sound", "true").toBool();
  changeSize(settings->value("size", 32).toInt());
  int borisX = settings->value("boris_x", QApplication::desktop()->width() / 2).toInt();
  int borisY = settings->value("boris_y", QApplication::desktop()->height() / 2).toInt();
  if(borisY > QApplication::desktop()->height() - borisSize) {
    borisY = QApplication::desktop()->height() - borisSize;
  }
  
  move(borisX, borisY);

  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background:transparent");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  
  connect(this, SIGNAL(_moveBoris(int, int)), this, SLOT(moveBoris(int, int)));
  connect(this, SIGNAL(_showBoris()), this, SLOT(showBoris()));
  connect(this, SIGNAL(_hideBoris()), this, SLOT(hideBoris()));

  QGraphicsScene *scene = new QGraphicsScene();
  setScene(scene);
  sprite = scene->addPixmap(QPixmap());

  curFrame = 0;
  curBehav = 0;
  grabbed = false;
  falling = false;
  energy = 75 + qrand() % 25;
  hunger = qrand() % 25;
  bladder = qrand() % 25;
  social = 75 + qrand() % 25;
  fun = 75 + qrand() % 25;
  
  bMenu = new QMenu();
  bMenu->setTitle(tr("Behaviours"));
  QMenu *energyMenu = new QMenu(tr("Energy"), bMenu);
  energyMenu->setIcon(QIcon(":energy.png"));
  QMenu *hungerMenu = new QMenu(tr("Hunger"), bMenu);
  hungerMenu->setIcon(QIcon(":hunger.png"));
  QMenu *bladderMenu = new QMenu(tr("Toilet"), bMenu);
  bladderMenu->setIcon(QIcon(":bladder.png"));
  QMenu *socialMenu = new QMenu(tr("Social"), bMenu);
  socialMenu->setIcon(QIcon(":social.png"));
  QMenu *funMenu = new QMenu(tr("Fun"), bMenu);
  funMenu->setIcon(QIcon(":fun.png"));
  connect(energyMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(hungerMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(bladderMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(socialMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(funMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  for(int i = 0; i < behaviours->length(); ++i) {
    if(behaviours->at(i).file.left(1) != "_") {
      if(behaviours->at(i).category == "Energy") {
        energyMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Hunger") {
        hungerMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Bladder") {
        bladderMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Social") {
        socialMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Fun") {
        funMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      }
    }
  }
  bMenu->addMenu(energyMenu);
  bMenu->addMenu(hungerMenu);
  bMenu->addMenu(bladderMenu);
  bMenu->addMenu(socialMenu);
  bMenu->addMenu(funMenu);

  showStats = false;
  stats = new Stats(this);
  if(settings->value("stats", "true").toBool()) {
    stats->show();
    showStats = true;
  }
  
  connect(&behavTimer, SIGNAL(timeout()), this, SLOT(changeBehaviour()));
  behavTimer.setInterval((qrand() % 8000) + 1000);
  behavTimer.start();

  physicsTimer.setInterval(30);
  physicsTimer.setSingleShot(true);
  connect(&physicsTimer, SIGNAL(timeout()), this, SLOT(handlePhysics()));

  animTimer.setInterval(0);
  animTimer.setSingleShot(true);
  connect(&animTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
  animTimer.start();

  statTimer.setInterval(60000);
  connect(&statTimer, SIGNAL(timeout()), this, SLOT(statProgress()));
  statTimer.start();
  
  changeBehaviour("_dropped");

  setCursor(QCursor(QPixmap(":mouse_hover.png")));
}

Boris::~Boris()
{
  delete bMenu;
}

void Boris::changeBehaviour(QString behav, int time)
{
  // Check if Boris is dead... If so, don't do anything. :(
  if(behaviours->at(curBehav).file == "_drop_dead") {
    behavTimer.stop();
    statTimer.stop();
    // RIP
    return;
  }

  // Stop stat flashing no matter what
  if(behaviours->at(curBehav).file == "_energy" ||
     behaviours->at(curBehav).file == "_hunger" ||
     behaviours->at(curBehav).file == "_bladder" ||
     behaviours->at(curBehav).file == "_social" ||
     behaviours->at(curBehav).file == "_fun") {
    stats->flashStat("none");
  }

  // Check if there are behaviours in queue
  if(behavQueue.length() != 0 && behav == "") {
    behav = behavQueue.first();
    behavQueue.removeFirst();
  }

  // Stat check
  if(behav == "" && time == 0) {
    if(energy <= 50) {
      if(energy <= 5) {
        behav = "_sleep";
      } else if(qrand() % (100 - energy) > 75) {
        stats->flashStat("energy");
        behav = "_energy";
      }
    }
    if(hunger >= 50) {
      if(qrand() % hunger > 75) {
        stats->flashStat("hunger");
        behav = "_hunger";
      }
    }
    if(bladder >= 50) {
      if(bladder >= 95) {
        behav = "_weewee";
      } else if(qrand() % bladder > 75) {
        stats->flashStat("bladder");
        behav = "_bladder";
      }
    }
    if(social <= 50) {
      if(qrand() % (100 - social) > 75) {
        stats->flashStat("social");
        behav = "_social";
      }
    }
    if(fun <= 50) {
      if(qrand() % (100 - fun) > 75) {
        stats->flashStat("fun");
        behav = "_fun";
      }
    }
  }

  if(behav == "") {
    alreadyEvading = false;
  }
  
  // Pick random behaviour except sleep and weewee
  do {
    curBehav = (qrand() % (behaviours->size() - 23)) + 23;
  } while(behaviours->at(curBehav).file == "weewee" || behaviours->at(curBehav).file == "sleep");
  // If a specific behaviour is requested, use that instead of the random one
  if(behav != "") {
    //behav = "_fun"; // Use this to test behaviours
    for(int a = 0; a < behaviours->size(); ++a) {
      if(behaviours->at(a).file == behav) {
        curBehav = a;
      }
    }
  }

  if(time == 0) {
    behavTimer.setInterval((qrand() % 8000) + 1000);
  } else {
    behavTimer.setInterval(time);
  }
#ifdef DEBUG
  qDebug("Changing to behaviour '%d' titled '%s' for %d ms\n",
         curBehav, behaviours->at(curBehav).file.toStdString().c_str(),
         behavTimer.interval());
#endif

  // Applying behaviour stats to Boris
  energy += behaviours->at(curBehav).energy;
  hunger += behaviours->at(curBehav).hunger;
  bladder += behaviours->at(curBehav).bladder;
  social += behaviours->at(curBehav).social;
  fun += behaviours->at(curBehav).fun;
  
  curFrame = 0;
  if(behaviours->at(curBehav).oneShot) {
#ifdef DEBUG
    qDebug("Behaviour is oneShot, ignoring timeout\n");
#endif
    behavTimer.stop();
  } else {
    behavTimer.start();
  }
  animTimer.setInterval(0);
}

void Boris::nextFrame()
{
  sanityCheck();
  
  if(!falling && !grabbed && behaviours->at(curBehav).file != "_sleep" && behaviours->at(curBehav).file != "sleep") {
    QPoint p = QCursor::pos();
    int minX = borisSize * 2;
    int maxX = QApplication::desktop()->width() - borisSize * 2;
    int minY = borisSize * 2;
    int maxY = QApplication::desktop()->height() - borisSize * 2;
    if((this->pos().x() <= maxX && this->pos().x() >= minX &&
        this->pos().y() <= maxY && this->pos().y() >= minY) ||
       (p.x() > QApplication::desktop()->width() - borisSize / 2.0 ||
        p.x() < borisSize / 2.0 ||
        p.y() > QApplication::desktop()->height() - borisSize / 2.0 ||
        p.y() < borisSize / 2.0)) {
      if(p.x() > this->pos().x() + (borisSize / 2.0) - 100 && p.x() < this->pos().x() + (borisSize / 2.0) + 100 &&
         p.y() > this->pos().y() + (borisSize / 2.0) - 100 && p.y() < this->pos().y() + (borisSize / 2.0) + 100) {
        if(!alreadyEvading) {
          if(qrand() % 3 >= 2) {
            if(p.x() < this->pos().x() + (borisSize / 2.0)) {
              if(p.y() < this->pos().y() + (borisSize / 2.0) - 25) {
                changeBehaviour("_flee_right_down");
              } else if(p.y() > this->pos().y() + (borisSize / 2.0) + 25) {
                changeBehaviour("_flee_right_up");
              } else {
                changeBehaviour("_flee_right");
              }
            } else if(p.x() > this->pos().x() + (borisSize / 2.0)) {
              if(p.y() < this->pos().y() + (borisSize / 2.0) - 25) {
                changeBehaviour("_flee_left_down");
              } else if(p.y() > this->pos().y() + (borisSize / 2.0) + 25) {
                changeBehaviour("_flee_left_up");
              } else {
                changeBehaviour("_flee_left");
              }
            }
          }
        }
        alreadyEvading = true;
      } else {
        alreadyEvading = false;
      }
    }
  }
  
  if(curFrame >= behaviours->at(curBehav).behaviour.size()) {
    curFrame = 0;
    if(!behavTimer.isActive() && !grabbed) {
      behavTimer.start();
      changeBehaviour();
    }
  }
  sprite->setPixmap(behaviours->at(curBehav).behaviour.at(curFrame).sprite);
  if(soundEnabled && behaviours->at(curBehav).behaviour.at(curFrame).soundFx != NULL) {
    behaviours->at(curBehav).behaviour.at(curFrame).soundFx->play();
  }
  animTimer.setInterval(behaviours->at(curBehav).behaviour.at(curFrame).time);

  if(behaviours->at(curBehav).behaviour.at(curFrame).dx != 0 || behaviours->at(curBehav).behaviour.at(curFrame).dy != 0) {
  emit moveBoris(behaviours->at(curBehav).behaviour.at(curFrame).dx,
                 behaviours->at(curBehav).behaviour.at(curFrame).dy);
}

  if(behaviours->at(curBehav).behaviour.at(curFrame).show) {
#ifdef DEBUG
    qDebug("Telling Boris to show himself...\n");
#endif
    emit showBoris();
  }

  if(behaviours->at(curBehav).behaviour.at(curFrame).hide) {
#ifdef DEBUG
    qDebug("Telling Boris to hide...\n");
#endif
    emit hideBoris();
  }

  if(behaviours->at(curBehav).behaviour.at(curFrame).change) {
#ifdef DEBUG
    qDebug("Telling Boris to change behaviour...\n");
#endif
    changeBehaviour();
  }

  curFrame++;
  animTimer.start();
}

void Boris::moveBoris(int dX, int dY)
{
  sanityCheck();
  
  dX = ceil((double)borisSize / 32.0) * dX;
  dY = ceil((double)borisSize / 32.0) * dY;
  int minX = 0;
  int maxX = QApplication::desktop()->width() - borisSize;
  int minY = 0;
  int maxY = QApplication::desktop()->height() - borisSize;

  // Always move Boris, even outside borders. sanitycheck() will rectify later.
  move(this->pos().x() + dX,
       this->pos().y() + dY);
  stats->move(this->pos().x() + (borisSize / 2) - (stats->width() / 2), this->pos().y() - stats->height());
  if(stats->pos().y() < 0) {
    stats->move(this->pos().x() + (borisSize / 2) - (stats->width() / 2), this->pos().y() + borisSize);
  }
  // if Boris is outside borders
  if(this->pos().x() + dX > maxX || this->pos().x() + dX < minX ||
     this->pos().y() + dY > maxY || this->pos().y() + dY < minY) {
    if(falling) {
      // Physics velocity when hitting borders
      if(this->pos().x() + dX > maxX || this->pos().x() + dX < minX) {
        hVel *= -0.4;
        vVel *= 0.4;
      } else {
        hVel *= 0.2;
        vVel = 0.0;
      }
    } else {
      if(!grabbed) {
        changeBehaviour();
      }
    }
  }
}

void Boris::handleBehaviourChange(QAction* a) {
  for(int i = 0; i < behaviours->length(); ++i) {
    if(behaviours->at(i).title == a->text()) {
      behavQueue.append(behaviours->at(i).file);
    }
  }
}

void Boris::showBoris()
{
  show();
}

void Boris::hideBoris()
{
  hide();
}

void Boris::enterEvent(QEvent *event)
{
  event->accept();
  if(behaviours->at(curBehav).file != "_drop_dead" &&
     behaviours->at(curBehav).file != "_sleep" &&
     behaviours->at(curBehav).file != "sleep") {
    social += 5;
  }
  if(!showStats) {
    stats->show();
  }
}

void Boris::leaveEvent(QEvent *event)
{
  event->accept();
  if(!showStats) {
    stats->hide();
  }
}

void Boris::mousePressEvent(QMouseEvent* event)
{
  if(event->type() == QEvent::MouseButtonDblClick) {
  }
  if(event->button() == Qt::RightButton) {
    bMenu->exec(QCursor::pos());
  }
  if(event->button() == Qt::LeftButton) {
    if(behaviours->at(curBehav).file == "_sleep" || behaviours->at(curBehav).file == "sleep") {
      energy -= 100;
    }
    setCursor(QCursor(QPixmap(":mouse_grab.png")));
    grabbed = true;
    changeBehaviour("_grabbed", 100000);
    mMoving = true;
    this->move(event->globalPos().x() - (float)borisSize / 32.0 * 17.0, 
               event->globalPos().y() - (float)borisSize / 32.0 * 2.0);
    oldCursor = QCursor::pos();
    physicsTimer.start();
  }
}

void Boris::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons().testFlag(Qt::LeftButton) && mMoving) {
    this->move(event->globalPos().x() - (float)borisSize / 32.0 * 17.0, 
               event->globalPos().y() - (float)borisSize / 32.0 * 2.0);
    stats->move(this->pos().x() + (borisSize / 2) - (stats->width() / 2), this->pos().y() - stats->height());
    if(stats->pos().y() < 0) {
      stats->move(this->pos().x() + (borisSize / 2) - (stats->width() / 2), this->pos().y() + borisSize);
    }
  }
}

void Boris::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton) {
    setCursor(QCursor(QPixmap(":mouse_hover.png")));
    grabbed = false;
    mMoving = false;
    settings->setValue("boris_x", this->pos().x());
    settings->setValue("boris_y", this->pos().y());
    changeBehaviour("_falling", 20000);
    falling = true;
    alt = QCursor::pos().y() + 40;
  }
}


void Boris::changeSize(int newSize)
{
  borisSize = newSize;
  if(borisSize == 0) {
    borisSize = (qrand() % 64) + 32;
  }
  resetTransform();
  setFixedSize(borisSize, borisSize);
  scale((qreal)borisSize / 32.0, (qreal)borisSize / 32.0);
}

void Boris::soundEnable(bool enabled)
{
  soundEnabled = enabled;
}

void Boris::statsEnable(bool enabled)
{
  if(enabled) {
    stats->show();
  } else {
    stats->hide();
  }
  showStats = enabled;
}

void Boris::handlePhysics()
{
  if(falling && !grabbed) {
    moveBoris(hVel, vVel);
    vVel += 0.5;
    if(this->pos().y() < alt) {
      physicsTimer.start();
    } else {
      changeBehaviour("_landing");
      falling = false;
    }
  } else if (grabbed) {
    hVel = (QCursor::pos().x() - oldCursor.x()) / 4.0;
    vVel = (QCursor::pos().y() - oldCursor.y()) / 4.0;
#ifdef DEBUG
    qDebug("hVel is %f\n", hVel);
    qDebug("vVel is %f\n", vVel);
#endif
    oldCursor = QCursor::pos();
    physicsTimer.start();
  }
}

void Boris::earthquake()
{
  if(!falling && !grabbed) {
    changeBehaviour("_falling", 20000);
    falling = true;
    alt = this->pos().y();
    vVel = ((qrand() % 10) * -1) - 5;
    hVel = qrand() % 20 - 11;
    physicsTimer.start();
  }
}

void Boris::walkUp()
{
  changeBehaviour("_zombie_walk_up");
}

void Boris::walkDown()
{
  changeBehaviour("_zombie_walk_down");
}

void Boris::walkLeft()
{
  changeBehaviour("_zombie_walk_left");
}

void Boris::walkRight()
{
  changeBehaviour("_zombie_walk_right");
}

void Boris::statProgress()
{
  energy -= qrand() % 4;
  hunger += qrand() % 4;
  //bladder += 0;
  social -= qrand() % 4;
  fun -= qrand() % 20;
}

void Boris::sanityCheck()
{
  int minX = 0;
  int maxX = QApplication::desktop()->width() - borisSize;
  int minY = 0;
  int maxY = QApplication::desktop()->height() - borisSize;

  // Make sure Boris is not located outside boundaries
  if(this->pos().y() < minY) {
    move(this->pos().x(), minY);
  }
  if(this->pos().y() > maxY) {
    move(this->pos().x(), maxY);
  }
  if(this->pos().x() > maxX) {
    move(maxX, this->pos().y());
  }
  if(this->pos().x() < minX) {
    move(minX, this->pos().y());
  }

  // Make sure Boris altitude is not outside bottom boundary
  if(alt > maxY) {
    alt = maxY;
  }

  // Make sure Boris stats aren't out of bounds
  if(energy <= 0) {
    energy = 0;
  }
  if(energy >= 100) {
    energy = 100;
  }
  if(hunger <= 0) {
    hunger = 0;
  }
  if(hunger >= 100) {
    hunger = 100;
  }
  if(bladder <= 0) {
    bladder = 0;
  }
  if(bladder >= 100) {
    bladder = 100;
  }
  if(social <= 0) {
    social = 0;
  }
  if(social >= 100) {
    social = 100;
  }
  if(fun <= 0) {
    fun = 0;
  }
  if(fun >= 100) {
    fun = 100;
  }

  stats->updateStats(energy, hunger, bladder, social, fun);

  // Check if Boris is dying or is already dead
  if(behaviours->at(curBehav).file != "_drop_dead") {
    if(energy + social + fun + ((hunger - 100) *-1) < 50) {
      qDebug("Boris has died... RIP!\n");
      changeBehaviour("_drop_dead");
    }
  }
}
