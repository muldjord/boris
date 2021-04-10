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
#include "soundmixer.h"
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
#include <QElapsedTimer>
#include <QRandomGenerator>

constexpr int STATTIMER = 200;
constexpr double PI = 3.1415927;
constexpr int ANNOYMAX = 42;

extern QList<Boris*> borisList;
extern QList<Item*> itemList;
extern QList<Behaviour> behaviours;
extern QList<Behaviour> weathers;
extern SoundMixer soundMixer;

Boris::Boris(Settings *settings)
{
  this->settings = settings;
  
  int borisX = settings->borisX;
  int borisY = settings->borisY;
  if(borisY > QApplication::desktop()->height() - height()) {
    borisY = QApplication::desktop()->height() - height();
  }
  
  move(borisX + QRandomGenerator::global()->bounded(200) - 100, borisY + QRandomGenerator::global()->bounded(200) - 100);

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
  
  createBehavMenu();

  // Set initial stats with some randomization
  int hyper = 0;
  int health = 100;
  int energy = 50 + QRandomGenerator::global()->bounded(25);
  int hunger = 50 + QRandomGenerator::global()->bounded(25);
  int bladder = 50 + QRandomGenerator::global()->bounded(25);
  int hygiene = 100;
  int social = 15 + QRandomGenerator::global()->bounded(25);
  int fun = 50 + QRandomGenerator::global()->bounded(25);
  hyperQueue = 0;
  healthQueue = 0;
  energyQueue = 0;
  hungerQueue = 0;
  bladderQueue = 0;
  socialQueue = 0;
  funQueue = 0;
  hygieneQueue = 0;
  stats = new Stats(settings, hyper, health, energy, hunger, bladder, social, fun, hygiene, this);
  bubble = new Bubble(settings);
  
  staticBehavs = 0;
  // Figure out how many static behaviours there are
  for(const auto &behaviour: behaviours) {
    if(behaviour.file.left(1) == "_") {
      staticBehavs++;
    }
  }

  behavTimer.setSingleShot(true);
  connect(&behavTimer, &QTimer::timeout, this, &Boris::nextBehaviour);

  // QBasicTimer for better accuracy
  physicsTimer.setInterval(30);
  connect(&physicsTimer, &QTimer::timeout, this, &Boris::handlePhysics);
  physicsTimer.start();
  //physicsTimer.start(30, this);

  /*
  animTimer.setInterval(0);
  connect(&animTimer, &QTimer::timeout, this, &Boris::nextFrame);
  animTimer.start();
  */

  animTimer.start(0, Qt::PreciseTimer, this);

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
  connect(&interactionsTimer, &QTimer::timeout, this, &Boris::checkInteractions);
  interactionsTimer.start();

  setCursor(QCursor(QPixmap(":mouse_hover.png")));

  updateBoris();
}

Boris::~Boris()
{
  delete stats;
  delete bubble;
  delete behavioursMenu;
}

