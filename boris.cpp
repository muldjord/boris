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
#include <QBitmap>

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
  
  QGraphicsScene *scene = new QGraphicsScene();
  setScene(scene);
  sprite = scene->addPixmap(QPixmap());
  origDirt.load(":dirt.png");
  dirt = scene->addPixmap(origDirt);
  dirt->setOpacity(0.0);
  
  curFrame = 0;
  curBehav = 0;
  grabbed = false;
  falling = false;
  boris = NULL;
  energy = 75 + qrand() % 25;
  hunger = qrand() % 25;
  bladder = qrand() % 25;
  hygiene = 100;
  social = 75 + qrand() % 25;
  fun = 75 + qrand() % 25;
  independence = settings->value("independence", "0").toInt();
  energyQueue = 0;
  hungerQueue = 0;
  bladderQueue = 0;
  socialQueue = 0;
  funQueue = 0;
  hygieneQueue = 0;
  
  createBehavMenu();

  showStats = false;
  stats = new Stats(this);
  if(settings->value("stats", "true").toBool()) {
    stats->show();
    showStats = true;
  }

  staticBehavs = 0;
  // Figure out how many static behaviours there are
  for(int i = 0; i < behaviours->length(); ++i) {
    if(behaviours->at(i).file.left(1) == "_") {
      staticBehavs++;
    }
  }

  connect(&behavTimer, SIGNAL(timeout()), this, SLOT(changeBehaviour()));
  behavTimer.setInterval((qrand() % 8000) + 1000);
  behavTimer.start();

  physicsTimer.setInterval(30);
  //physicsTimer.setSingleShot(true);
  connect(&physicsTimer, SIGNAL(timeout()), this, SLOT(handlePhysics()));
  physicsTimer.start();
  
  animTimer.setInterval(0);
  animTimer.setSingleShot(true);
  connect(&animTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
  animTimer.start();

  statTimer.setInterval(60000);
  connect(&statTimer, SIGNAL(timeout()), this, SLOT(statProgress()));
  statTimer.start();

  statQueueTimer.setInterval(200);
  connect(&statQueueTimer, SIGNAL(timeout()), this, SLOT(statQueueProgress()));
  statQueueTimer.start();
  
  setCursor(QCursor(QPixmap(":mouse_hover.png")));
}

Boris::~Boris()
{
  delete stats;
  delete bMenu;
}

void Boris::stopTimers()
{
  physicsTimer.stop();
  animTimer.stop();
  statTimer.stop();
  statQueueTimer.stop();
}

void Boris::startTimers()
{
  physicsTimer.start();
  animTimer.start();
  statTimer.start();
  statQueueTimer.start();
}

