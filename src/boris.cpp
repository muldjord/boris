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

#include "boris.h"
#include "scripthandler.h"

#include "SFML/Audio.hpp"

#include <stdio.h>
#include <math.h>
#include <QTimer>
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
#include <QBitmap>
#include <QScreen>
#include <QTime>

constexpr int STATTIMER = 200;
constexpr double PI = 3.1415927;
constexpr int ANNOYMAX = 42;

extern QList<Behaviour> behaviours;
extern QList<Behaviour> weathers;

Boris::Boris(Settings *settings)
{
  this->settings = settings;
  
  int borisX = settings->borisX;
  int borisY = settings->borisY;
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
  // Set the scene size for correct scaling when changing Boris' size through updateBoris()
  scene()->setSceneRect(0.0, 0.0, 32, 48);

  origShadow.load(":shadow.png");
  shadowSprite = this->scene()->addPixmap(origShadow);
  shadowSprite->setOpacity(0.35);

  borisSprite = this->scene()->addPixmap(QPixmap());
  borisSprite->setPos(0, 15); // To make room for shadow

  scriptSprite = this->scene()->addPixmap(QPixmap());
  scriptSprite->setPos(0, 15); // To make room for shadow

  origDirt.load(":dirt.png");
  dirtSprite = this->scene()->addPixmap(origDirt);
  dirtSprite->setOpacity(0.0);
  dirtSprite->setPos(0, 15); // To make room for shadow

  origBruises.load(":bruises.png");
  bruisesSprite = this->scene()->addPixmap(origBruises);
  bruisesSprite->setOpacity(0.0);
  bruisesSprite->setPos(0, 15); // To make room for shadow

  weatherSprite = this->scene()->addPixmap(QPixmap());
  weatherSprite->setPos(0, 0);
  weatherSprite->hide();
  
  curWeather = 0;
  curWeatherFrame = 0;
  curFrame = 0;
  curBehav = 0;
  grabbed = false;
  falling = false;
  isAlive = true;
  flipFrames = false;
  alreadyEvading = false;
  timeForWeather = 0;
  tooLateForLoo = 0;
  
  createBehavMenu();

  // Set initial stats with some randomization
  int hyper = 0;
  int health = 100;
  int energy = 50 + qrand() % 25;
  int hunger = 50 + qrand() % 25;
  int bladder = 50 + qrand() % 25;
  int hygiene = 100;
  int social = 50 + qrand() % 25;
  int fun = 50 + qrand() % 25;
  hyperQueue = 0;
  healthQueue = 0;
  energyQueue = 0;
  hungerQueue = 0;
  bladderQueue = 0;
  socialQueue = 0;
  funQueue = 0;
  hygieneQueue = 0;
  stats = new Stats(settings, hyper, health, energy, hunger, bladder, social, fun, hygiene, this);
  chatter = new Chatter(settings, this);
  
  staticBehavs = 0;
  // Figure out how many static behaviours there are
  for(const auto &behaviour: behaviours) {
    if(behaviour.file.left(1) == "_") {
      staticBehavs++;
    }
  }

  connect(&behavTimer, &QTimer::timeout, this, &Boris::nextBehaviour);
  behavTimer.setInterval((qrand() % 8000) + 1000);
  behavTimer.start();

  physicsTimer.setInterval(30);
  connect(&physicsTimer, &QTimer::timeout, this, &Boris::handlePhysics);
  physicsTimer.start();

  animTimer.setInterval(0);
  animTimer.setSingleShot(true);
  connect(&animTimer, &QTimer::timeout, this, &Boris::nextFrame);
  animTimer.start();

  weatherTimer.setSingleShot(true);
  connect(&weatherTimer, &QTimer::timeout, this, &Boris::nextWeatherFrame);
  
  statTimer.setInterval(30000);
  connect(&statTimer, &QTimer::timeout, this, &Boris::statProgress);
  statTimer.start();

  statQueueTimer.setInterval(STATTIMER);
  statQueueTimer.setSingleShot(true);
  connect(&statQueueTimer, &QTimer::timeout, this, &Boris::statQueueProgress);
  statQueueTimer.start();
  
  interactionsTimer.setInterval(2000);
  interactionsTimer.setSingleShot(true);
  connect(&interactionsTimer, &QTimer::timeout, this, &Boris::balanceInteractions);
  interactionsTimer.start();

  setCursor(QCursor(QPixmap(":mouse_hover.png")));

  updateBoris();
}