void Boris::createBehavMenu()
{
  behavioursMenu = new QMenu();
  behavioursMenu->setTitle(tr("Behaviours"));
  QMenu *healthMenu = new QMenu(tr("Health"), behavioursMenu);
  healthMenu->setIcon(QIcon(":health.png"));
  QMenu *energyMenu = new QMenu(tr("Energy"), behavioursMenu);
  energyMenu->setIcon(QIcon(":energy.png"));
  QMenu *hungerMenu = new QMenu(tr("Food"), behavioursMenu);
  hungerMenu->setIcon(QIcon(":hunger.png"));
  QMenu *bladderMenu = new QMenu(tr("Toilet"), behavioursMenu);
  bladderMenu->setIcon(QIcon(":bladder.png"));
  QMenu *hygieneMenu = new QMenu(tr("Hygiene"), behavioursMenu);
  hygieneMenu->setIcon(QIcon(":hygiene.png"));
  QMenu *socialMenu = new QMenu(tr("Social"), behavioursMenu);
  socialMenu->setIcon(QIcon(":social.png"));
  QMenu *funMenu = new QMenu(tr("Fun"), behavioursMenu);
  funMenu->setIcon(QIcon(":fun.png"));
  QMenu *movementMenu = new QMenu(tr("Movement"), behavioursMenu);
  movementMenu->setIcon(QIcon(":movement.png"));
  QMenu *iddqdMenu = new QMenu(tr("Iddqd"), behavioursMenu);
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
  for(const auto &behaviour: behaviours) {
    if(behaviour.file.left(1) != "_" || behaviour.category != "Hidden") {
      if(behaviour.category == "Movement") {
        movementMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
      } else if(behaviour.category == "Energy") {
        energyMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
      } else if(behaviour.category == "Hunger") {
        hungerMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
      } else if(behaviour.category == "Bladder") {
        bladderMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
      } else if(behaviour.category == "Social") {
        socialMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
      } else if(behaviour.category == "Fun") {
        funMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
      } else if(behaviour.category == "Hygiene") {
        hygieneMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
      } else if(behaviour.category == "Health") {
        healthMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
      } else {
        iddqdMenu->addAction(QIcon(":iddqd.png"), behaviour.title);
      }
    } else {
      iddqdMenu->addAction(QIcon(":iddqd.png"), behaviour.title);
    }
  }
  behavioursMenu->addMenu(healthMenu);
  behavioursMenu->addMenu(energyMenu);
  behavioursMenu->addMenu(hungerMenu);
  behavioursMenu->addMenu(bladderMenu);
  behavioursMenu->addMenu(hygieneMenu);
  behavioursMenu->addMenu(socialMenu);
  behavioursMenu->addMenu(funMenu);
  behavioursMenu->addMenu(movementMenu);
  if(settings->iddqd) {
    behavioursMenu->addMenu(iddqdMenu);
  }
}