void Boris::createBehavMenu()
{
  bMenu = new QMenu();
  bMenu->setTitle(tr("Behaviours"));
  QMenu *movementMenu = new QMenu(tr("Movement"), bMenu);
  movementMenu->setIcon(QIcon(":movement.png"));
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
  QMenu *hygieneMenu = new QMenu(tr("Hygiene"), bMenu);
  hygieneMenu->setIcon(QIcon(":hygiene.png"));
  connect(movementMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(energyMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(hungerMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(bladderMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(socialMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(funMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(hygieneMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  for(int i = 0; i < behaviours->length(); ++i) {
    if(behaviours->at(i).file.left(1) != "_") {
      if(behaviours->at(i).category == "Movement") {
        movementMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Energy") {
        energyMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Hunger") {
        hungerMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Bladder") {
        bladderMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Social") {
        socialMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Fun") {
        funMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      } else if(behaviours->at(i).category == "Hygiene") {
        hygieneMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      }
    }
  }
  bMenu->addMenu(movementMenu);
  bMenu->addMenu(energyMenu);
  bMenu->addMenu(hungerMenu);
  bMenu->addMenu(bladderMenu);
  bMenu->addMenu(hygieneMenu);
  bMenu->addMenu(socialMenu);
  bMenu->addMenu(funMenu);
}


QString Boris::chooseFromCategory(QString category)
{
  QList<QString> b;
  for(int i = 0; i < behaviours->length(); ++i) {
    if(behaviours->at(i).category == category) {
      b.append(behaviours->at(i).file);
    }
  }
  int chosen = qrand() % b.length();
  for(int i = 0; i < behaviours->length(); ++i) {
    if(behaviours->at(i).file == b.at(chosen)) {
      chosen = i;
      break;
    }
  }
  return behaviours->at(chosen).file;
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

  // Make sure we don't get any timeouts while this function is running to avoid weird bugs
  stopTimers();

  // Stop stat flashing if ending a stat attention behaviour
  if(behaviours->at(curBehav).file == "_energy" ||
     behaviours->at(curBehav).file == "_hunger" ||
     behaviours->at(curBehav).file == "_bladder" ||
     behaviours->at(curBehav).file == "_social" ||
     behaviours->at(curBehav).file == "_fun" ||
     behaviours->at(curBehav).file == "_hygiene") {
    stats->flashStat("none");
  }

  // Check if already colliding with other Boris
  if(boris != NULL) {
    int borisSizeB = boris->borisSize;
    int xB = boris->pos().x();
    int yB = boris->pos().y();
    double hypotenuse = sqrt((yB - this->pos().y()) * (yB - this->pos().y()) + (xB - this->pos().x()) * (xB - this->pos().x()));
    if(hypotenuse > borisSizeB * 2) {
      boris = NULL;
    }
  }

  // Check if there are behaviours in queue
  if(behavQueue.length() != 0 && behav == "") {
    behav = behavQueue.first();
    behavQueue.removeFirst();
  }

  // Stat check
  if(behav == "" && time == 0) {
    if(energy <= 50) {
      if(qrand() % (100 - energy) > independence) {
        stats->flashStat("energy");
        behav = "_energy";
      } else if(energy <= 10) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Energy");
        }
      }
    }
    if(hunger >= 50) {
      if(qrand() % hunger > independence) {
        stats->flashStat("hunger");
        behav = "_hunger";
      } else if(hunger >= 90) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Hunger");
        }
      }
    }
    if(bladder >= 50) {
      if(qrand() % bladder > independence) {
        stats->flashStat("bladder");
        behav = "_bladder";
      } else if(bladder >= 90) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Bladder");
        }
      }
    }
    if(social <= 50) {
      if(qrand() % (100 - social) > independence) {
        stats->flashStat("social");
        behav = "_social";
      } else if(social <= 10) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Social");
        }
      }
    }
    if(fun <= 50) {
      if(qrand() % (100 - fun) > independence) {
        stats->flashStat("fun");
        behav = "_fun";
      } else if(fun <= 10) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Fun");
        }
      }
    }
    if(hygiene <= 50) {
      if(qrand() % (100 - hygiene) > independence) {
        stats->flashStat("none");
        behav = "_hygiene";
      } else if(hygiene <= 10) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Hygiene");
        }
      }
    }
  }

  if(behav == "") {
    alreadyEvading = false;
  }
  
  // Pick random behaviour except sleep and weewee
  do {
    curBehav = (qrand() % (behaviours->size() - staticBehavs)) + staticBehavs;
  } while(behaviours->at(curBehav).file == "weewee" || behaviours->at(curBehav).file == "sleep");
  // If a specific behaviour is requested, use that instead of the random one
  if(behav != "") {
    //behav = "teleport"; // Use this to test behaviours
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
  energyQueue = behaviours->at(curBehav).energy;
  hungerQueue = behaviours->at(curBehav).hunger;
  bladderQueue = behaviours->at(curBehav).bladder;
  socialQueue = behaviours->at(curBehav).social;
  funQueue = behaviours->at(curBehav).fun;
  hygieneQueue = behaviours->at(curBehav).hygiene;
  
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

  // Start all timers again
  startTimers();
}