Boris::~Boris()
{
  delete stats;
  delete chatter;
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
  QMenu *iddqdMenu = new QMenu(tr("Iddqd"), bMenu);
  iddqdMenu->setIcon(QIcon(":iddqd.png"));
  connect(healthMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  connect(energyMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  connect(hungerMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  connect(bladderMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  connect(hygieneMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  connect(socialMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  connect(funMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  connect(movementMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  connect(iddqdMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  for(int i = 0; i < behaviours.length(); ++i) {
    if(behaviours.at(i).file.left(1) != "_") {
      if(behaviours.at(i).category == "Movement") {
        movementMenu->addAction(QIcon(":" + behaviours.at(i).category.toLower() + ".png"), behaviours.at(i).title);
      } else if(behaviours.at(i).category == "Energy") {
        energyMenu->addAction(QIcon(":" + behaviours.at(i).category.toLower() + ".png"), behaviours.at(i).title);
      } else if(behaviours.at(i).category == "Hunger") {
        hungerMenu->addAction(QIcon(":" + behaviours.at(i).category.toLower() + ".png"), behaviours.at(i).title);
      } else if(behaviours.at(i).category == "Bladder") {
        bladderMenu->addAction(QIcon(":" + behaviours.at(i).category.toLower() + ".png"), behaviours.at(i).title);
      } else if(behaviours.at(i).category == "Social") {
        socialMenu->addAction(QIcon(":" + behaviours.at(i).category.toLower() + ".png"), behaviours.at(i).title);
      } else if(behaviours.at(i).category == "Fun") {
        funMenu->addAction(QIcon(":" + behaviours.at(i).category.toLower() + ".png"), behaviours.at(i).title);
      } else if(behaviours.at(i).category == "Hygiene") {
        hygieneMenu->addAction(QIcon(":" + behaviours.at(i).category.toLower() + ".png"), behaviours.at(i).title);
      } else if(behaviours.at(i).category == "Health") {
        healthMenu->addAction(QIcon(":" + behaviours.at(i).category.toLower() + ".png"), behaviours.at(i).title);
      } else {
        iddqdMenu->addAction(QIcon(":iddqd.png"), behaviours.at(i).title);
      }
    } else {
      iddqdMenu->addAction(QIcon(":iddqd.png"), behaviours.at(i).title);
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
  if(settings->iddqd) {
    bMenu->addMenu(iddqdMenu);
  }
}

QString Boris::getFileFromCategory(QString category)
{
  QList<QString> b;
  for(int i = 0; i < behaviours.length(); ++i) {
    if(behaviours.at(i).category == category) {
      b.append(behaviours.at(i).file);
    }
  }
  int chosen = qrand() % b.length();
  for(int i = 0; i < behaviours.length(); ++i) {
    if(behaviours.at(i).file == b.at(chosen)) {
      chosen = i;
      break;
    }
  }
  return behaviours.at(chosen).file;
}

int Boris::getIdxFromCategory(QString category)
{
  QList<QString> b;
  for(const auto &behav: behaviours) {
    if(behav.category == category) {
      b.append(behav.file);
    }
  }
  int chosen = qrand() % b.length();
  for(int i = 0; i < behaviours.length(); ++i) {
    if(behaviours.at(i).file == b.at(chosen)) {
      chosen = i;
      break;
    }
  }
  return chosen;
}

void Boris::nextBehaviour()
{
  changeBehaviour();
}

void Boris::changeBehaviour(QString behav, int time)
{
  // Reset all script variables
  scriptVars.clear();

  // Clear script image canvas
  scriptImage.fill(Qt::transparent);
  drawing = false;
  
  // If Boris has died, just return
  if(!isAlive) {
    return;
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
  
  // Pick random behaviour but rule out certain behavs such as weewee and sleep
  // Bias towards behavs from 'Idle' and 'Locomotion' categories to make Boris less erratic
  if(qrand() % 10 >= 3) {
    if(qrand() % 70 >= stats->getEnergy()) {
      curBehav = getIdxFromCategory("Idle");
    } else {
      curBehav = getIdxFromCategory("Locomotion");
    }
  } else {
    do {
      curBehav = (qrand() % (behaviours.size() - staticBehavs)) + staticBehavs;
    } while(behaviours.at(curBehav).file == "weewee" ||
            behaviours.at(curBehav).file == "toilet_visit" ||
            behaviours.at(curBehav).file == "sleep" ||
            behaviours.at(curBehav).file == "shower" ||
            behaviours.at(curBehav).file == "wash_hands" ||
            behaviours.at(curBehav).file == "patch_up");
  }

  // If a specific behaviour is requested, use that
  if(!behav.isEmpty()) {
    //behav = "_health"; // Use this to test behaviours
    for(int a = 0; a < behaviours.size(); ++a) {
      if(behaviours.at(a).file == behav) {
        curBehav = a;
      }
    }
  }
  
  // Hide speech bubble in case Boris was grabbed or otherwise stopped in the middle of speaking
  chatter->hide();
  // Check for chatter
  if(behaviours.at(curBehav).file == "chatter") {
    QPair<QString, int> selectedChatter = chatter->initChatter(this->pos().x(), this->pos().y(), size);
    for(int a = 0; a < behaviours.size(); ++a) {
      if(behaviours.at(a).file == selectedChatter.first) {
        curBehav = a;
      }
    }
    time = selectedChatter.second;
  }

  if(time == 0) {
    time = qrand() % 7000 + 5000;
  }
  time = time - ((double)time / 100.0 * stats->getHyper());
  behavTimer.setInterval(time);

#ifdef DEBUG
  qInfo("Changing to behaviour '%d' titled '%s' for %d ms\n",
         curBehav, behaviours.at(curBehav).file.toStdString().c_str(),
         behavTimer.interval());
#endif

  // Applying behaviour stats to Boris
  hyperQueue += behaviours.at(curBehav).hyper;
  healthQueue += behaviours.at(curBehav).health;
  energyQueue += behaviours.at(curBehav).energy;
  hungerQueue += behaviours.at(curBehav).hunger;
  bladderQueue += behaviours.at(curBehav).bladder;
  socialQueue += behaviours.at(curBehav).social;
  funQueue += behaviours.at(curBehav).fun;
  hygieneQueue += behaviours.at(curBehav).hygiene;
  
  curFrame = 0;
  if(behaviours.at(curBehav).allowFlip && qrand() %2) {
    flipFrames = true;
  } else {
    flipFrames = false;
  }
  if(behaviours.at(curBehav).oneShot) {
#ifdef DEBUG
    qInfo("Behaviour is oneShot, ignoring timeout\n");
#endif
    behavTimer.stop();
  } else {
    behavTimer.start();
  }
  animTimer.setInterval(0);
}

QPixmap Boris::getShadow(const QPixmap &sprite)
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
  shadowSprite->setPos(firstLeft, bottom - 15);
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

void Boris::runScript()
{
  // Update current stat variables for scripting use
  scriptVars["energy"] = stats->getEnergy();
  scriptVars["health"] = stats->getHealth();
  scriptVars["hyper"] = stats->getHyper();
  scriptVars["hunger"] = stats->getHunger();
  scriptVars["bladder"] = stats->getBladder();
  scriptVars["social"] = stats->getSocial();
  scriptVars["fun"] = stats->getFun();
  scriptVars["hygiene"] = stats->getHygiene();
  scriptVars["yvel"] = mouseVVel;
  scriptVars["xvel"] = mouseHVel;

  if(!drawing) {
    scriptImage.fill(Qt::transparent);
  }
  ScriptHandler scriptHandler(&scriptImage, &drawing, this, settings, stats);
  int stop = 0; // Will be true if a goto or break is run
  scriptHandler.runScript(stop, behaviours.at(curBehav).frames.at(curFrame).script);

  scriptSprite->setPixmap(QPixmap::fromImage(scriptImage));

  if(stop == 1) {
    return;
  } else if(stop == 2) {
    behavTimer.stop();
    nextBehaviour();
    return;
  }
  curFrame++;
}

void Boris::nextFrame()
{
  sanityCheck();
  
  if(curFrame >= behaviours.at(curBehav).frames.count()) {
    if(!isAlive) {
      return;
    }
    curFrame = 0;
    if(!behavTimer.isActive() && !grabbed) {
      behavTimer.start();
      changeBehaviour();
    }
  }

  QTime frameTimer;
  frameTimer.start();

  QBitmap mask = behaviours.at(curBehav).frames.at(curFrame).sprite.mask();

  QPixmap dirtPixmap(origDirt);
  dirtPixmap.setMask(mask);

  QPixmap bruisesPixmap(origBruises);
  bruisesPixmap.setMask(mask);

  if(falling) {
    if(shadowSprite->isVisible()) {
      shadowSprite->hide();
    }
  } else {
    shadowSprite->show();
  }
  if(flipFrames) {
    QImage flipped = behaviours.at(curBehav).frames.at(curFrame).sprite.toImage().mirrored(true, false);
    borisSprite->setPixmap(QPixmap::fromImage(flipped));
    shadowSprite->setPixmap(getShadow(QPixmap::fromImage(flipped)));
    dirtSprite->setPixmap(QPixmap::fromImage(dirtPixmap.toImage().mirrored(true, false)));
    bruisesSprite->setPixmap(QPixmap::fromImage(bruisesPixmap.toImage().mirrored(true, false)));
  } else {
    borisSprite->setPixmap(behaviours.at(curBehav).frames.at(curFrame).sprite);
    shadowSprite->setPixmap(getShadow(behaviours.at(curBehav).frames.at(curFrame).sprite));
    dirtSprite->setPixmap(dirtPixmap);
    bruisesSprite->setPixmap(bruisesPixmap);
  }

  if(settings->sound && behaviours.at(curBehav).frames.at(curFrame).soundBuffer != nullptr) {
    if(behaviours.at(curBehav).pitchLock) {
      emit playSound(behaviours.at(curBehav).frames.at(curFrame).soundBuffer,
                     (float)this->pos().x() / (float)settings->desktopWidth * 2.0 - 1.0,
                     (stats->getHyper() / 60.0) + 1);
    } else {
      emit playSound(behaviours.at(curBehav).frames.at(curFrame).soundBuffer,
                     (float)this->pos().x() / (float)settings->desktopWidth * 2.0 - 1.0,
                     (stats->getHyper() / 60.0) + (0.95 + (qrand() % 100) / 1000.0));
    }
  }
  int frameTime = behaviours.at(curBehav).frames.at(curFrame).time;
  frameTime -= ((double)frameTime / 100.0 * stats->getHyper());
  int elapsedTime = frameTimer.elapsed();
  if(elapsedTime < frameTime) {
    frameTime -= elapsedTime;
  }
  if(frameTime <= 5) {
    frameTime = 5;
  }
  animTimer.setInterval(frameTime);

  if(behaviours.at(curBehav).frames.at(curFrame).dx != 0 ||
     behaviours.at(curBehav).frames.at(curFrame).dy != 0) {
    moveBoris(behaviours.at(curBehav).frames.at(curFrame).dx,
              behaviours.at(curBehav).frames.at(curFrame).dy,
              flipFrames);
  }

  runScript();
  animTimer.start();
}

void Boris::moveBoris(int dX, int dY, const bool &flipped, const bool &vision)
{
  sanityCheck();
  
  int minX = 0;
  int maxX = QApplication::desktop()->width() - size;
  int minY = 0 - (size / 2);
  int maxY = QApplication::desktop()->height() - height();

  if(dX == 666) {
    dX = qrand() % maxX;
  } else {
    if(flipped) {
      dX *= -1;
    }
    dX = this->pos().x() + (dX * ceil((double)size / 32.0));
  }
  if(dY == 666) {
    dY = qrand() % maxY;
  } else {
    dY = this->pos().y() + (dY * ceil((double)size / 32.0));
  }
  
  move(dX, dY);
  stats->move(this->pos().x() + (size / 2) - (stats->width() / 2),
              this->pos().y() - stats->height() + (size / 3));
  // Move stats below Boris when he's at the top of the screen
  if(stats->pos().y() < 0) {
    stats->move(this->pos().x() + (size / 2) - (stats->width() / 2),
                this->pos().y() + size + size / 3);
  }
  // if Boris is outside borders
  if(this->pos().y() > maxY || this->pos().y() < minY) {
    if(falling) {
      healthQueue -= 5; // It hurts to hit the borders
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
  if(vision && settings->vision && !falling && !grabbed) {
    processVision();
  }
}

void Boris::handleBehaviourChange(QAction* a) {
  for(int i = 0; i < behaviours.length(); ++i) {
    if(behaviours.at(i).title == a->text()) {
      behavQueue.append(behaviours.at(i).file);
    }
  }
}

void Boris::enterEvent(QEvent *event)
{
  event->accept();
  stats->underMouse = true;
}

void Boris::leaveEvent(QEvent *event)
{
  event->accept();
  if(settings->stats == STATS_MOUSEOVER || settings->stats == STATS_CRITICAL) {
    stats->hide(); 
  }
  stats->underMouse = false;
}

void Boris::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::RightButton) {
    bMenu->exec(QCursor::pos());
  }
  if(event->button() == Qt::LeftButton) {
    if(behaviours.at(curBehav).file == "sleep" && stats->getEnergy() <= 95) {
      energyQueue -= 25;
      funQueue -= 20;
    }
    setCursor(QCursor(QPixmap(":mouse_grab.png")));
    grabbed = true;
    changeBehaviour("_grabbed");
    mMoving = true;
    this->move(event->globalPos().x() - (float)size / 32.0 * 17.0, 
               event->globalPos().y() - (float)size / 32.0 * 16.0);
    oldCursor = QCursor::pos();
  }
}

void Boris::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons().testFlag(Qt::LeftButton) && mMoving) {
    this->move(event->globalPos().x() - (float)size / 32.0 * 17.0, 
               event->globalPos().y() - (float)size / 32.0 * 16.0);
    stats->move(this->pos().x() + (size / 2) - (stats->width() / 2), this->pos().y() - stats->height());
    if(stats->pos().y() < 0) {
      stats->move(this->pos().x() + (size / 2) - (stats->width() / 2), this->pos().y() + size + size / 3);
    }
  }

}

void Boris::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton) {
    setCursor(QCursor(QPixmap(":mouse_hover.png")));
    grabbed = false;
    mMoving = false;
    settings->borisX = this->pos().x();
    settings->borisY = this->pos().y();
    changeBehaviour("_falling");
    falling = true;
    hVel = mouseHVel;
    vVel = mouseVVel;
    alt = QCursor::pos().y() + 40;
  }
}

void Boris::wheelEvent(QWheelEvent *)
{
  if(stats->underMouse && !falling && !grabbed && !behaviours.at(curBehav).doNotDisturb) {
    if(annoyance < ANNOYMAX) {
      changeBehaviour("_tickle");
    } else {
      changeBehaviour("_annoyed");
    }
    annoyance++;
  }
}

void Boris::handlePhysics()
{
  if(!grabbed && weatherSprite->isVisible()) {
    sinVal += (double)(qrand() % 2000) / 20000.0;
    if(sinVal > PI) {
      sinVal = 0.0;
    }
    if(settings->windDirection.contains("W")) {
      moveBoris(round(-(sin(sinVal) + 0.25) * settings->windSpeed * 0.05), 0);
    } else if(settings->windDirection.contains("E")) {
      moveBoris(round((sin(sinVal) + 0.25) * settings->windSpeed * 0.05), 0);
    }
    if(chatter->isVisible()) {
      chatter->moveChatter(this->pos().x(), this->pos().y(), size);
    }
  }
  
  if(falling && !grabbed) {
    moveBoris(hVel, vVel);
    vVel += 0.5;
    if(behaviours.at(curBehav).file != "_parachute_deploy") {
      if(vVel > 10 && qrand() % 100 <= 7) {
        changeBehaviour("_parachute_deploy");
      }
    } else {
      if(vVel > 2) {
        vVel -= 1.0;
      } else {
        vVel = 2;
      }
      hVel *= 0.9;
    }
    if(this->pos().y() >= alt) {
      move(this->pos().x(), alt);
      if(vVel < 5.0) {
        if(behaviours.at(curBehav).file != "_parachute_deploy") {
          changeBehaviour("_landing");
        } else {
          changeBehaviour("_complain");
        }
        falling = false;
      } else {
        hVel *= 0.5;
        vVel = (vVel * 0.5) * -1;
        changeBehaviour("_bounce");
      }
    }
  }
  mouseHVel = (QCursor::pos().x() - oldCursor.x()) / 4.0;
  mouseVVel = (QCursor::pos().y() - oldCursor.y()) / 4.0;
#ifdef DEBUG
  qInfo("mouseHVel is %f\n", mouseHVel);("mouseVVel is %f\n", mouseVVel);
#endif
  oldCursor = QCursor::pos();

  if(!falling && !grabbed &&
     !behaviours.at(curBehav).doNotDisturb) {
    QPoint p = QCursor::pos();
    int xA = p.x();
    int yA = p.y();
    int xB = this->pos().x() + (size / 2);
    int yB = this->pos().y() + (size / 2);
    double hypotenuse = sqrt((yB - yA) * (yB - yA) + (xB - xA) * (xB - xA));
    if(hypotenuse < size * 3) {
      if(!alreadyEvading) {
        interactions++;
        if(fabs(mouseHVel) > 20.0 || fabs(mouseVVel) > 20.0) {
          double fleeAngle = atan2((this->pos().y() + (size / 2.0)) - p.y(),
                                   p.x() - (this->pos().x() + (size / 2.0))
                                   ) * 180.0 / PI;
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
        } else if(stats->getFun() > 10 && interactions >= 4 && qrand() % 10 >= 3) {
          changeBehaviour(getFileFromCategory("Social"));
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
    changeBehaviour("_falling", 200000);
    falling = true;
    vVel = ((qrand() % 12) * -1) - 5;
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
  // If it's late decrease energy to make Boris tired
  // Other than that energy decreases are mainly controlled by walking behavs
  if(QTime::currentTime().hour() >= 21 && QTime::currentTime().hour() < 23) {
    energyQueue -= 1;
  } else if(QTime::currentTime().hour() >= 23) {
    energyQueue -= 3;
  } else if(QTime::currentTime().hour() >= 0 && QTime::currentTime().hour() < 3) {
    energyQueue -= 4;
  } else if(QTime::currentTime().hour() >= 3 && QTime::currentTime().hour() < 6) {
    energyQueue -= 5;
  } else if(QTime::currentTime().hour() >= 6 && QTime::currentTime().hour() < 8) {
    energyQueue -= 1;
  }
  hyperQueue -= qrand() % 7;
  hungerQueue -= qrand() % 2;
  socialQueue -= qrand() % 2;
  hygieneQueue -= qrand() % 1;
  funQueue -= qrand() % 3;
  // Nothing needed for 'health' and 'bladder'
}

void Boris::sanityCheck()
{
  int minX = - size;
  int maxX = QApplication::desktop()->width();
  int minY = 0 - (size / 2);
  int maxY = QApplication::desktop()->height() - height();

  // Make sure Boris is not located outside boundaries
  if(this->pos().y() < minY) {
    move(this->pos().x(), minY);
  }
  if(this->pos().y() > maxY) {
    move(this->pos().x(), maxY);
  }
  if(this->pos().x() > maxX) {
    move(minX, this->pos().y());
  }
  if(this->pos().x() < minX) {
    move(maxX, this->pos().y());
  }

  // Make sure Boris altitude is not outside bottom boundary
  if(alt > maxY) {
    alt = maxY;
  }

  // Check if Boris is dying or is already dead
  if(isAlive) {
    if(stats->getHealth() <= 2 || (stats->getEnergy() + stats->getSocial() + stats->getFun() + stats->getHunger()) < 35) {
      killBoris();
    }
  }
}

void Boris::killBoris()
{
  qInfo("Boris has died... RIP!\n");
  statQueueTimer.stop();
  dirtSprite->setOpacity(0.0);
  bruisesSprite->setOpacity(0.0);
  changeBehaviour("_drop_dead");
  behavTimer.stop();
  statTimer.stop();
  isAlive = false;
}

void Boris::statQueueProgress()
{
  // Adjust timer interval to match how hyper Boris is
  int interval = STATTIMER;
  statQueueTimer.setInterval(interval - ((double)interval / 100.0 * stats->getHyper()));
  if(statQueueTimer.interval() < 5)
    statQueueTimer.setInterval(5);

  if(hyperQueue > 100)
    hyperQueue = 100;
  if(healthQueue > 100)
    healthQueue = 100;
  if(energyQueue > 100)
    energyQueue = 100;
  if(hungerQueue > 100)
    hungerQueue = 100;
  if(bladderQueue > 100)
    bladderQueue = 100;
  if(socialQueue > 100)
    socialQueue = 100;
  if(funQueue > 100)
    funQueue = 100;
  if(hygieneQueue > 100)
    hygieneQueue = 100;

  if(hyperQueue < -100)
    hyperQueue = -100;
  if(healthQueue < -100)
    healthQueue = -100;
  if(energyQueue < -100)
    energyQueue = -100;
  if(hungerQueue < -100)
    hungerQueue = -100;
  if(bladderQueue < -100)
    bladderQueue = -100;
  if(socialQueue < -100)
    socialQueue = -100;
  if(funQueue < -100)
    funQueue = -100;
  if(hygieneQueue < -100)
    hygieneQueue = -100;

  if(hyperQueue > 0) {
    hyperQueue--;
    stats->deltaHyper(1);
  }
  if(hyperQueue < 0) {
    hyperQueue++;
    stats->deltaHyper(-1);
  }
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
    stats->deltaHunger(-1);
  }
  if(hungerQueue < 0) {
    hungerQueue++;
    stats->deltaHunger(1);
  }
  if(bladderQueue > 0) {
    bladderQueue--;
    stats->deltaBladder(-1);
  }
  if(bladderQueue < 0) {
    bladderQueue += 2;
    stats->deltaBladder(2);
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

  dirtSprite->setOpacity(0.50 - ((qreal)stats->getHygiene()) * 0.01);
  bruisesSprite->setOpacity(0.75 - ((qreal)stats->getHealth()) * 0.01);
  stats->updateStats();

  statQueueTimer.start();
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
  
  if(borisFriend != nullptr || falling || grabbed || behaviours.at(curBehav).doNotDisturb) {
    return;
  }
  borisFriend = b;
    
  double angle = atan2(this->pos().y() - borisFriend->pos().y(),
                       borisFriend->pos().x() - this->pos().x()) * 180 / 3.1415927;
  if(angle < 0) {
    angle += 360;
  } else if(angle >= 360) {
    angle -= 360;
  }

  int friendAt = Direction::None;
  if((angle >= 0.0 && angle < 22.5) || (angle >= 337.5 && angle < 360.0)) {
    friendAt = Direction::East;
  } else if(angle >= 22.5 && angle < 67.5) {
    friendAt = Direction::NorthEast;
  } else if(angle >= 67.5 && angle < 112.5) {
    friendAt = Direction::North;
  } else if(angle >= 112.5 && angle < 157.5) {
    friendAt = Direction::NorthWest;
  } else if(angle >= 157.5 && angle < 202.5) {
    friendAt = Direction::West;
  } else if(angle >= 202.5 && angle < 247.5) {
    friendAt = Direction::SouthWest;
  } else if(angle >= 247.5 && angle < 292.5) {
    friendAt = Direction::South;
  } else if(angle >= 292.5 && angle < 337.5) {
    friendAt = Direction::SouthEast;
  }

  if(behaviours.at(borisFriend->getCurBehav()).file == "_drop_dead") {
    if(friendAt == Direction::South ||
       friendAt == Direction::SouthEast ||
       friendAt == Direction::East ||
       friendAt == Direction::NorthEast) {
      changeBehaviour("_sad_right");
    } else if(friendAt == Direction::North ||
       friendAt == Direction::NorthWest ||
       friendAt == Direction::West ||
       friendAt == Direction::SouthWest) {
      changeBehaviour("_sad_left");
    }
  } else if(borisFriend->getHygiene() <= 22) {
    if(friendAt == Direction::South) {
      changeBehaviour("_flee_up", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::SouthEast) {
      changeBehaviour("_flee_left_up", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::East) {
      changeBehaviour("_flee_left", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::NorthEast) {
      changeBehaviour("_flee_left_down", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::North) {
      changeBehaviour("_flee_down", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::NorthWest) {
      changeBehaviour("_flee_right_down", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::West) {
      changeBehaviour("_flee_right", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::SouthWest) {
      changeBehaviour("_flee_right_up", (qrand() % 2000) + 1500);
    }
  } else {
    if(friendAt == Direction::South) {
      changeBehaviour("_casual_wave_down", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::SouthEast) {
      changeBehaviour("_casual_wave_right_down", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::East) {
      changeBehaviour("_casual_wave_right", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::NorthEast) {
      changeBehaviour("_casual_wave_right_up", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::North) {
      changeBehaviour("_casual_wave_up", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::NorthWest) {
      changeBehaviour("_casual_wave_left_up", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::West) {
      changeBehaviour("_casual_wave_left", (qrand() % 2000) + 1500);
    } else if(friendAt == Direction::SouthWest) {
      changeBehaviour("_casual_wave_left_down", (qrand() % 2000) + 1500);
    }
  }
  QTimer::singleShot((qrand() % 5000) + 15000, this, &Boris::readyForFriend);
}

void Boris::processVision()
{
  int border = 4;
  int contrast = 15;
  QImage vision = QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId(), pos().x() - border, pos().y() - border, size + border * 2, size + border * 2).toImage();
  
  QRgb *bits = (QRgb *)vision.bits();
  { // Check for wall to the west
    int top = border * vision.width();
    int spread = size / 5;
    int wallDetect = 0;
    for(int smp = 0; smp < 5; ++smp) {
      printf("CONTRAST: %d\n", abs(qGray(bits[top + (smp * spread * vision.width())]) -
                                   qGray(bits[top + (smp * spread * vision.width()) + border - 1])));
      if(abs(qGray(bits[top + (smp * spread * vision.width())]) -
             qGray(bits[top + (smp * spread * vision.width()) + border - 1])) > contrast) {
        printf("MAYBE A WALL!\n");
        wallDetect++;
      }
    }
    if(wallDetect >= 3) {
      moveBoris(2, 0, false);
      changeBehaviour();
      return;
    }
  }
  { // Check for wall to the east
    int top = border * vision.width() + vision.width() - border;
    int spread = size / 5;
    int wallDetect = 0;
    for(int smp = 0; smp < 5; ++smp) {
      printf("CONTRAST: %d\n", abs(qGray(bits[top + (smp * spread * vision.width())]) -
                                   qGray(bits[top + (smp * spread * vision.width()) + border - 1])));
      if(abs(qGray(bits[top + (smp * spread * vision.width())]) -
             qGray(bits[top + (smp * spread * vision.width()) + border - 1])) > contrast) {
        printf("MAYBE A WALL!\n");
        wallDetect++;
      }
    }
    if(wallDetect >= 3) {
      moveBoris(-2, 0, false);
      changeBehaviour();
      return;
    }
  }
  { // Check for wall to the north
    int top = 0 + border;
    int spread = size / 5;
    int wallDetect = 0;
    for(int smp = 0; smp < 5; ++smp) {
      if(abs(qGray(bits[top + (smp * spread)]) -
             qGray(bits[top + (smp * spread) + ((border - 1) * vision.width())])) > contrast) {
        printf("MAYBE A WALL!\n");
        wallDetect++;
      }
    }
    if(wallDetect >= 3) {
      moveBoris(2, 0, false);
      changeBehaviour();
      return;
    }
  }
  { // Check for wall to the south
    int top = ((size + border) * vision.width()) + border;
    int spread = size / 5;
    int wallDetect = 0;
    for(int smp = 0; smp < 5; ++smp) {
      if(abs(qGray(bits[top + (smp * spread)]) -
             qGray(bits[top + (smp * spread) + ((border - 1) * vision.width())])) > contrast) {
        printf("MAYBE A WALL!\n");
        wallDetect++;
      }
      //bits[top + (smp * spread)] = qRgb(255, 0, 0);
      //bits[qGray(bits[top + (smp * spread) + ((border - 1) * vision.width())])] = qRgb(255, 0, 0);
    }
    //vision.save("blah.png");
    //exit(0);
    if(wallDetect >= 3) {
      moveBoris(2, 0, false);
      changeBehaviour();
      return;
    }
  }
}

void Boris::processAi(QString &behav, int &time)
{
  // You might wonder why I check behav == "" and time == 0 in all the following if sentences.
  // The reason is that they might change throughout the function, and thus makes sense to
  // make sure an AI decision hasn't already been made.

  // Check if it is time to show the weather again
  timeForWeather++;
  if(timeForWeather >= 80 || timeForWeather == 666) {
    timeForWeather = 0;
    showWeather(behav);
  }

  // Check if Boris has put off the toilet visit for too long. If so, make him s**t his pants :(
  if(behav == "" && time == 0 && stats->getBladder() <= 0) {
    tooLateForLoo++;
    if(tooLateForLoo >= 6) {
      tooLateForLoo = 0;
      behav = "_too_late";
    }
  }
  
  if(behav == "" && time == 0 && qrand() % 2) {
    // Stat check
    QList<QString> potentials;
    if(stats->getFun() <= 50) {
      if(qrand() % (100 - stats->getFun()) > independence) {
        potentials.append("_fun");
      } else if(stats->getFun() <= 15) {
        if(qrand() % 100 < independence) {
          potentials.append(getFileFromCategory("Fun"));
        }
      }
    }
    if(stats->getEnergy() <= 50) {
      if(qrand() % (100 - stats->getEnergy()) > independence) {
        potentials.append("_energy");
      } else if(stats->getEnergy() <= 15) {
        if(qrand() % 100 < independence) {
          potentials.append(getFileFromCategory("Energy"));
        }
      }
    }
    if(stats->getHunger() <= 50) {
      if(qrand() % (100 - stats->getHunger()) > independence) {
        potentials.append("_hunger");
      } else if(stats->getHunger() <= 15) {
        if(qrand() % 100 < independence) {
          potentials.append(getFileFromCategory("Hunger"));
        }
      }
    }
    if(stats->getBladder() <= 50) {
      if(qrand() % (100 - stats->getBladder()) > independence) {
        potentials.append("_bladder");
      } else if(stats->getBladder() <= 15) {
        if(qrand() % 100 < independence) {
          potentials.append(getFileFromCategory("Bladder"));
        }
      }
    }
    if(stats->getSocial() <= 50) {
      if(qrand() % (100 - stats->getSocial()) > independence) {
        potentials.append("_social");
      } else if(stats->getSocial() <= 15) {
        if(qrand() % 100 < independence) {
          potentials.append(getFileFromCategory("Social"));
        }
      }
    }
    if(stats->getHygiene() <= 50) {
      if(qrand() % (100 - stats->getHygiene()) > independence) {
        potentials.append("_hygiene");
      } else if(stats->getHygiene() <= 15) {
        if(qrand() % 100 < independence) {
          potentials.append(getFileFromCategory("Hygiene"));
        }
      }
    }
    if(stats->getHealth() <= 50) {
      if(qrand() % (150 - stats->getHealth()) > independence) {
        potentials.append("_health");
      }
    }
    // Now choose one from the potentials
    if(!potentials.isEmpty()) {
      behav = potentials.at(qrand() % potentials.size());
      // Flash stat if appropriate
      if(behav == "_fun") {
        stats->flashStat("fun");
      } else if(behav == "_energy") {
        stats->flashStat("energy");
      } else if(behav == "_hunger") {
        stats->flashStat("hunger");
      } else if(behav == "_bladder") {
        stats->flashStat("bladder");
      } else if(behav == "_social") {
        stats->flashStat("social");
      }
    }
  }
}

void Boris::updateBoris()
{
  // Reset Boris pointer
  borisFriend = nullptr;

  // Set new size
  size = settings->size;
  if(size == 0) {
    size = (qrand() % (256 - 32)) + 32; // Make him at least 32
  }
  setFixedSize(size, size + (size / 2.0));
  resetTransform();
  scale(size / 32.0, size / 32.0);

  // The following should be used in a reimplementation of resizeEvent
  //fitInView(QRectF(0.0, 0.0, 32.0, 48.0), Qt::IgnoreAspectRatio);

  // Set new independence value
  independence = settings->independence;
  if(independence == 0) {
    independence = (qrand() % 99) + 1;
  }

  // Show or hide stats
  if(settings->stats == STATS_ALWAYS) {
    stats->show();
  } else {
    stats->hide();
  }
}

void Boris::nextWeatherFrame()
{
  weatherSprite->setPixmap(weathers.at(curWeather).frames.at(curWeatherFrame).sprite);
  weatherTimer.setInterval(weathers.at(curWeather).frames.at(curWeatherFrame).time);
  curWeatherFrame++;
  if(curWeatherFrame >= weathers.at(curWeather).frames.length()) {
    curWeatherFrame = 0;
  }
  weatherTimer.start();
}

void Boris::triggerWeather()
{
  // Force showing weather next time processAi is run
  timeForWeather = 666;
}

void Boris::showWeather(QString &behav)
{
  // Reset sine wave for wind
  sinVal = 0.0;
  
  for(int a = 0; a < weathers.count(); ++a) {
    if(weathers.at(a).file == settings->weatherType) {
      curWeather = a;
      break;
    }
  }
  curWeatherFrame = 0;
  weatherTimer.setInterval(0);
  weatherTimer.start();

  weatherSprite->show();
  QTimer::singleShot(30000, this, SLOT(hideWeather()));
  
  if(!falling && !grabbed) {
    if(settings->weatherType == "01d") {
      behavQueue.append("sunglasses");
    } else if(settings->weatherType == "02d" && settings->temperature >= 15) {
      behavQueue.append("sunglasses");
    } else if(settings->weatherType == "09d" || settings->weatherType == "09n" ||
              settings->weatherType == "10d" || settings->weatherType == "10n") {
      behavQueue.append("_umbrella");
    } else if(settings->weatherType == "11d" || settings->weatherType == "11n") {
      behav = "_lightning";
    } else if(settings->weatherType == "13d" || settings->weatherType == "13n") {
      behavQueue.append("_freezing");
    } else if(settings->weatherType == "01n" || settings->weatherType == "02n") {
      behavQueue.append("_energy"); // Yawn for weathers that have a moon
    } else if(settings->weatherType == "04d" || settings->weatherType == "04n" ||
              settings->weatherType == "50d" || settings->weatherType == "50n") {
      behavQueue.append("_cloudy");
    }
  }
}

void Boris::hideWeather()
{
  weatherSprite->hide();
}

int Boris::getCurBehav()
{
  return curBehav;
}

void Boris::readyForFriend()
{
  borisFriend = nullptr;
}

void Boris::balanceInteractions()
{
  if((settings->stats == STATS_MOUSEOVER || settings->stats == STATS_CRITICAL) &&
     stats->underMouse) {
    stats->show();
  }
  // This balances the interaction count which is increased when mouse is moved across Boris
  if(interactions > 6) {
    interactions = 6;
  } else if(interactions > 0) {
    interactions--;
  }
  if(annoyance > ANNOYMAX) {
    annoyance = ANNOYMAX;
  } else if(annoyance > 0) {
    annoyance = annoyance - 4;
  }
  interactionsTimer.start();
}
