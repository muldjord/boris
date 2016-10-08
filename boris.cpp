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
#include <QScreen>

#include "boris.h"

//#define DEBUG

extern QSettings *settings;

Boris::Boris(QList<Behaviour> *behaviours, QList<Behaviour> *weathers, QWidget *parent) : QGraphicsView(parent)
{
  this->behaviours = behaviours;
  this->weathers = weathers;

  vVel = 0.0;
  hVel = 0.0;
  mouseVVel = 0.0;
  mouseHVel = 0.0;

  int borisX = settings->value("boris_x", QApplication::desktop()->width() / 2).toInt();
  int borisY = settings->value("boris_y", QApplication::desktop()->height() / 2).toInt();
  if(borisY > QApplication::desktop()->height() - height()) {
    borisY = QApplication::desktop()->height() - height();
  }
  
  move(borisX + (qrand() % 200) - 100, borisY + (qrand() % 200) - 100);

  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background:transparent");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  
  setScene(new QGraphicsScene);
  sprite = this->scene()->addPixmap(QPixmap());
  origDirt.load(":dirt.png");
  dirt = this->scene()->addPixmap(origDirt);
  dirt->setOpacity(0.0);
  origBruises.load(":bruises.png");
  bruises = this->scene()->addPixmap(origBruises);
  bruises->setOpacity(0.0);

  weatherSprite = this->scene()->addPixmap(QPixmap(32, 32));
  weatherSprite->setPos(0, 0 - 16);
  weatherSprite->hide();
  
  curWeather = 0;
  curWeatherFrame = 0;
  curFrame = 0;
  curBehav = 0;
  timeFactor = settings->value("time_factor", "1").toInt();
  grabbed = false;
  falling = false;
  timeForWeather = 0;
  
  createBehavMenu();

  alreadyEvading = false;
  // Set initial stats with some randomization
  int health = 100;
  int energy = 50 + qrand() % 25;
  int hunger = (qrand() % 25) + 15;
  int bladder = (qrand() % 25) + 15;
  int hygiene = 100;
  int social = 50 + qrand() % 25;
  int fun = 50 + qrand() % 25;
  healthQueue = 0;
  energyQueue = 0;
  hungerQueue = 0;
  bladderQueue = 0;
  socialQueue = 0;
  funQueue = 0;
  hygieneQueue = 0;
  stats = new Stats(health, energy, hunger, bladder, social, fun, hygiene, this);
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
  connect(&physicsTimer, SIGNAL(timeout()), this, SLOT(handlePhysics()));
  physicsTimer.start();

  animTimer.setInterval(0);
  animTimer.setSingleShot(true);
  connect(&animTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
  animTimer.start();

  weatherTimer.setSingleShot(true);
  connect(&weatherTimer, SIGNAL(timeout()), this, SLOT(nextWeatherFrame()));
  
  statTimer.setInterval(60000 / timeFactor);
  connect(&statTimer, SIGNAL(timeout()), this, SLOT(statProgress()));
  statTimer.start();

  statQueueTimer.setInterval(200 / timeFactor);
  connect(&statQueueTimer, SIGNAL(timeout()), this, SLOT(statQueueProgress()));
  statQueueTimer.start();
  
  setCursor(QCursor(QPixmap(":mouse_hover.png")));

  updateBoris(settings->value("size", 32).toInt(),
              settings->value("weather", "false").toBool(),
              settings->value("stats", "false").toBool(),
              settings->value("sound", "true").toBool(),
              settings->value("independence", "60").toInt());
}

Boris::~Boris()
{
  delete stats;
  delete bMenu;
}

void Boris::createBehavMenu()
{
  bMenu = new QMenu();
  bMenu->setTitle(tr("Behaviours"));
  QMenu *healthMenu = new QMenu(tr("Health"), bMenu);
  healthMenu->setIcon(QIcon(":health.png"));
  QMenu *energyMenu = new QMenu(tr("Energy"), bMenu);
  energyMenu->setIcon(QIcon(":energy.png"));
  QMenu *hungerMenu = new QMenu(tr("Food"), bMenu);
  hungerMenu->setIcon(QIcon(":hunger.png"));
  QMenu *bladderMenu = new QMenu(tr("Toilet"), bMenu);
  bladderMenu->setIcon(QIcon(":bladder.png"));
  QMenu *hygieneMenu = new QMenu(tr("Hygiene"), bMenu);
  hygieneMenu->setIcon(QIcon(":hygiene.png"));
  QMenu *socialMenu = new QMenu(tr("Social"), bMenu);
  socialMenu->setIcon(QIcon(":social.png"));
  QMenu *funMenu = new QMenu(tr("Fun"), bMenu);
  funMenu->setIcon(QIcon(":fun.png"));
  QMenu *movementMenu = new QMenu(tr("Movement"), bMenu);
  movementMenu->setIcon(QIcon(":movement.png"));
  connect(healthMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(energyMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(hungerMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(bladderMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(hygieneMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(socialMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(funMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
  connect(movementMenu, SIGNAL(triggered(QAction*)), this, SLOT(handleBehaviourChange(QAction*)));
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
      } else if(behaviours->at(i).category == "Health") {
        healthMenu->addAction(QIcon(":" + behaviours->at(i).category.toLower() + ".png"), behaviours->at(i).title);
      }
    }
  }
  bMenu->addMenu(healthMenu);
  bMenu->addMenu(energyMenu);
  bMenu->addMenu(hungerMenu);
  bMenu->addMenu(bladderMenu);
  bMenu->addMenu(hygieneMenu);
  bMenu->addMenu(socialMenu);
  bMenu->addMenu(funMenu);
  bMenu->addMenu(movementMenu);
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
  // Check if it is time to show the weather again
  timeForWeather++;
  if(timeForWeather >= 30) {
    timeForWeather = 0;
    showWeather();
    return;
  }

  // Check if Boris is dead... If so, don't do anything. :(
  if(behaviours->at(curBehav).file == "_drop_dead") {
    behavTimer.stop();
    statTimer.stop();
    // RIP
    return;
  }

  // Stop stat flashing
  stats->flashStat("none");

  // Check if already colliding with other Boris
  if(boris != NULL) {
    int xB = boris->pos().x();
    int yB = boris->pos().y();
    double hypotenuse = sqrt((yB - this->pos().y()) * (yB - this->pos().y()) + (xB - this->pos().x()) * (xB - this->pos().x()));
    if(hypotenuse > 128) {
      // Reset Boris pointer to make Boris wave at a new Boris again
      boris = NULL;
    }
  }

  // Check if there are behaviours in queue, these are prioritized
  if(behav == "" && behavQueue.length() != 0) {
    behav = behavQueue.first();
    behavQueue.removeFirst();
  }

  // Process the AI if no forced behaviour is set
  if(behav == "" && time == 0) {
    processAi(behav, time);
  }
  
  // Pick random behaviour except sleep, weewee and patch_up
  do {
    curBehav = (qrand() % (behaviours->size() - staticBehavs)) + staticBehavs;
  } while(behaviours->at(curBehav).file == "weewee" || behaviours->at(curBehav).file == "sleep" || behaviours->at(curBehav).file == "patch_up");
  // If a specific behaviour is requested, use that instead of the random one
  if(behav != "") {
    //behav = "_health"; // Use this to test behaviours
    for(int a = 0; a < behaviours->size(); ++a) {
      if(behaviours->at(a).file == behav) {
        curBehav = a;
      }
    }
  }

  if(time == 0) {
    behavTimer.setInterval(((qrand() % 7000) + 5000) / timeFactor);
  } else {
    behavTimer.setInterval(time / timeFactor);
  }
#ifdef DEBUG
  qDebug("Changing to behaviour '%d' titled '%s' for %d ms\n",
         curBehav, behaviours->at(curBehav).file.toStdString().c_str(),
         behavTimer.interval());
#endif

  // Applying behaviour stats to Boris
  healthQueue = behaviours->at(curBehav).health;
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
}

void Boris::nextFrame()
{
  sanityCheck();
  
  if(curFrame >= behaviours->at(curBehav).behaviour.length()) {
    if(behaviours->at(curBehav).file == "_drop_dead") {
      return;
    }
    curFrame = 0;
    if(!behavTimer.isActive() && !grabbed) {
      behavTimer.start();
      changeBehaviour();
    }
  }
  sprite->setPixmap(behaviours->at(curBehav).behaviour.at(curFrame).sprite);

  QBitmap mask = behaviours->at(curBehav).behaviour.at(curFrame).sprite.createMaskFromColor(QColor(0, 0, 0, 0));

  QPixmap dirtPixmap(origDirt);
  dirtPixmap.setMask(mask);
  dirt->setPixmap(dirtPixmap);

  QPixmap bruisesPixmap(origBruises);
  bruisesPixmap.setMask(mask);
  bruises->setPixmap(bruisesPixmap);

  if(soundEnabled && behaviours->at(curBehav).behaviour.at(curFrame).soundFx != NULL) {
    behaviours->at(curBehav).behaviour.at(curFrame).soundFx->play();
  }
  animTimer.setInterval(behaviours->at(curBehav).behaviour.at(curFrame).time / timeFactor);

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

  curFrame++;
  animTimer.start();
}

void Boris::moveBoris(int dX, int dY)
{
  sanityCheck();

  int minX = 0;
  int maxX = QApplication::desktop()->width() - borisSize;
  int minY = 0 - (borisSize / 2);
  int maxY = QApplication::desktop()->height() - height();

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
                this->pos().y() + borisSize + borisSize / 3);
  }
  // if Boris is outside borders
  if(this->pos().x() > maxX || this->pos().x() < minX ||
     this->pos().y() > maxY || this->pos().y() < minY) {
    if(falling) {
      stats->deltaHealth(-5); // It hurts to hit the borders
      // Physics velocity when hitting borders
      if(this->pos().x() + dX > maxX || this->pos().x() + dX < minX) {
        hVel *= -0.4;
        vVel *= 0.4;
      } else {
        hVel *= 0.2;
        vVel = 0.0;
      }
    } else if(!grabbed) {
      changeBehaviour();
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
  if(event->button() == Qt::RightButton) {
    bMenu->exec(QCursor::pos());
  }
  if(event->button() == Qt::LeftButton) {
    if(behaviours->at(curBehav).file == "sleep" && stats->getEnergy() < 100) {
      stats->deltaEnergy(-100);
    }
    setCursor(QCursor(QPixmap(":mouse_grab.png")));
    grabbed = true;
    changeBehaviour("_grabbed", 100000);
    mMoving = true;
    this->move(event->globalPos().x() - (float)borisSize / 32.0 * 17.0, 
               event->globalPos().y() - (float)borisSize / 32.0 * 16.0);
    oldCursor = QCursor::pos();
  }
}

void Boris::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons().testFlag(Qt::LeftButton) && mMoving) {
    this->move(event->globalPos().x() - (float)borisSize / 32.0 * 17.0, 
               event->globalPos().y() - (float)borisSize / 32.0 * 16.0);
    stats->move(this->pos().x() + (borisSize / 2) - (stats->width() / 2), this->pos().y() - stats->height());
    if(stats->pos().y() < 0) {
      stats->move(this->pos().x() + (borisSize / 2) - (stats->width() / 2), this->pos().y() + borisSize + borisSize / 3);
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
    hVel = mouseHVel;
    vVel = mouseVVel;
    alt = QCursor::pos().y() + 40;
  }
}

void Boris::handlePhysics()
{
  if(falling && !grabbed) {
    moveBoris(hVel, vVel);
    vVel += 0.5;
    if(this->pos().y() < alt) {
    } else {
      changeBehaviour("_landing");
      falling = false;
    }
  }
  mouseHVel = (QCursor::pos().x() - oldCursor.x()) / 4.0;
  mouseVVel = (QCursor::pos().y() - oldCursor.y()) / 4.0;
#ifdef DEBUG
  qDebug("mouseHVel is %f\n", mouseHVel);("mouseVVel is %f\n", mouseVVel);
#endif
  oldCursor = QCursor::pos();

  if(!falling && !grabbed &&
     !behaviours->at(curBehav).doNotDisturb) {
    QPoint p = QCursor::pos();
    int xA = p.x();
    int yA = p.y();
    int xB = this->pos().x() + (borisSize / 2);
    int yB = this->pos().y() + (borisSize / 2);
    double hypotenuse = sqrt((yB - yA) * (yB - yA) + (xB - xA) * (xB - xA));
    if(hypotenuse < borisSize * 3) {
      if(!alreadyEvading) {
        if(fabs(mouseHVel) > 20.0 || fabs(mouseVVel) > 20.0) {
          double fleeAngle = atan2((this->pos().y() + (borisSize / 2.0)) - p.y(),
                                   p.x() - (this->pos().x() + (borisSize / 2.0))
                                   ) * 180.0 / 3.1415927;
          if (fleeAngle < 0) {
            fleeAngle += 360;
          } else if (fleeAngle > 360) {
            fleeAngle -= 360;
          }
          if((fleeAngle >= 0.0 && fleeAngle < 22.5) || (fleeAngle >= 337.5 && fleeAngle < 360.0)) {
            changeBehaviour("_flee_left", (qrand() % 2000) + 1000);
          } else if(fleeAngle >= 22.5 && fleeAngle < 67.5) {
            changeBehaviour("_flee_left_down", (qrand() % 2000) + 1000);
          } else if(fleeAngle >= 67.5 && fleeAngle < 112.5) {
            changeBehaviour("_flee_down", (qrand() % 2000) + 1000);
          } else if(fleeAngle >= 112.5 && fleeAngle < 157.5) {
            changeBehaviour("_flee_right_down", (qrand() % 2000) + 1000);
          } else if(fleeAngle >= 157.5 && fleeAngle < 202.5) {
            changeBehaviour("_flee_right", (qrand() % 2000) + 1000);
          } else if(fleeAngle >= 202.5 && fleeAngle < 247.5) {
            changeBehaviour("_flee_right_up", (qrand() % 2000) + 1000);
          } else if(fleeAngle >= 247.5 && fleeAngle < 292.5) {
            changeBehaviour("_flee_up", (qrand() % 2000) + 1000);
          } else if(fleeAngle >= 292.5 && fleeAngle < 337.5) {
            changeBehaviour("_flee_left_up", (qrand() % 2000) + 1000);
          }
        } else if(stats->getFun() > 40 && qrand() % 100 >= 75) {
          changeBehaviour(chooseFromCategory("Social"));
        }
      }
      alreadyEvading = true;
    } else {
      alreadyEvading = false;
    }
  }
}

void Boris::earthquake()
{
  if(!falling && !grabbed) {
    changeBehaviour("_falling", 20000);
    falling = true;
    vVel = ((qrand() % 10) * -1) - 5;
    hVel = qrand() % 20 - 11;
    alt = this->pos().y();
  }
}

void Boris::teleport()
{
  if(!falling && !grabbed) {
    changeBehaviour("teleport");
  }
}

void Boris::statProgress()
{
  stats->deltaEnergy(- qrand() % 4);
  stats->deltaHunger(qrand() % 4);
  stats->deltaSocial(- qrand() % 4);
  stats->deltaHygiene(- qrand() % 2);
  stats->deltaFun(- qrand() % 20);
  // Nothing needed for 'health' and 'bladder'
}

void Boris::sanityCheck()
{
  int minX = 0;
  int maxX = QApplication::desktop()->width() - borisSize;
  int minY = 0 - (borisSize / 2);
  int maxY = QApplication::desktop()->height() - height();

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

  // Check if Boris is dying or is already dead
  if(behaviours->at(curBehav).file != "_drop_dead") {
    if(stats->getHealth() <= 2 || stats->getEnergy() + stats->getSocial() + stats->getFun() + ((stats->getHunger() - 100) *-1) < 50) {
      qDebug("Boris has died... RIP!\n");
      statQueueTimer.stop();
      dirt->setOpacity(0.0);
      bruises->setOpacity(0.0);
      changeBehaviour("_drop_dead");
    }
  }
}

void Boris::statQueueProgress()
{
  if(healthQueue > 0) {
    healthQueue -= 2;
    stats->deltaHealth(2);
  }
  if(healthQueue < 0) {
    healthQueue++;
    stats->deltaHealth(-1);
  }
  if(energyQueue > 0) {
    energyQueue--;
    stats->deltaEnergy(1);
  }
  if(energyQueue < 0) {
    energyQueue++;
    stats->deltaEnergy(-1);
  }
  if(hungerQueue > 0) {
    hungerQueue--;
    stats->deltaHunger(1);
  }
  if(hungerQueue < 0) {
    hungerQueue++;
    stats->deltaHunger(-1);
  }
  if(bladderQueue > 0) {
    bladderQueue--;
    stats->deltaBladder(1);
  }
  if(bladderQueue < 0) {
    bladderQueue += 2;
    stats->deltaBladder(-2);
  }
  if(socialQueue > 0) {
    socialQueue--;
    stats->deltaSocial(1);
  }
  if(socialQueue < 0) {
    socialQueue++;
    stats->deltaSocial(-1);
  }
  if(funQueue > 0) {
    funQueue--;
    stats->deltaFun(1);
  }
  if(funQueue < 0) {
    funQueue++;
    stats->deltaFun(-1);
  }
  if(hygieneQueue > 0) {
    hygieneQueue -= 2;
    stats->deltaHygiene(2);
  }
  if(hygieneQueue < 0) {
    hygieneQueue++;
    stats->deltaHygiene(-1);
  }

  dirt->setOpacity(0.35 - ((qreal)stats->getHygiene()) * 0.01);
  bruises->setOpacity(0.75 - ((qreal)stats->getHealth()) * 0.01);
  stats->updateStats();
}

// Used by 'other Boris' to determine whether to flee or greet
int Boris::getHygiene()
{
  return stats->getHygiene();
}

void Boris::collide(Boris *b)
{
  if(this->pos().y() > b->pos().y()) {
    // Bring this Boris to the front, because he is considered closer in 3D space
    raise();
    setFocus();
  }
  
  if(!falling && !grabbed && !behaviours->at(curBehav).doNotDisturb && boris == NULL) {
    boris = b;
    
    double approachAngle = atan2(this->pos().y() - boris->pos().y(), boris->pos().x() - this->pos().x()) * 180.0 / 3.1415927;
    if (approachAngle < 0) {
      approachAngle += 360;
    } else if (approachAngle > 360) {
      approachAngle -= 360;
    }
    
    if((approachAngle >= 0.0 && approachAngle < 22.5) || (approachAngle >= 337.5 && approachAngle < 360.0)) {
      if(boris->getHygiene() >= 15) {
        changeBehaviour("_casual_wave_right");
      } else {
        changeBehaviour("_flee_left", (qrand() % 2000) + 1500);
      }
    } else if(approachAngle >= 22.5 && approachAngle < 67.5) {
      if(boris->getHygiene() >= 15) {
        changeBehaviour("_casual_wave_right_up");
      } else {
        changeBehaviour("_flee_left_down", (qrand() % 2000) + 1500);
      }
    } else if(approachAngle >= 67.5 && approachAngle < 112.5) {
      if(boris->getHygiene() >= 15) {
        changeBehaviour("_casual_wave_up");
      } else {
        changeBehaviour("_flee_down", (qrand() % 2000) + 1500);
      }
    } else if(approachAngle >= 112.5 && approachAngle < 157.5) {
      if(boris->getHygiene() >= 15) {
        changeBehaviour("_casual_wave_left_up");
      } else {
        changeBehaviour("_flee_right_down", (qrand() % 2000) + 1500);
      }
    } else if(approachAngle >= 157.5 && approachAngle < 202.5) {
      if(boris->getHygiene() >= 15) {
        changeBehaviour("_casual_wave_left");
      } else {
        changeBehaviour("_flee_right", (qrand() % 2000) + 1500);
      }
    } else if(approachAngle >= 202.5 && approachAngle < 247.5) {
      if(boris->getHygiene() >= 15) {
        changeBehaviour("_casual_wave_left_down");
      } else {
        changeBehaviour("_flee_right_up", (qrand() % 2000) + 1500);
      }
    } else if(approachAngle >= 247.5 && approachAngle < 292.5) {
      if(boris->getHygiene() >= 15) {
        changeBehaviour("_casual_wave_down");
      } else {
        changeBehaviour("_flee_up", (qrand() % 2000) + 1500);
      }
    } else if(approachAngle >= 292.5 && approachAngle < 337.5) {
      if(boris->getHygiene() >= 15) {
        changeBehaviour("_casual_wave_right_down");
      } else {
        changeBehaviour("_flee_left_up", (qrand() % 2000) + 1500);
      }
    }      
  }
}

void Boris::processVision()
{
  int border = 2;
  QImage vision = QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId(), pos().x() - border, pos().y() - border, borisSize + border * 2, borisSize + border *2).toImage();

  bool wallDetect;
  QRgb wallColor;

  // Check for wall to the west
  wallDetect = true;
  wallColor = vision.pixel(0, border);
  for(int a = border; a < borisSize + border; ++a) {
    if(vision.pixel(0, a) != wallColor ||
       vision.pixel(1, a) == wallColor) {
      wallDetect = false;
      break;
    }
  }
  if(wallDetect) {
    moveBoris(1, 0);
    changeBehaviour();
    return;
  }

  // Check for wall to the east
  wallDetect = true;
  wallColor = vision.pixel(vision.width() - 1, border);
  for(int a = border; a < borisSize + border; ++a) {
    if(vision.pixel(vision.width() - 1, a) != wallColor ||
       vision.pixel(vision.width() - 2, a) == wallColor) {
      wallDetect = false;
      break;
    }
  }
  if(wallDetect) {
    moveBoris(-1, 0);
    changeBehaviour();
    return;
  }

  // Check for wall to the north
  wallDetect = true;
  wallColor = vision.pixel(border, 0);
  for(int a = border; a < borisSize + border; ++a) {
    if(vision.pixel(a, 0) != wallColor ||
       vision.pixel(a, 1) == wallColor) {
      wallDetect = false;
      break;
    }
  }
  if(wallDetect) {
    moveBoris(0, 1);
    changeBehaviour();
    return;
  }

  // Check for wall to the south
  wallDetect = true;
  wallColor = vision.pixel(border, vision.height() - 1);
  for(int a = border; a < borisSize + border; ++a) {
    if(vision.pixel(a, vision.height() - 1) != wallColor ||
       vision.pixel(a, vision.height() - 2) == wallColor) {
      wallDetect = false;
      break;
    }
  }
  if(wallDetect) {
    moveBoris(0, -1);
    changeBehaviour();
    return;
  }
}

void Boris::processAi(QString &behav, int &time)
{
  if(behav == "" && time == 0 && qrand() % 10 >= 3 && behaviours->at(curBehav).file.contains("casual_walk_")) {
    time = qrand() % 1500 + 500;
    if(behaviours->at(curBehav).file == "casual_walk_up") {
      if(qrand() % 2) {
        behav = "casual_walk_left_up";
      } else {
        behav = "casual_walk_right_up";
      }
    } else if(behaviours->at(curBehav).file == "casual_walk_right_up") {
      if(qrand() % 2) {
        behav = "casual_walk_up";
      } else {
        behav = "casual_walk_right";
      }
    } else if(behaviours->at(curBehav).file == "casual_walk_right") {
      if(qrand() % 2) {
        behav = "casual_walk_right_up";
      } else {
        behav = "casual_walk_right_down";
      }
    } else if(behaviours->at(curBehav).file == "casual_walk_right_down") {
      if(qrand() % 2) {
        behav = "casual_walk_right";
      } else {
        behav = "casual_walk_down";
      }
    } else if(behaviours->at(curBehav).file == "casual_walk_down") {
      if(qrand() % 2) {
        behav = "casual_walk_right_down";
      } else {
        behav = "casual_walk_left_down";
      }
    } else if(behaviours->at(curBehav).file == "casual_walk_left_down") {
      if(qrand() % 2) {
        behav = "casual_walk_down";
      } else {
        behav = "casual_walk_left";
      }
    } else if(behaviours->at(curBehav).file == "casual_walk_left") {
      if(qrand() % 2) {
        behav = "casual_walk_left_down";
      } else {
        behav = "casual_walk_left_up";
      }
    } else if(behaviours->at(curBehav).file == "casual_walk_left_up") {
      if(qrand() % 2) {
        behav = "casual_walk_left";
      } else {
        behav = "casual_walk_up";
      }
    }
  }

  // Stat check
  if(behav == "" && time == 0) {
    if(stats->getEnergy() <= 50) {
      if(qrand() % (100 - stats->getEnergy()) > independence) {
        stats->flashStat("energy");
        behav = "_energy";
      } else if(stats->getEnergy() <= 10) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Energy");
        }
      }
    }
    if(stats->getHunger() >= 50) {
      if(qrand() % stats->getHunger() > independence) {
        stats->flashStat("hunger");
        behav = "_hunger";
      } else if(stats->getHunger() >= 90) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Hunger");
        }
      }
    }
    if(stats->getBladder() >= 50) {
      if(qrand() % stats->getBladder() > independence) {
        stats->flashStat("bladder");
        behav = "_bladder";
      } else if(stats->getBladder() >= 90) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Bladder");
        }
      }
    }
    if(stats->getSocial() <= 50) {
      if(qrand() % (100 - stats->getSocial()) > independence) {
        stats->flashStat("social");
        behav = "_social";
      } else if(stats->getSocial() <= 10) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Social");
        }
      }
    }
    if(stats->getFun() <= 50) {
      if(qrand() % (100 - stats->getFun()) > independence) {
        stats->flashStat("fun");
        behav = "_fun";
      } else if(stats->getFun() <= 10) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Fun");
        }
      }
    }
    if(stats->getHygiene() <= 50) {
      if(qrand() % (100 - stats->getHygiene()) > independence) {
        stats->flashStat("none");
        behav = "_hygiene";
      } else if(stats->getHygiene() <= 10) {
        if(qrand() % 100 < independence) {
          behav = chooseFromCategory("Hygiene");
        }
      }
    }
    if(stats->getHealth() <= 50) {
      if(qrand() % (150 - stats->getHealth()) > independence) {
        stats->flashStat("none");
        behav = "_health";
      }
    }
  }
}