void Boris::nextFrame()
{
  sanityCheck();
  
  if(curFrame >= behaviours->at(curBehav).behaviour.size()) {
    curFrame = 0;
    if(!behavTimer.isActive() && !grabbed) {
      behavTimer.start();
      changeBehaviour();
    }
  }
  sprite->setPixmap(behaviours->at(curBehav).behaviour.at(curFrame).sprite);

  QPixmap dirtPixmap(origDirt);
  dirtPixmap.setMask(behaviours->at(curBehav).behaviour.at(curFrame).sprite.createMaskFromColor(QColor(0, 0, 0, 0)));
  dirt->setPixmap(dirtPixmap);
  
  if(soundEnabled && behaviours->at(curBehav).behaviour.at(curFrame).soundFx != NULL) {
    behaviours->at(curBehav).behaviour.at(curFrame).soundFx->play();
  }
  animTimer.setInterval(behaviours->at(curBehav).behaviour.at(curFrame).time);

  if(behaviours->at(curBehav).behaviour.at(curFrame).dx != 0 || behaviours->at(curBehav).behaviour.at(curFrame).dy != 0) {
    moveBoris(behaviours->at(curBehav).behaviour.at(curFrame).dx,
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

  if(!falling && !grabbed && behaviours->at(curBehav).file != "sleep") {
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
      int xA = p.x();
      int yA = p.y();
      int xB = this->pos().x();
      int yB = this->pos().y();
      double hypotenuse = sqrt((yB - yA) * (yB - yA) + (xB - xA) * (xB - xA));
      if(hypotenuse < borisSize * 2) {
        if(!alreadyEvading) {
          if(fabs(hVel) > 10.0 || fabs(vVel) > 10.0) {
            double fleeAngle = atan2((this->pos().y() + (borisSize / 2.0)) - p.y(),
                                     p.x() - (this->pos().x() + (borisSize / 2.0))
                                     ) * 180.0 / 3.1415927;
            if (fleeAngle < 0) {
              fleeAngle += 360;
            } else if (fleeAngle > 360) {
              fleeAngle -= 360;
            }
            if((fleeAngle >= 0.0 && fleeAngle < 22.5) || (fleeAngle >= 337.5 && fleeAngle < 360.0)) {
              changeBehaviour("_flee_left");
            } else if(fleeAngle >= 22.5 && fleeAngle < 67.5) {
              changeBehaviour("_flee_left_down");
            } else if(fleeAngle >= 67.5 && fleeAngle < 112.5) {
              changeBehaviour("_flee_down");
            } else if(fleeAngle >= 112.5 && fleeAngle < 157.5) {
              changeBehaviour("_flee_right_down");
            } else if(fleeAngle >= 157.5 && fleeAngle < 202.5) {
              changeBehaviour("_flee_right");
            } else if(fleeAngle >= 202.5 && fleeAngle < 247.5) {
              changeBehaviour("_flee_right_up");
            } else if(fleeAngle >= 247.5 && fleeAngle < 292.5) {
              changeBehaviour("_flee_up");
            } else if(fleeAngle >= 292.5 && fleeAngle < 337.5) {
              changeBehaviour("_flee_left_up");
            }
          }
        }
        alreadyEvading = true;
      } else {
        alreadyEvading = false;
      }
    }
  }
}

void Boris::moveBoris(int dX, int dY)
{
  sanityCheck();

  int minX = 0;
  int maxX = QApplication::desktop()->width() - borisSize;
  int minY = 0;
  int maxY = QApplication::desktop()->height() - borisSize;

  if(dX == 666) { // If dX == 666 we are meant to move Boris randomly
    dX = qrand() % maxX - this->pos().x();
  } else {
    // Multiply delta by the factor or Boris' current size
    dX *= ceil((double)borisSize / 32.0);
  }
  if(dY == 666) { // If dX == 666 we are meant to move Boris randomly
    dY = qrand() % maxY - this->pos().y();
  } else {
    // Multiply delta by the factor or Boris' current size
    dY *= ceil((double)borisSize / 32.0);
  }

  // Always move Boris, even outside borders. sanitycheck() will rectify later.
  move(this->pos().x() + dX, this->pos().y() + dY);
  stats->move(this->pos().x() + (borisSize / 2) - (stats->width() / 2),
              this->pos().y() - stats->height());
  if(stats->pos().y() < 0) {
    stats->move(this->pos().x() + (borisSize / 2) - (stats->width() / 2),
                this->pos().y() + borisSize);
  }
  // if Boris is outside borders
  if(this->pos().x() > maxX || this->pos().x() < minX ||
     this->pos().y() > maxY || this->pos().y() < minY) {
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
    if(behaviours->at(curBehav).file == "sleep" && energy < 100) {
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

void Boris::setIndependence(int value)
{
  independence = value;
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
      //physicsTimer.start();
    } else {
      changeBehaviour("_landing");
      falling = false;
    }
  }
  if(!falling) {
    hVel = (QCursor::pos().x() - oldCursor.x()) / 4.0;
    vVel = (QCursor::pos().y() - oldCursor.y()) / 4.0;
#ifdef DEBUG
    qDebug("hVel is %f\n", hVel);
    qDebug("vVel is %f\n", vVel);
#endif
    oldCursor = QCursor::pos();
    //physicsTimer.start();
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
    //physicsTimer.start();
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
  hygiene -= qrand() % 2;
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
  if(hygiene <= 0) {
    hygiene = 0;
  }
  if(hygiene >= 100) {
    hygiene = 100;
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

void Boris::statQueueProgress()
{
  if(energyQueue > 0) {
    energyQueue--;
    energy++;
  }
  if(energyQueue < 0) {
    energyQueue++;
    energy--;
  }
  if(hungerQueue > 0) {
    hungerQueue--;
    hunger++;
  }
  if(hungerQueue < 0) {
    hungerQueue++;
    hunger--;
  }
  if(bladderQueue > 0) {
    bladderQueue--;
    bladder++;
  }
  if(bladderQueue < 0) {
    bladderQueue += 2;
    bladder -= 2;
  }
  if(socialQueue > 0) {
    socialQueue--;
    social++;
  }
  if(socialQueue < 0) {
    socialQueue++;
    social--;
  }
  if(funQueue > 0) {
    funQueue--;
    fun++;
  }
  if(funQueue < 0) {
    funQueue++;
    fun--;
  }
  if(hygieneQueue > 0) {
    hygieneQueue--;
    hygiene++;
  }
  if(hygieneQueue < 0) {
    hygieneQueue++;
    hygiene--;
  }

  dirt->setOpacity(0.35 - ((qreal)hygiene) * 0.01);
}

void Boris::collide(Boris *b)
{
  if(!falling && !grabbed && behaviours->at(curBehav).file != "sleep" && boris == NULL) {
    boris = b;

    double socialAngle = atan2(this->pos().y() - boris->pos().y(), boris->pos().x() - this->pos().x()) * 180.0 / 3.1415927;
    if (socialAngle < 0) {
      socialAngle += 360;
    } else if (socialAngle > 360) {
      socialAngle -= 360;
    }
    
    if((socialAngle >= 0.0 && socialAngle < 22.5) || (socialAngle >= 337.5 && socialAngle < 360.0)) {
      changeBehaviour("_casual_wave_right");
    } else if(socialAngle >= 22.5 && socialAngle < 67.5) {
      changeBehaviour("_casual_wave_right_up");
    } else if(socialAngle >= 67.5 && socialAngle < 112.5) {
      changeBehaviour("_casual_wave_up");
    } else if(socialAngle >= 112.5 && socialAngle < 157.5) {
      changeBehaviour("_casual_wave_left_up");
    } else if(socialAngle >= 157.5 && socialAngle < 202.5) {
      changeBehaviour("_casual_wave_left");
    } else if(socialAngle >= 202.5 && socialAngle < 247.5) {
      changeBehaviour("_casual_wave_left_down");
    } else if(socialAngle >= 247.5 && socialAngle < 292.5) {
      changeBehaviour("_casual_wave_down");
    } else if(socialAngle >= 292.5 && socialAngle < 337.5) {
      changeBehaviour("_casual_wave_right_down");
    }      
      
  }
}
