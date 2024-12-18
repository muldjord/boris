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
#include <QBitmap>
#include <QScreen>
#include <QTime>
#include <QRandomGenerator>

constexpr int STATTIMER = 200;
constexpr int ANNOYMAX = 42;

extern SoundMixer soundMixer;

Boris::Boris(Settings &settings) : settings(settings)
{
  int borisX = settings.borisX;
  int borisY = settings.borisY;
  if(borisY > QApplication::primaryScreen()->size().height() - height()) {
    borisY = QApplication::primaryScreen()->size().height() - height();
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

  origShadow = settings.getPixmap("shadow.png");
  shadowSprite = this->scene()->addPixmap(origShadow);
  shadowSprite->setPos(0, 16);
  shadowSprite->setOpacity(0.35);

  borisSprite = this->scene()->addPixmap(QPixmap());
  borisSprite->setPos(0, 15); // To make room for shadow

  scriptSprite = this->scene()->addPixmap(QPixmap());
  scriptSprite->setPos(0, 15); // To make room for shadow

  origDirt = settings.getPixmap("dirt.png");
  dirtSprite = this->scene()->addPixmap(origDirt);
  dirtSprite->setOpacity(0.0);
  dirtSprite->setPos(0, 15); // To make room for shadow

  origBruises = settings.getPixmap("bruises.png");
  bruisesSprite = this->scene()->addPixmap(origBruises);
  bruisesSprite->setOpacity(0.0);
  bruisesSprite->setPos(0, 15); // To make room for shadow

  weatherSprite = this->scene()->addPixmap(QPixmap());
  weatherSprite->setPos(0, 0);
  weatherSprite->hide();

  behavioursMenu = new QMenu();
  behavioursMenu->setTitle(tr("Behaviours"));
  connect(behavioursMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  createBehavioursMenu();

  // Set initial stats with some randomization
  int hyper = 0;
  int health = 100;
  int energy = 40 + QRandomGenerator::global()->bounded(25);
  int hunger = 40 + QRandomGenerator::global()->bounded(25);
  int toilet = 40 + QRandomGenerator::global()->bounded(25);
  int hygiene = 100;
  int anxiety = 0;
  int social = 40 + QRandomGenerator::global()->bounded(25);
  int fun = 40 + QRandomGenerator::global()->bounded(25);
  hyperQueue = 0;
  healthQueue = 0;
  energyQueue = 0;
  hungerQueue = 0;
  toiletQueue = 0;
  socialQueue = 0;
  funQueue = 0;
  hygieneQueue = 0;
  anxietyQueue = 0;
  stats = new Stats(settings, hyper, health, energy, hunger, toilet, social, fun, hygiene, anxiety, this);
  bubble = new Bubble(settings);

  staticBehaviours = 0;
  // Figure out how many static behaviours there are
  for(const auto &behaviour: settings.behaviours) {
    if(behaviour.file.left(1) == "_") {
      staticBehaviours++;
    }
  }

  behavTimer.setSingleShot(true);
  connect(&behavTimer, &QTimer::timeout, this, &Boris::nextBehaviour);

  physicsTimer.setInterval(30);
  connect(&physicsTimer, &QTimer::timeout, this, &Boris::handlePhysics);
  physicsTimer.start();

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

  interactionsTimer.setInterval(1000);
  interactionsTimer.setSingleShot(true);
  connect(&interactionsTimer, &QTimer::timeout, this, &Boris::checkInteractions);
  interactionsTimer.start();

  enableInteractTimer.setInterval(2000);
  enableInteractTimer.setSingleShot(true);
  connect(&enableInteractTimer, &QTimer::timeout, this, &Boris::enableInteract);

  setCursor(settings.getCursor("hover.png"));

  updateBoris();
}

Boris::~Boris()
{
  delete stats;
  delete bubble;
  delete behavioursMenu;
}

void Boris::createBehavioursMenu()
{
  QList<QMenu*> subMenus;

  QMenu *idkfaMenu = new QMenu(tr("Idkfa"), behavioursMenu);
  idkfaMenu->setIcon(QIcon(settings.getPixmap("idkfa.png")));

  for(const auto &behaviour: settings.behaviours) {
    QString title = behaviour.title;
    QPixmap iconPixmap(settings.getPixmap(behaviour.category.toLower() + ".png"));
    QMenu *menu = nullptr;

    // Find correct menu to put behaviour into
    if((behaviour.file.left(1) == "_" && behaviour.category.isEmpty()) ||
       behaviour.category == "Other" ||
       behaviour.category == "Locomotion") {
      menu = idkfaMenu;
    }
    if(menu == nullptr) {
      for(auto &subMenu: subMenus) {
        if(subMenu->title() == behaviour.category && behaviour.category != "Hidden") {
          menu = subMenu;
          break;
        }
      }
    }
    if(menu == nullptr) {
      menu = new QMenu(behaviour.category, behavioursMenu);
      menu->setIcon(QIcon(iconPixmap));
      subMenus.append(menu);
    }

    menu->addAction(QIcon(iconPixmap), title)->setData(behaviour.file);
  }

  for(auto &subMenu: subMenus) {
    if(!subMenu->isEmpty()) {
      behavioursMenu->addMenu(subMenu);
    }
  }

  if(settings.idkfa) {
    behavioursMenu->addSeparator();
    if(!idkfaMenu->isEmpty()) {
      behavioursMenu->addMenu(idkfaMenu);
    }
    QMenu *debugMenu = new QMenu(tr("Debug"), behavioursMenu);
    debugMenu->setIcon(QIcon(settings.getPixmap("idkfa.png")));
    debugAction = new DebugAction(debugMenu);
    debugMenu->addAction(debugAction);
    behavioursMenu->addMenu(debugMenu);
  }

  if(behavioursMenu->isEmpty()) {
    behavioursMenu->addAction(QIcon(settings.getPixmap("idkfa.png")), tr("No behaviours!"));
  }
}

QString Boris::getFileFromCategory(const QString &category)
{
  QList<QString> b;
  for(const auto &behaviour: settings.behaviours) {
    if(behaviour.category == category) {
      b.append(behaviour.file);
    }
  }
  int chosen = QRandomGenerator::global()->bounded(b.length());
  for(int i = 0; i < settings.behaviours.length(); ++i) {
    if(settings.behaviours.at(i).file == b.at(chosen)) {
      chosen = i;
      break;
    }
  }
  return settings.behaviours.at(chosen).file;
}

int Boris::getIdxFromCategory(const QString &category)
{
  QList<QString> b;
  for(const auto &behav: settings.behaviours) {
    if(behav.category == category) {
      b.append(behav.file);
    }
  }
  int chosen = QRandomGenerator::global()->bounded(b.length());
  for(int i = 0; i < settings.behaviours.length(); ++i) {
    if(settings.behaviours.at(i).file == b.at(chosen)) {
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
  if(settings.behaviours.at(curBehav).file == "_drop_dead") {
    return;
  }

  // Always stop behavTimer, just in case. At this point we never want it to be running
  behavTimer.stop();

  // Reset all script variables
  scriptVars.clear();

  // Clear script image canvas
  scriptImage.fill(Qt::transparent);
  drawing = false;

  bool coinCheck = false;
  // Check if there are behaviours in queue, these are prioritized
  if(behav == "" && !behavQueue.isEmpty()) {
    behav = behavQueue.takeFirst();
    coinCheck = true;
  }

  // Process the AI if no forced behaviour is set
  if(behav == "") {
    processAi(behav);
  }

  // Bias towards behaviours from 'Idle' and 'Locomotion' categories to make Boris less erratic
  if(QRandomGenerator::global()->bounded(10) >= 3) {
    if(QRandomGenerator::global()->bounded(70) >= stats->getEnergy()) {
      curBehav = getIdxFromCategory("Idle");
    } else {
      curBehav = getIdxFromCategory("Locomotion"); // This category DOES exist. See data/behaviours/README.md
    }
  } else {
    curBehav = QRandomGenerator::global()->bounded(settings.behaviours.size() - staticBehaviours) + staticBehaviours;
  }

  // If a specific behaviour is requested, use that
  if(!behav.isEmpty()) {
    for(int a = 0; a < settings.behaviours.size(); ++a) {
      if(settings.behaviours.at(a).file == behav) {
        curBehav = a;
        if(coinCheck) {
          int coins = 0;
          if(settings.behaviours.at(curBehav).category.toLower() == "health") {
            coins = (100 - stats->getHealth());
          } else if(settings.behaviours.at(curBehav).category.toLower() == "energy") {
            coins = (100 - stats->getEnergy());
          } else if(settings.behaviours.at(curBehav).category.toLower() == "hunger") {
            coins = stats->getHunger();
          } else if(settings.behaviours.at(curBehav).category.toLower() == "toilet") {
            coins = stats->getToilet();
          } else if(settings.behaviours.at(curBehav).category.toLower() == "social") {
            coins = (100 - stats->getSocial());
          } else if(settings.behaviours.at(curBehav).category.toLower() == "fun") {
            coins = (100 - stats->getFun());
          } else if(settings.behaviours.at(curBehav).category.toLower() == "hygiene") {
            coins = (100 - stats->getHygiene());
          } else if(settings.behaviours.at(curBehav).category.toLower() == "anxiety") {
            coins = stats->getAnxiety();
          }
          coins /= 20;

          if(coins) {
            emit addCoins("+" + QString::number(coins), coins);
          }
        }
        break;
      }
    }
  }

  // Applying behaviour stats to Boris
  hyperQueue += settings.behaviours.at(curBehav).hyper;
  healthQueue += settings.behaviours.at(curBehav).health;
  energyQueue += settings.behaviours.at(curBehav).energy;
  hungerQueue += settings.behaviours.at(curBehav).hunger;
  toiletQueue += settings.behaviours.at(curBehav).toilet;
  socialQueue += settings.behaviours.at(curBehav).social;
  funQueue += settings.behaviours.at(curBehav).fun;
  hygieneQueue += settings.behaviours.at(curBehav).hygiene;
  anxietyQueue += settings.behaviours.at(curBehav).anxiety;

  if(settings.behaviours.at(curBehav).allowFlip && QRandomGenerator::global()->bounded(2)) {
    flipFrames = true;
  } else {
    flipFrames = false;
  }

  if(!settings.behaviours.at(curBehav).oneShot) {
    if(time == -1) {
      time = QRandomGenerator::global()->bounded(7000) + 5000;
    }
    time = time - (time / 100.0 * stats->getHyper());
    behavTimer.setInterval(time);
    behavTimer.start();
  }

  curFrame = 0;
  // Look if a 'define init' exists in behaviour. If so, run it before starting first frame.
  if(settings.behaviours.at(curBehav).defines.contains("init")) {
    int stop = 0;
    runScript(stop, true);
  }

  animTimer.start(0, Qt::PreciseTimer, this);
}

QPixmap Boris::getShadow(const QPixmap &sprite)
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

void Boris::runScript(int &stop, const bool &init)
{
  // Update current stat variables for scripting use
  scriptVars["bsize"] = size;
  scriptVars["energy"] = stats->getEnergy();
  scriptVars["health"] = stats->getHealth();
  scriptVars["hyper"] = stats->getHyper();
  scriptVars["hunger"] = stats->getHunger();
  scriptVars["toilet"] = stats->getToilet();
  scriptVars["social"] = stats->getSocial();
  scriptVars["fun"] = stats->getFun();
  scriptVars["hygiene"] = stats->getHygiene();
  scriptVars["anxiety"] = stats->getAnxiety();
  scriptVars["borisx"] = pos().x() + (size / 2);
  scriptVars["borisy"] = pos().y() + size;
  QPoint p = QCursor::pos();
  scriptVars["mx"] = p.x();
  scriptVars["my"] = p.y();
  scriptVars["mxvel"] = mouseHVel;
  scriptVars["myvel"] = mouseVVel;
  scriptVars["mdist"] = getDistance(QCursor::pos());
  scriptVars["msec"] = getSector(QCursor::pos());
  if(borisFriend != nullptr) {
    scriptVars["fsec"] = getSector(QPoint(borisFriend->pos().x() + (size / 2), borisFriend->pos().y() + size));
  } else {
    scriptVars["fsec"] = -1;
  }
  scriptVars["xvel"] = 0;
  scriptVars["yvel"] = 0;

  QDate date = QDate::currentDate();
  QTime time = QTime::currentTime();
  scriptVars["day"] = date.day();
  scriptVars["month"] = date.month();
  scriptVars["year"] = date.year();
  scriptVars["hour"] = time.hour();
  scriptVars["minute"] = time.minute();
  scriptVars["second"] = time.second();
  scriptVars["weather"] = settings.weatherType.left(2).toInt();
  scriptVars["wind"] = settings.windSpeed;
  scriptVars["temp"] = settings.temperature;
  scriptVars["queued"] = behavQueue.count();

  ScriptHandler scriptHandler(&scriptImage, &drawing, settings, bubble, settings.behaviours.at(curBehav).labels, settings.behaviours.at(curBehav).defines, scriptVars, QPoint(pos().x(), pos().y()), size);
  connect(&scriptHandler, &ScriptHandler::behavFromFile, this, &Boris::behavFromFile);
  connect(&scriptHandler, &ScriptHandler::setCurFrame, this, &Boris::setCurFrame);
  connect(&scriptHandler, &ScriptHandler::statChange, this, &Boris::statChange);
  if(init) {
    scriptHandler.runScript(stop, settings.behaviours.at(curBehav).defines["init"]);
  } else {
    scriptHandler.runScript(stop, settings.behaviours.at(curBehav).frames.at(curFrame).script);
  }

  if(flipFrames) {
    QImage flipped = scriptImage.mirrored(true, false);
    scriptSprite->setPixmap(QPixmap::fromImage(flipped));
  } else {
    scriptSprite->setPixmap(QPixmap::fromImage(scriptImage));
  }
  if(!drawing) {
    scriptImage.fill(Qt::transparent);
  }
  if(scriptVars["xvel"] != 0 &&
     scriptVars["yvel"] != 0) {
    falling = true;
    hVel = scriptVars["xvel"];
    vVel = scriptVars["yvel"];
    altitude = pos().y();
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

void Boris::timerEvent(QTimerEvent *)
{
  if(stopNextBehaviour) {
    stopNextBehaviour = false;
    behavTimer.stop();
    nextBehaviour();
    return;
  }

  sanityCheck();

  if(curFrame >= settings.behaviours.at(curBehav).frames.count()) {
    curFrame = 0;
    if(settings.behaviours.at(curBehav).oneShot) {
      changeBehaviour();
      return;
    }
  }

  QBitmap mask = settings.behaviours.at(curBehav).frames.at(curFrame).sprite.mask();

  QPixmap dirtPixmap(origDirt);
  dirtPixmap.setMask(mask);

  QPixmap bruisesPixmap(origBruises);
  bruisesPixmap.setMask(mask);

  if(flipFrames) {
    QImage flipped = settings.behaviours.at(curBehav).frames.at(curFrame).sprite.toImage().mirrored(true, false);
    borisSprite->setPixmap(QPixmap::fromImage(flipped));
    shadowSprite->setPixmap(getShadow(QPixmap::fromImage(flipped)));
    dirtSprite->setPixmap(QPixmap::fromImage(dirtPixmap.toImage().mirrored(true, false)));
    bruisesSprite->setPixmap(QPixmap::fromImage(bruisesPixmap.toImage().mirrored(true, false)));
  } else {
    borisSprite->setPixmap(settings.behaviours.at(curBehav).frames.at(curFrame).sprite);
    shadowSprite->setPixmap(getShadow(settings.behaviours.at(curBehav).frames.at(curFrame).sprite));
    dirtSprite->setPixmap(dirtPixmap);
    bruisesSprite->setPixmap(bruisesPixmap);
  }

  if(settings.sound && settings.behaviours.at(curBehav).frames.at(curFrame).soundBuffer != nullptr) {
    if(settings.behaviours.at(curBehav).pitchLock) {
      soundMixer.playSound(settings.behaviours.at(curBehav).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings.desktopWidth * 2.0 - 1.0,
                           (stats->getHyper() / 60.0) + 1);
    } else {
      soundMixer.playSound(settings.behaviours.at(curBehav).frames.at(curFrame).soundBuffer,
                           (float)pos().x() / (float)settings.desktopWidth * 2.0 - 1.0,
                           (stats->getHyper() / 60.0) + (0.95 + QRandomGenerator::global()->bounded(100) / 1000.0));
    }
  }

  if(settings.behaviours.at(curBehav).frames.at(curFrame).dx != 0 ||
     settings.behaviours.at(curBehav).frames.at(curFrame).dy != 0) {
    moveBoris(settings.behaviours.at(curBehav).frames.at(curFrame).dx,
              settings.behaviours.at(curBehav).frames.at(curFrame).dy,
              flipFrames);
  }

  int frameTime = settings.behaviours.at(curBehav).frames.at(curFrame).time;
  if(frameTime != 0) { // Allow frameTime at 0 if it is specifically set in the script frame. This is to allow 'invisible' frames, such as used in 'mazegame'
    frameTime -= (frameTime / 100.0 * stats->getHyper());
    if(frameTime <= 5) {
      frameTime = 5;
    }
  }

  int stop = 0; // Will be > 0 if a goto, behav or break command is run
  runScript(stop);
  if(stop == 1) {
    // In case of 'goto' curFrame has been set in scriptHandler
    // In case of 'behav' behavFromFile has been emitted
    // In case of 'return' will simply advance to the next frame
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

  int maxX = QApplication::primaryScreen()->size().width() - size;
  int minY = 0 - (size / 2);
  int maxY = QApplication::primaryScreen()->size().height() - height();

  if(dX == 666) {
    dX = QRandomGenerator::global()->bounded(maxX);
  } else {
    if(flipped) {
      dX *= -1;
    }
    dX = pos().x() + (dX * settings.sizeFactor);
  }
  if(dY == 666) {
    dY = QRandomGenerator::global()->bounded(maxY);
  } else {
    dY = pos().y() + (dY * settings.sizeFactor);
  }

  move(dX, dY);
  bubble->moveBubble(pos().x(), pos().y(), size);
  stats->move(pos().x() + (size / 2) - (stats->width() / 2),
              pos().y() + size + (size * 0.6));
  // if Boris is outside borders
  if(pos().y() > maxY || pos().y() < minY) {
    if(falling) {
      healthQueue -= 5; // It hurts to hit the borders
      // Physics velocity when hitting borders
      hVel *= 0.2;
      vVel = 0.0;
    } else if(!grabbed) {
      changeBehaviour();
    }
  }
  if(vision && settings.vision && !falling && !grabbed) {
    processVision();
  }
}

void Boris::handleBehaviourChange(QAction* a)
{
  QString behavFile = a->data().toString();
  for(const auto &behaviour: settings.behaviours) {
    if(behaviour.file == behavFile) {
      behavQueue.append(behaviour.file);
    }
  }
}

void Boris::enterEvent(QEnterEvent *event)
{
  event->accept();
  if(settings.stats == STATS_MOUSEOVER || settings.stats == STATS_CRITICAL) {
    stats->show();
  }
  stats->underMouse = true;
}

void Boris::leaveEvent(QEvent *event)
{
  event->accept();
  if(settings.stats == STATS_MOUSEOVER || settings.stats == STATS_CRITICAL) {
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
    if(settings.behaviours.at(curBehav).file == "_sleep" && stats->getEnergy() <= 95) {
      energyQueue -= 25;
      funQueue -= 20;
    }
    setCursor(settings.getCursor("grab.png"));
    grabbed = true;
    changeBehaviour("_grabbed");
    this->move(event->globalPosition().x() - settings.sizeFactor * 17.0, 
               event->globalPosition().y() - settings.sizeFactor * 16.0);
    oldCursor = QCursor::pos();
  }
}

void Boris::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons().testFlag(Qt::LeftButton)) {
    this->move(event->globalPosition().x() - settings.sizeFactor * 17.0, 
               event->globalPosition().y() - settings.sizeFactor * 16.0);
    stats->move(pos().x() + (size / 2) - (stats->width() / 2),
                pos().y() + size + (size / 2));
    bubble->moveBubble(pos().x(), pos().y(), size);
  }

}

void Boris::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton) {
    setCursor(settings.getCursor("hover.png"));
    grabbed = false;
    settings.borisX = pos().x();
    settings.borisY = pos().y();
    changeBehaviour("_falling");
    falling = true;
    hVel = mouseHVel;
    vVel = mouseVVel;
    altitude = QCursor::pos().y();
  }
}

void Boris::wheelEvent(QWheelEvent *)
{
  if(stats->underMouse && !falling && !grabbed && !settings.behaviours.at(curBehav).doNotDisturb) {
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
  if(falling && !grabbed) {
    if(shadowSprite->isVisible()) {
      shadowSprite->hide();
    }
    moveBoris(hVel, vVel);
    vVel += 0.5;
    if(settings.behaviours.at(curBehav).file != "_umbrella_falling") {
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
    if(pos().y() >= altitude) {
      move(pos().x(), altitude - (pos().y() - altitude));
      if(vVel < 5.0) {
        if(settings.behaviours.at(curBehav).file != "_umbrella_falling") {
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
  } else {
    if(!shadowSprite->isVisible()) {
      shadowSprite->show();
    }
  }
  mouseHVel = (QCursor::pos().x() - oldCursor.x()) * 0.25; // 0.25 is just an abstract factor that relates well to the 30 fps physics
  mouseVVel = (QCursor::pos().y() - oldCursor.y()) * 0.25;
  oldCursor = QCursor::pos();

  if(!falling && !grabbed) {
    if(getDistance(QCursor::pos()) < size * 3) {
      if(!mouseHovering) {
        interactions += 2; // Should be 2 due to checkInteractions now run every 1 second instead of 2
        if(!settings.behaviours.at(curBehav).doNotDisturb) {
          if(stats->getAnxiety() >= QRandomGenerator::global()->bounded(20) + 10) {
            changeBehaviour("_flee");
          } else if(stats->getSocial() < QRandomGenerator::global()->bounded(interactions * 40)) {
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
    altitude = pos().y();
  }
}

void Boris::statProgress()
{
  // If it's late decrease energy to make Boris tired
  // Other than that energy decreases are mainly controlled by walking behaviours
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
  hungerQueue += QRandomGenerator::global()->bounded(2); // This one is opposite in the stats overview
  socialQueue -= QRandomGenerator::global()->bounded(2);
  hygieneQueue -= QRandomGenerator::global()->bounded(1);
  anxietyQueue -= QRandomGenerator::global()->bounded(8) + 8;
  funQueue -= QRandomGenerator::global()->bounded(3);
  // Nothing needed for 'health' and 'toilet'
}

void Boris::sanityCheck()
{
  int minX = - size;
  int maxX = QApplication::primaryScreen()->size().width();
  int minY = 0 - (size / 2);
  int maxY = QApplication::primaryScreen()->size().height() - height();

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
  if(altitude > maxY) {
    altitude = maxY;
  }

  // Check if Boris is dying or is already dead
  if(isAlive) {
    if(stats->getHealth() <= 2) {
      killBoris();
    }
  }
}

void Boris::killBoris()
{
  if(settings.isInvincible) {
    qInfo("Boris is in a bad shape, but is invincible! Phew...\n");
    stats->deltaHealth(100);
  } else {
    qInfo("Boris has died... RIP!\n");
    isAlive = false;
    statQueueTimer.stop();
    statTimer.stop();
    dirtSprite->setOpacity(0.0);
    bruisesSprite->setOpacity(0.0);
    changeBehaviour("_drop_dead");
  }
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
  if(toiletQueue > 100)
    toiletQueue = 100;
  if(socialQueue > 100)
    socialQueue = 100;
  if(funQueue > 100)
    funQueue = 100;
  if(hygieneQueue > 100)
    hygieneQueue = 100;
  if(anxietyQueue > 100)
    anxietyQueue = 100;
  if(hyperQueue < -100)
    hyperQueue = -100;
  if(healthQueue < -100)
    healthQueue = -100;
  if(energyQueue < -100)
    energyQueue = -100;
  if(hungerQueue < -100)
    hungerQueue = -100;
  if(toiletQueue < -100)
    toiletQueue = -100;
  if(socialQueue < -100)
    socialQueue = -100;
  if(funQueue < -100)
    funQueue = -100;
  if(hygieneQueue < -100)
    hygieneQueue = -100;
  if(anxietyQueue < -100)
    anxietyQueue = -100;

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
    stats->deltaHunger(1);
  }
  if(hungerQueue < 0) {
    hungerQueue++;
    stats->deltaHunger(-1);
  }
  if(toiletQueue > 0) {
    toiletQueue--;
    stats->deltaToilet(1);
  }
  if(toiletQueue < 0) {
    toiletQueue += 2;
    stats->deltaToilet(-2);
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
  if(anxietyQueue > 0) {
    anxietyQueue -= 2;
    stats->deltaAnxiety(2);
  }
  if(anxietyQueue < 0) {
    anxietyQueue++;
    stats->deltaAnxiety(-1);
  }

  dirtSprite->setOpacity(0.50 - ((qreal)stats->getHygiene()) * 0.01);
  bruisesSprite->setOpacity(0.75 - ((qreal)stats->getHealth()) * 0.01);
  if(debugAction != nullptr) {
    debugAction->setText("Stats:\n"
                         "Anxiety: " + QString::number(stats->getAnxiety()) + "\n" +
                         "Energy: " + QString::number(stats->getEnergy()) + "\n" +
                         "Fun: " + QString::number(stats->getFun()) + "\n" +
                         "Health: " + QString::number(stats->getHealth()) + "\n" +
                         "Hunger: " + QString::number(stats->getHunger()) + "\n" +
                         "Hygiene: " + QString::number(stats->getHygiene()) + "\n" +
                         "Hyper: " + QString::number(stats->getHyper()) + "\n" +
                         "Social: " + QString::number(stats->getSocial()) + "\n" +
                         "Toilet: " + QString::number(stats->getToilet()) + "\n");
  }
  stats->updateStats();

  statQueueTimer.start();
}

int Boris::getEnergy() const
{
  return stats->getEnergy();
}

int Boris::getHealth() const
{
  return stats->getHealth();
}

int Boris::getHyper() const
{
  return stats->getHyper();
}

int Boris::getHunger() const
{
  return stats->getHunger();
}

int Boris::getToilet() const
{
  return stats->getToilet();
}

int Boris::getSocial() const
{
  return stats->getSocial();
}

int Boris::getFun() const
{
  return stats->getFun();
}

int Boris::getHygiene() const
{
  return stats->getHygiene();
}

int Boris::getAnxiety() const
{
  return stats->getAnxiety();
}

void Boris::processVision()
{
  int border = 4;
  int contrast = 15;
  // FIXME: This is broken!
  //QImage vision = QImageQGuiApplication::primaryScreen()->grabWindow(QApplication::primaryScreen()->size().winId(), pos().x() - border, pos().y() - border, size + border * 2, size + border * 2).toImage();
  QImage vision = QImage();

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
  if(timeForWeather >= 50) {
    timeForWeather = 0;
    showWeather(behav);
  }

  if(behav == "" && QRandomGenerator::global()->bounded(2)) {
    // Stat check
    QList<QString> potentials;
    if(stats->getFun() <= 50) {
      if(QRandomGenerator::global()->bounded(stats->getFun()) > independence) {
        potentials.append("_fun");
      } else {
        potentials.append(getFileFromCategory("Fun"));
      }
    }
    if(stats->getEnergy() <= 50) {
      if(QRandomGenerator::global()->bounded(100) > independence) {
        potentials.append("_energy");
      } else {
        potentials.append(getFileFromCategory("Energy"));
      }
    }
    if(stats->getSocial() <= 50) {
      if(QRandomGenerator::global()->bounded(100) > independence) {
        potentials.append("_social");
      } else {
        potentials.append(getFileFromCategory("Social"));
      }
    }
    if(stats->getHygiene() <= 50) {
      if(QRandomGenerator::global()->bounded(100) > independence) {
        potentials.append("_hygiene");
      } else {
        potentials.append(getFileFromCategory("Hygiene"));
      }
    }
    if(stats->getHealth() <= 50) {
      if(QRandomGenerator::global()->bounded(150 - stats->getHealth()) < independence) {
        potentials.append("_health");
      }
    }
    // This one is 'inverted' compared to the others. High 'hunger' means Boris is starving.
    if(stats->getHunger() >= 50) {
      if(QRandomGenerator::global()->bounded(100) > independence) {
        potentials.append("_hunger");
      } else {
        potentials.append(getFileFromCategory("Hunger"));
      }
    }
    // This one is 'inverted' compared to the others. High 'toilet' means Boris has to go to the bathroom.
    if(stats->getToilet() >= 50) {
      if(QRandomGenerator::global()->bounded(100) > independence) {
        potentials.append("_toilet");
      } else {
        potentials.append(getFileFromCategory("Toilet"));
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
      } else if(behav == "_toilet") {
        stats->flashStat("toilet");
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
  size = settings.size;
  if(size == 0) {
    size = QRandomGenerator::global()->bounded(257 - 32) + 32; // Make him at least 32 and max 256
  }
  setFixedSize(size, size + (size / 2.0));
  resetTransform();
  scale(settings.sizeFactor, settings.sizeFactor);

  // Set new independence value
  independence = settings.independence;

  // Show or hide stats
  if(settings.stats == STATS_ALWAYS) {
    stats->show();
  } else {
    stats->hide();
  }
  moveBoris(0, 0);
}

void Boris::nextWeatherFrame()
{
  weatherSprite->setPixmap(settings.weathers.at(curWeather).frames.at(curWeatherFrame).sprite);
  weatherTimer.setInterval(settings.weathers.at(curWeather).frames.at(curWeatherFrame).time);
  curWeatherFrame++;
  if(curWeatherFrame >= settings.weathers.at(curWeather).frames.length()) {
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
  for(int a = 0; a < settings.weathers.count(); ++a) {
    if(settings.weathers.at(a).file == settings.weatherType) {
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
    if(settings.weatherType == "01d") {
      behavQueue.append("sunglasses");
    } else if(settings.weatherType == "02d" && settings.temperature >= 15) {
      behavQueue.append("sunglasses");
    } else if(settings.weatherType == "09d" || settings.weatherType == "09n" ||
              settings.weatherType == "10d" || settings.weatherType == "10n") {
      behavQueue.append("_umbrella");
    } else if(settings.weatherType == "11d" || settings.weatherType == "11n") {
      behav = "_lightning";
    } else if(settings.weatherType == "13d" || settings.weatherType == "13n") {
      behavQueue.append("_freezing");
    } else if(settings.weatherType == "01n" || settings.weatherType == "02n") {
      behavQueue.append("_energy"); // Yawn for weathers that have a moon
    } else if(settings.weatherType == "04d" || settings.weatherType == "04n" ||
              settings.weatherType == "50d" || settings.weatherType == "50n") {
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

void Boris::enableInteract()
{
  interact = true;
  borisFriend = nullptr;
}

void Boris::checkInteractions()
{
  // Check if there are any collisions with other Borises
  for(auto &boris: settings.borisList) {
    if(boris != this) {
      if(getDistance(boris->getGlobalCenter()) < size * 2) {
        borisInteract(boris);
        break;
      }
    }
  }

  // Check if user is dragging any items close by
  for(auto &item: settings.itemList) {
    if(interact && !falling && !grabbed && !settings.behaviours.at(curBehav).doNotDisturb &&
       (!item->ignore || item->grabbed) &&
       getDistance(item->getGlobalCenter()) < (size * 0.75)) {
      itemInteract(item);
      break;
    }
  }

  if((settings.stats == STATS_MOUSEOVER || settings.stats == STATS_CRITICAL) &&
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
  annoyance = annoyance - 2;
  if(annoyance < 0) {
    annoyance = 0;
  }

  interactionsTimer.start();
}

void Boris::borisInteract(Boris *boris)
{
  if(pos().y() > boris->pos().y()) {
    // Bring this Boris to the front, because he is considered closer in 3D space
    raise();
    setFocus();
  }

  if(borisFriend != nullptr || falling || grabbed || settings.behaviours.at(curBehav).doNotDisturb) {
    return;
  }
  borisFriend = boris;
  // Queue current behaviour so it isn't 'forgotten'
  behavQueue.prepend(settings.behaviours.at(curBehav).file);

  if(settings.behaviours.at(borisFriend->getCurBehav()).file == "_drop_dead") {
    changeBehaviour("_sad");
  } else if(borisFriend->getHygiene() <= 22) {
    changeBehaviour("_hygiene");
  } else {
    changeBehaviour("_wave");
  }
  enableInteractTimer.setInterval(QRandomGenerator::global()->bounded(5000) + 17000);
  enableInteractTimer.start();
}

void Boris::itemInteract(Item * item)
{
  // Disable interactions for a while
  interact = false;

  if(!item->getReactionBehaviour().isEmpty()) {
    changeBehaviour(item->getReactionBehaviour());
    item->interact(this);
  } else if(item->grabbed) {
    bubble->initBubble(pos().x(), pos().y(), size, stats->getHyper(), "I don't know what to do with that.", "_thought");
  }
  enableInteractTimer.setInterval(5000);
  enableInteractTimer.start();
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
  } else if(type == "toilet") {
    toiletQueue += amount;
  } else if(type == "social") {
    socialQueue += amount;
  } else if(type == "fun") {
    funQueue += amount;
  } else if(type == "hygiene") {
    hygieneQueue += amount;
  } else if(type == "anxiety") {
    anxietyQueue += amount;
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