QString Boris::getFileFromCategory(QString category)
{
  QList<QString> b;
  for(const auto &behaviour: behaviours) {
    if(behaviour.category == category) {
      b.append(behaviour.file);
    }
  }
  int chosen = QRandomGenerator::global()->bounded(b.length());
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
  int chosen = QRandomGenerator::global()->bounded(b.length());
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
  // This is necessary even though there's a 'stop' in the behaviour. Otherwise he will revive midair.
  if(behaviours.at(curBehav).file == "_drop_dead") {
    return;
  }

  // Always stop behavTimer, just in case. At this point we never want it to be running
  behavTimer.stop();
  
  // Reset all script variables
  scriptVars.clear();

  // Clear script image canvas
  scriptImage.fill(Qt::transparent);
  drawing = false;
  
  // Check if there are behaviours in queue, these are prioritized
  if(behav == "" && !behavQueue.isEmpty()) {
    behav = behavQueue.takeFirst();
  }

  // Process the AI if no forced behaviour is set
  if(behav == "") {
    processAi(behav);
  }
  
  // Bias towards behavs from 'Idle' and 'Locomotion' categories to make Boris less erratic
  if(QRandomGenerator::global()->bounded(10) >= 3) {
    if(QRandomGenerator::global()->bounded(70) >= stats->getEnergy()) {
      curBehav = getIdxFromCategory("Idle");
    } else {
      curBehav = getIdxFromCategory("Locomotion"); // This category DOES exist. See data/behavs/README.md
    }
  } else {
    curBehav = QRandomGenerator::global()->bounded(behaviours.size() - staticBehavs) + staticBehavs;
  }

  // If a specific behaviour is requested, use that
  if(!behav.isEmpty()) {
    for(int a = 0; a < behaviours.size(); ++a) {
      if(behaviours.at(a).file == behav) {
        curBehav = a;
        break;
      }
    }
  }
  
  // Applying behaviour stats to Boris
  hyperQueue += behaviours.at(curBehav).hyper;
  healthQueue += behaviours.at(curBehav).health;
  energyQueue += behaviours.at(curBehav).energy;
  hungerQueue += behaviours.at(curBehav).hunger;
  bladderQueue += behaviours.at(curBehav).bladder;
  socialQueue += behaviours.at(curBehav).social;
  funQueue += behaviours.at(curBehav).fun;
  hygieneQueue += behaviours.at(curBehav).hygiene;
  
  if(behaviours.at(curBehav).allowFlip && QRandomGenerator::global()->bounded(2)) {
    flipFrames = true;
  } else {
    flipFrames = false;
  }

  if(!behaviours.at(curBehav).oneShot) {
    if(time == -1) {
      time = QRandomGenerator::global()->bounded(7000) + 5000;
    }
    time = time - (time / 100.0 * stats->getHyper());
    behavTimer.setInterval(time);
    behavTimer.start();
  }

  curFrame = 0;
  animTimer.start(0, Qt::PreciseTimer, this);
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

void Boris::runScript(int &stop)
{
  // Update current stat variables for scripting use
  scriptVars["bsize"] = size;
  scriptVars["energy"] = stats->getEnergy();
  scriptVars["health"] = stats->getHealth();
  scriptVars["hyper"] = stats->getHyper();
  scriptVars["hunger"] = stats->getHunger();
  scriptVars["bladder"] = stats->getBladder();
  scriptVars["social"] = stats->getSocial();
  scriptVars["fun"] = stats->getFun();
  scriptVars["hygiene"] = stats->getHygiene();
  scriptVars["xvel"] = mouseHVel;
  scriptVars["yvel"] = mouseVVel;
  scriptVars["borisx"] = pos().x() + (size / 2);
  scriptVars["borisy"] = pos().y() + size;
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
  scriptVars["wind"] = settings->windSpeed;
  scriptVars["temp"] = settings->temperature;

  ScriptHandler scriptHandler(&scriptImage, &drawing, settings, bubble, behaviours.at(curBehav).labels, behaviours.at(curBehav).defines, scriptVars, QPoint(pos().x(), pos().y()), size);
  connect(&scriptHandler, &ScriptHandler::behavFromFile, this, &Boris::behavFromFile);
  connect(&scriptHandler, &ScriptHandler::setCurFrame, this, &Boris::setCurFrame);
  connect(&scriptHandler, &ScriptHandler::statChange, this, &Boris::statChange);
  scriptHandler.runScript(stop, behaviours.at(curBehav).frames.at(curFrame).script);

  if(flipFrames) {
    QImage flipped = scriptImage.mirrored(true, false);
    scriptSprite->setPixmap(QPixmap::fromImage(flipped));
  } else {
    scriptSprite->setPixmap(QPixmap::fromImage(scriptImage));
  }
  if(!drawing) {
    scriptImage.fill(Qt::transparent);
  }
}

int Boris::getDistance(const QPoint &p)
{
  int xA = p.x();
  int yA = p.y();
  QPoint globalCenter = getGlobalCenter();
  int xB = globalCenter.x();
  int yB = globalCenter.y();

  return sqrt((yB - yA) * (yB - yA) + (xB - xA) * (xB - xA));
}

QPoint Boris::getGlobalCenter()
{
  return QPoint(pos().x() + (width() / 2),
                pos().y() + height() - (size / 2));
}

int Boris::getSector(const QPoint &p)
{
  // Center coordinate of Boris
  QPoint b(pos().x() + (size / 2), pos().y() + size);

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

void Boris::timerEvent(QTimerEvent *)
{
  if(stopNextBehaviour) {
    stopNextBehaviour = false;
    behavTimer.stop();
    nextBehaviour();
    return;
  }

  sanityCheck();
  
  if(curFrame >= behaviours.at(curBehav).frames.count()) {
    curFrame = 0;
    if(behaviours.at(curBehav).oneShot) {
      changeBehaviour();
      return;
    }
  }

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
      soundMixer.playSound(behaviours.at(curBehav).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings->desktopWidth * 2.0 - 1.0,
                           (stats->getHyper() / 60.0) + 1);
    } else {
      soundMixer.playSound(behaviours.at(curBehav).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings->desktopWidth * 2.0 - 1.0,
                           (stats->getHyper() / 60.0) + (0.95 + QRandomGenerator::global()->bounded(100) / 1000.0));
    }
  }

  if(behaviours.at(curBehav).frames.at(curFrame).dx != 0 ||
     behaviours.at(curBehav).frames.at(curFrame).dy != 0) {
    moveBoris(behaviours.at(curBehav).frames.at(curFrame).dx,
              behaviours.at(curBehav).frames.at(curFrame).dy,
              flipFrames);
  }

  int frameTime = behaviours.at(curBehav).frames.at(curFrame).time;
  frameTime -= (frameTime / 100.0 * stats->getHyper());
  if(frameTime <= 5) {
    frameTime = 5;
  }

  int stop = 0; // Will be > 0 if a goto, behav or break command is run
  runScript(stop);
  if(stop == 1) {
    // In case of 'goto' curFrame has been set in scriptHandler
    // In case of 'behav' behavFromFile has been emitted
  } else if(stop == 2) {
    // In case of 'break' it will change to the next behaviour when the frame times out
    stopNextBehaviour = true;
  } else if(stop == 3) {
    // In case of 'stop' it will cease any frame and animation progression
    behavTimer.stop();
    animTimer.stop();
    return;
  } else {
    curFrame++;
  }
  animTimer.start(frameTime, Qt::PreciseTimer, this);
}