void Boris::updateBoris(int newSize, bool alwaysWeather, bool statsEnable, bool soundEnable, int newIndependence)
{
  // Reset Boris pointer
  boris = NULL;

  // Set new size
  borisSize = newSize;
  if(borisSize == 0) {
    borisSize = (qrand() % 65) + 32;
  }
  resetTransform();
  setFixedSize(borisSize, borisSize + borisSize / 2.0);
  scale((qreal)borisSize / 32.0, (qreal)borisSize / 32.0);

  // Set new independence value
  independence = newIndependence;

  // Enable or disable sound
  soundEnabled = soundEnable;

  // Show or hide stats
  if(alwaysWeather) {
    weatherSprite->show();
  } else {
    weatherSprite->hide();
  }

  // Show or hide stats
  if(statsEnable) {
    stats->show();
  } else {
    stats->hide();
  }
  showStats = statsEnable;
}

void Boris::setWeatherType(QString type, double temp)
{
  curTemp = temp;
  for(int a = 0; a < weathers->length(); ++a) {
    if(weathers->at(a).file == type) {
      curWeather = a;
      break;
    }
  }
  curWeatherFrame = 0;
  weatherTimer.setInterval(0);
  weatherTimer.start();
}

void Boris::nextWeatherFrame()
{
  weatherSprite->setPixmap(weathers->at(curWeather).behaviour.at(curWeatherFrame).sprite);
  weatherTimer.setInterval(weathers->at(curWeather).behaviour.at(curWeatherFrame).time);
  curWeatherFrame++;
  if(curWeatherFrame >= weathers->at(curWeather).behaviour.length()) {
    curWeatherFrame = 0;
  }
  weatherTimer.start();
}

void Boris::showWeather()
{
  curWeatherFrame = 0;
  weatherTimer.setInterval(0);
  weatherTimer.start();

  QString type = weathers->at(curWeather).file;

  if(!settings->value("weather", "false").toBool()) {
    weatherSprite->show();
    QTimer::singleShot(30000, this, SLOT(hideWeather()));
  }
  if(!falling && !grabbed) {
    if((type == "01d" || type == "02d") && curTemp > 15.0) {
      behavQueue.append("sunglasses");
    } else if(type == "09d" || type == "09n" || type == "10d" || type == "10n") {
      behavQueue.append("_umbrella");
    } else if(type == "11d" || type == "11n") {
      changeBehaviour("_lightning");
    } else if(type == "13d" || type == "13n") {
      behavQueue.append("_freezing");
    }
  }
}

void Boris::hideWeather()
{
  weatherSprite->hide();
}