void Boris::moveBoris(int dX, int dY, const bool &flipped, const bool &vision)
{
  sanityCheck();
  
  int minX = 0;
  int maxX = QApplication::desktop()->width() - size;
  int minY = 0 - (size / 2);
  int maxY = QApplication::desktop()->height() - height();

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
  bubble->moveBubble(pos().x(), pos().y(), size);
  stats->move(pos().x() + (size / 2) - (stats->width() / 2),
              pos().y() - stats->height() + (size / 3));
  // Move stats below Boris when he's at the top of the screen
  if(stats->pos().y() < 0) {
    stats->move(pos().x() + (size / 2) - (stats->width() / 2),
                pos().y() + size + size / 3);
  }
  // if Boris is outside borders
  if(pos().y() > maxY || pos().y() < minY) {
    if(falling) {
      healthQueue -= 5; // It hurts to hit the borders
      // Physics velocity when hitting borders
      if(pos().x() + dX > maxX || pos().x() + dX < minX) {
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
  for(const auto &behaviour: behaviours) {
    if(behaviour.title == a->text()) {
      behavQueue.append(behaviour.file);
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
    behavioursMenu->exec(QCursor::pos());
  }
  if(event->button() == Qt::LeftButton) {
    if(behaviours.at(curBehav).file == "_sleep" && stats->getEnergy() <= 95) {
      energyQueue -= 25;
      funQueue -= 20;
    }
    setCursor(QCursor(QPixmap(":mouse_grab.png")));
    grabbed = true;
    changeBehaviour("_grabbed");
    mMoving = true;
    this->move(event->globalPos().x() - size / 32.0 * 17.0, 
               event->globalPos().y() - size / 32.0 * 16.0);
    oldCursor = QCursor::pos();
  }
}

void Boris::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons().testFlag(Qt::LeftButton) && mMoving) {
    this->move(event->globalPos().x() - size / 32.0 * 17.0, 
               event->globalPos().y() - size / 32.0 * 16.0);
    stats->move(pos().x() + (size / 2) - (stats->width() / 2), pos().y() - stats->height());
    if(stats->pos().y() < 0) {
      stats->move(pos().x() + (size / 2) - (stats->width() / 2), pos().y() + size + size / 3);
    }
    bubble->moveBubble(pos().x(), pos().y(), size);
  }

}

void Boris::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton) {
    setCursor(QCursor(QPixmap(":mouse_hover.png")));
    grabbed = false;
    mMoving = false;
    settings->borisX = pos().x();
    settings->borisY = pos().y();
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
    sinVal += QRandomGenerator::global()->bounded(2000) / 20000.0;
    if(sinVal > PI) {
      sinVal = 0.0;
    }
    if(settings->windDirection.contains("W")) {
      moveBoris(round(-(sin(sinVal) + 0.25) * settings->windSpeed * 0.05), 0);
    } else if(settings->windDirection.contains("E")) {
      moveBoris(round((sin(sinVal) + 0.25) * settings->windSpeed * 0.05), 0);
    }
    if(bubble->isVisible()) {
      bubble->moveBubble(pos().x(), pos().y(), size);
    }
  }
  
  if(falling && !grabbed) {
    moveBoris(hVel, vVel);
    vVel += 0.5;
    if(behaviours.at(curBehav).file != "_umbrella_falling") {
      if(vVel > 10 && QRandomGenerator::global()->bounded(100) <= 7) {
        changeBehaviour("_umbrella_falling");
      }
    } else {
      if(vVel > 2) {
        vVel -= 1.0;
      } else {
        vVel = 2;
      }
      hVel *= 0.9;
    }
    if(pos().y() >= alt) {
      move(pos().x(), alt);
      if(vVel < 5.0) {
        if(behaviours.at(curBehav).file != "_umbrella_falling") {
          changeBehaviour("_landing");
        } else {
          changeBehaviour("_umbrella_landing");
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
  oldCursor = QCursor::pos();

  if(!falling && !grabbed) {
    if(getDistance(QCursor::pos()) < size * 3) {
      if(!mouseHovering) {
        interactions++;
        if(!behaviours.at(curBehav).doNotDisturb) {
          if(fabs(mouseHVel) > 35.0 || fabs(mouseVVel) > 35.0) {
            int mouseSector = getSector(QCursor::pos());
            int timeout = QRandomGenerator::global()->bounded(2000) + 1000;
            if(mouseSector == 2) {
              changeBehaviour("_flee_w", timeout);
            } else if(mouseSector == 1) {
              changeBehaviour("_flee_sw", timeout);
            } else if(mouseSector == 0) {
              changeBehaviour("_flee_s", timeout);
            } else if(mouseSector == 7) {
              changeBehaviour("_flee_se", timeout);
            } else if(mouseSector == 6) {
              changeBehaviour("_flee_e", timeout);
            } else if(mouseSector == 5) {
              changeBehaviour("_flee_ne", timeout);
            } else if(mouseSector == 4) {
              changeBehaviour("_flee_n", timeout);
            } else if(mouseSector == 3) {
              changeBehaviour("_flee_nw", timeout);
            }
          } else if(stats->getFun() > 10 &&
                    stats->getSocial() < QRandomGenerator::global()->bounded(interactions * 40)) {
            changeBehaviour("_mouse_interact");
            interactions = 0;
          }
        }
      }
      mouseHovering = true;
    } else {
      mouseHovering = false;
    }
  }
}

void Boris::earthquake()
{
  if(!falling && !grabbed) {
    changeBehaviour("_falling");
    falling = true;
    vVel = (QRandomGenerator::global()->bounded(12) * -1) - 5;
    hVel = QRandomGenerator::global()->bounded(11) - 5;
    alt = pos().y();
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
  hyperQueue -= QRandomGenerator::global()->bounded(7);
  hungerQueue -= QRandomGenerator::global()->bounded(2);
  socialQueue -= QRandomGenerator::global()->bounded(2);
  hygieneQueue -= QRandomGenerator::global()->bounded(1);
  funQueue -= QRandomGenerator::global()->bounded(3);
  // Nothing needed for 'health' and 'bladder'
}

void Boris::sanityCheck()
{
  int minX = - size;
  int maxX = QApplication::desktop()->width();
  int minY = 0 - (size / 2);
  int maxY = QApplication::desktop()->height() - height();

  // Make sure Boris is not located outside boundaries
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
  isAlive = false;
  statQueueTimer.stop();
  statTimer.stop();
  dirtSprite->setOpacity(0.0);
  bruisesSprite->setOpacity(0.0);
  changeBehaviour("_drop_dead");
}

void Boris::statQueueProgress()
{
  // Adjust timer interval to match how hyper Boris is
  int interval = STATTIMER;
  statQueueTimer.setInterval(interval - (interval / 100.0 * stats->getHyper()));
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
    socialQueue += 2;
    stats->deltaSocial(-2);
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

void Boris::collide(Boris *boris)
{
  if(pos().y() > boris->pos().y()) {
    // Bring this Boris to the front, because he is considered closer in 3D space
    raise();
    setFocus();
  }
  
  if(borisFriend != nullptr || falling || grabbed || behaviours.at(curBehav).doNotDisturb) {
    return;
  }
  borisFriend = boris;
  // Queue current behaviour so it isn't 'forgotten'
  behavQueue.prepend(behaviours.at(curBehav).file);
  int friendAt = getSector(QPoint(boris->pos().x() + (size / 2), boris->pos().y() + size));

  if(behaviours.at(borisFriend->getCurBehav()).file == "_drop_dead") {
    if(friendAt == Direction::South ||
       friendAt == Direction::SouthEast ||
       friendAt == Direction::East ||
       friendAt == Direction::NorthEast) {
      changeBehaviour("_sad_e");
    } else if(friendAt == Direction::North ||
       friendAt == Direction::NorthWest ||
       friendAt == Direction::West ||
       friendAt == Direction::SouthWest) {
      changeBehaviour("_sad_w");
    }
  } else if(borisFriend->getHygiene() <= 22) {
    if(friendAt == Direction::South) {
      changeBehaviour("_flee_s", QRandomGenerator::global()->bounded(2000) + 1500);
    } else if(friendAt == Direction::SouthEast) {
      changeBehaviour("_flee_se", QRandomGenerator::global()->bounded(2000) + 1500);
    } else if(friendAt == Direction::East) {
      changeBehaviour("_flee_e", QRandomGenerator::global()->bounded(2000) + 1500);
    } else if(friendAt == Direction::NorthEast) {
      changeBehaviour("_flee_ne", QRandomGenerator::global()->bounded(2000) + 1500);
    } else if(friendAt == Direction::North) {
      changeBehaviour("_flee_n", QRandomGenerator::global()->bounded(2000) + 1500);
    } else if(friendAt == Direction::NorthWest) {
      changeBehaviour("_flee_nw", QRandomGenerator::global()->bounded(2000) + 1500);
    } else if(friendAt == Direction::West) {
      changeBehaviour("_flee_w", QRandomGenerator::global()->bounded(2000) + 1500);
    } else if(friendAt == Direction::SouthWest) {
      changeBehaviour("_flee_sw", QRandomGenerator::global()->bounded(2000) + 1500);
    }
  } else {
    if(friendAt == Direction::South) {
      changeBehaviour("_wave_s");
    } else if(friendAt == Direction::SouthEast) {
      changeBehaviour("_wave_se");
    } else if(friendAt == Direction::East) {
      changeBehaviour("_wave_e");
    } else if(friendAt == Direction::NorthEast) {
      changeBehaviour("_wave_ne");
    } else if(friendAt == Direction::North) {
      changeBehaviour("_wave_n");
    } else if(friendAt == Direction::NorthWest) {
      changeBehaviour("_wave_nw");
    } else if(friendAt == Direction::West) {
      changeBehaviour("_wave_w");
    } else if(friendAt == Direction::SouthWest) {
      changeBehaviour("_wave_sw");
    }
  }
  QTimer::singleShot(QRandomGenerator::global()->bounded(5000) + 15000, this, &Boris::readyForFriend);
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

void Boris::processAi(QString &behav)
{
  // You might wonder why I check behav == "" in all the following if sentences.
  // The reason is that they might change throughout the function, and thus makes sense to
  // make sure an AI decision hasn't already been made.

  // Check if it is time to show the weather again
  timeForWeather++;
  if(timeForWeather >= 80 || timeForWeather == 666) {
    timeForWeather = 0;
    showWeather(behav);
  }

  if(behav == "" && QRandomGenerator::global()->bounded(2)) {
    // Stat check
    QList<QString> potentials;
    if(stats->getFun() <= 50) {
      if(QRandomGenerator::global()->bounded(100 - stats->getFun()) > independence) {
        potentials.append("_fun");
      } else if(stats->getFun() <= 15) {
        if(QRandomGenerator::global()->bounded(100) < independence) {
          potentials.append(getFileFromCategory("Fun"));
        }
      }
    }
    if(stats->getEnergy() <= 50) {
      if(QRandomGenerator::global()->bounded(100 - stats->getEnergy()) > independence) {
        potentials.append("_energy");
      } else if(stats->getEnergy() <= 15) {
        if(QRandomGenerator::global()->bounded(100) < independence) {
          potentials.append(getFileFromCategory("Energy"));
        }
      }
    }
    if(stats->getHunger() <= 50) {
      if(QRandomGenerator::global()->bounded(100 - stats->getHunger()) > independence) {
        potentials.append("_hunger");
      } else if(stats->getHunger() <= 15) {
        if(QRandomGenerator::global()->bounded(100) < independence) {
          potentials.append(getFileFromCategory("Hunger"));
        }
      }
    }
    if(stats->getBladder() <= 50) {
      if(QRandomGenerator::global()->bounded(100 - stats->getBladder()) > independence) {
        potentials.append("_bladder");
      } else if(stats->getBladder() <= 15) {
        if(QRandomGenerator::global()->bounded(100) < independence) {
          potentials.append(getFileFromCategory("Bladder"));
        }
      }
    }
    if(stats->getSocial() <= 50) {
      if(QRandomGenerator::global()->bounded(100 - stats->getSocial()) > independence) {
        potentials.append("_social");
      } else if(stats->getSocial() <= 15) {
        if(QRandomGenerator::global()->bounded(100) < independence) {
          potentials.append(getFileFromCategory("Social"));
        }
      }
    }
    if(stats->getHygiene() <= 50) {
      if(QRandomGenerator::global()->bounded(100 - stats->getHygiene()) > independence) {
        potentials.append("_hygiene");
      } else if(stats->getHygiene() <= 15) {
        if(QRandomGenerator::global()->bounded(100) < independence) {
          potentials.append(getFileFromCategory("Hygiene"));
        }
      }
    }
    if(stats->getHealth() <= 50) {
      if(QRandomGenerator::global()->bounded(150 - stats->getHealth()) > independence) {
        potentials.append("_health");
      }
    }
    // Now choose one from the potentials
    if(!potentials.isEmpty()) {
      behav = potentials.at(QRandomGenerator::global()->bounded(potentials.size()));
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
      } else if(behav == "_health") {
        // This is a visual stat, nothing to flash
      } else if(behav == "_hygiene") {
        // This is a visual stat, nothing to flash
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
    size = QRandomGenerator::global()->bounded(257 - 32) + 32; // Make him at least 32 and max 256
  }
  setFixedSize(size, size + (size / 2.0));
  resetTransform();
  scale(size / 32.0, size / 32.0);

  // Set new independence value
  independence = settings->independence;
  if(independence == 0) {
    independence = QRandomGenerator::global()->bounded(99) + 1;
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

void Boris::checkInteractions()
{
  // Check if there are any collisions with other Borises
  for(auto &boris: borisList) {
    if(boris != this) {
      if(getDistance(boris->getGlobalCenter()) < size * 2) {
        collide(boris);
        break;
      }
    }
  }

  // Check if user is dragging any items close by
  for(auto &item: itemList) {
    if(!falling && !grabbed && !behaviours.at(curBehav).doNotDisturb &&
       (!item->ignore || item->grabbed) &&
       getDistance(item->getGlobalCenter()) < size) {
      itemInteract(item);
      break;
    }
  }

  if((settings->stats == STATS_MOUSEOVER || settings->stats == STATS_CRITICAL) &&
     stats->underMouse) {
    stats->show();
  }
  // This balances the interaction count which is increased when mouse is moved across Boris
  if(interactions > 10) {
    interactions = 10;
  }
  interactions--;
  if(interactions < 0) {
    interactions = 0;
  }

  if(annoyance > ANNOYMAX) {
    annoyance = ANNOYMAX;
  }
  annoyance = annoyance - 4;
  if(annoyance < 0) {
    annoyance = 0;
  }

  interactionsTimer.start();
}

void Boris::itemInteract(Item * item)
{
  if(!item->getReactionBehaviour().isEmpty()) {
    changeBehaviour(item->getReactionBehaviour());
    item->destroy();
  } else if(item->grabbed) {
    bubble->initBubble(pos().x(), pos().y(), size, stats->getHyper(), "I don't know what to do with that.", "_thought");
  }
}

void Boris::statChange(const QString &type, const int &amount)
{
  if(type == "hyper") {
    hyperQueue += amount;
  } else if(type == "health") {
    healthQueue += amount;
  } else if(type == "energy") {
    energyQueue += amount;
  } else if(type == "hunger") {
    hungerQueue += amount;
  } else if(type == "bladder") {
    bladderQueue += amount;
  } else if(type == "social") {
    socialQueue += amount;
  } else if(type == "fun") {
    funQueue += amount;
  } else if(type == "hygiene") {
    hygieneQueue += amount;
  }
}

// Used by scripthandler to immediately switch to behav using file
void Boris::behavFromFile(const QString &file)
{
  changeBehaviour(file);
}

// Used by mainwindow context menu to queue a behav from file
void Boris::queueBehavFromFile(const QString &file)
{
  behavQueue.append(file);
}

void Boris::setCurFrame(const int &frame)
{
  curFrame = frame;
}
