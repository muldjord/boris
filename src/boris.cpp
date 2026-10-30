/*
Boris
Copyright (C) 2026 Lars Muldjord

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "boris.h"

#include "bubble.h"
#include "debugaction.h"
#include "item.h"
#include "mainwindow.h"
#include "scripthandler.h"
#include "settings.h"
#include "shadow.h"
#include "soundmixer.h"
#include "stats.h"
#include "weather.h"

#include <algorithm>

#include <QAction>
#include <QBitmap>
#include <QCursor>
#include <QDate>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QMenu>
#include <QPainter>
#include <QPointF>
#include <QRandomGenerator>
#include <QRectF>
#include <QString>
#include <QTime>
#include <QTimer>

constexpr int ANNOYMAX = 42;

extern MainWindow *mainWindow;
extern SoundMixer *soundMixer;

Boris::Boris(Settings &settings)
  : settings(settings)
{
  setAcceptHoverEvents(true);

  setPos(QRandomGenerator::global()->bounded(settings.desktopWidth), QRandomGenerator::global()->bounded(settings.desktopHeight));

  origDirt = settings.getPixmap("dirt.png");
  origBruises = settings.getPixmap("bruises.png");

  behavioursMenu = new QMenu();
  behavioursMenu->setTitle(tr("Behaviours"));
  connect(behavioursMenu, &QMenu::triggered, this, &Boris::handleBehaviourChange);
  createBehavioursMenu();

  // Set initial stats with some randomization
  int energy = 40 + QRandomGenerator::global()->bounded(25);
  int social = 40 + QRandomGenerator::global()->bounded(25);
  int fun = 40 + QRandomGenerator::global()->bounded(25);
  int hunger = 40 + QRandomGenerator::global()->bounded(25);
  int toilet = 40 + QRandomGenerator::global()->bounded(25);

  int anxiety = 0;
  int health = 100;
  int hygiene = 100;
  int hyper = 0;

  int independence = settings.independence;

  stats = new Stats(settings, energy, social, fun, hunger, toilet, anxiety, health, hygiene, hyper, independence);
  stats->setParentItem(this);
  stats->setPos((getWidth() / 2) - 40, -100);

  bubble = new Bubble(settings);
  bubble->setParentItem(this);

  weather = new Weather(settings);
  weather->setParentItem(this);
  weather->setPos(0, 0 - getHeight() / 2);

  shadow = new Shadow(settings);
  shadow->setParentItem(this);
  shadow->setPos(0, 1);

  staticBehaviours = 0;
  // Figure out how many static behaviours there are
  for(const auto &behaviour: settings.behaviours) {
    if(behaviour.file.left(1) == "_") {
      staticBehaviours++;
    }
  }

  behavTimer.setSingleShot(true);
  connect(&behavTimer, &QTimer::timeout, this, &Boris::nextBehaviour);

  interactionsTimer.setInterval(1000);
  interactionsTimer.setSingleShot(true);
  connect(&interactionsTimer, &QTimer::timeout, this, &Boris::checkInteractions);
  interactionsTimer.start();

  enableInteractTimer.setInterval(2000);
  enableInteractTimer.setSingleShot(true);
  connect(&enableInteractTimer, &QTimer::timeout, this, &Boris::enableInteract);

  setCursor(settings.getCursor("hover.png"));

  reconfBoris();

  animTimer.start(0, Qt::PreciseTimer, this);
}

Boris::~Boris()
{
}

void Boris::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->drawPixmap(0, 0, rendered);
}

QRectF Boris::boundingRect() const
{
  return QRectF(0, 0, rendered.width(), rendered.height());
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
            emit deltaCoins(coins);
          }
        }
        break;
      }
    }
  }

  // Applying behaviour stats to Boris
  stats->queueStatChange(StatType::HYPER, settings.behaviours.at(curBehav).hyper);
  stats->queueStatChange(StatType::HEALTH,  settings.behaviours.at(curBehav).health);
  stats->queueStatChange(StatType::ENERGY,  settings.behaviours.at(curBehav).energy);
  stats->queueStatChange(StatType::HUNGER, settings.behaviours.at(curBehav).hunger);
  stats->queueStatChange(StatType::TOILET, settings.behaviours.at(curBehav).toilet);
  stats->queueStatChange(StatType::SOCIAL, settings.behaviours.at(curBehav).social);
  stats->queueStatChange(StatType::FUN, settings.behaviours.at(curBehav).fun);
  stats->queueStatChange(StatType::HYGIENE, settings.behaviours.at(curBehav).hygiene);
  stats->queueStatChange(StatType::ANXIETY, settings.behaviours.at(curBehav).anxiety);

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

void Boris::runScript(int &stop, const bool &init)
{
  QPointF cursorPos = mainWindow->mapToScene(mainWindow->viewport()->mapFromGlobal(QCursor::pos()));

  // Update current stat variables for scripting use
  scriptVars["bsize"] = getWidth();
  scriptVars["energy"] = stats->getEnergy();
  scriptVars["health"] = stats->getHealth();
  scriptVars["hyper"] = stats->getHyper();
  scriptVars["hunger"] = stats->getHunger();
  scriptVars["toilet"] = stats->getToilet();
  scriptVars["social"] = stats->getSocial();
  scriptVars["fun"] = stats->getFun();
  scriptVars["hygiene"] = stats->getHygiene();
  scriptVars["anxiety"] = stats->getAnxiety();
  scriptVars["borisx"] = pos().x() + (getWidth() / 2);
  scriptVars["borisy"] = pos().y() + (getHeight() / 2);
  scriptVars["mx"] = cursorPos.x();
  scriptVars["my"] = cursorPos.y();
  scriptVars["mxvel"] = settings.mouseHVel;
  scriptVars["myvel"] = settings.mouseVVel;
  scriptVars["mdist"] = getDistance(cursorPos);
  scriptVars["msec"] = getDirectionTo(cursorPos);
  if(borisFriend != nullptr) {
    scriptVars["fsec"] = getDirectionTo(QPoint(borisFriend->pos().x() + (borisFriend->getWidth() / 2),
                                               borisFriend->pos().y() + (borisFriend->getHeight() / 2)));
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

  ScriptHandler scriptHandler(&scriptImage, &drawing, settings, bubble, settings.behaviours.at(curBehav).labels, settings.behaviours.at(curBehav).defines, scriptVars, pos());
  connect(&scriptHandler, &ScriptHandler::behavFromFile, this, &Boris::behavFromFile);
  connect(&scriptHandler, &ScriptHandler::setCurFrame, this, &Boris::setCurFrame);
  connect(&scriptHandler, &ScriptHandler::queueStatChange, stats, &Stats::queueStatChange);
  if(init) {
    scriptHandler.runScript(stop, settings.behaviours.at(curBehav).defines["init"]);
  } else {
    scriptHandler.runScript(stop, settings.behaviours.at(curBehav).frames.at(curFrame).script);
  }

  if(flipFrames) {
    QImage flipped = scriptImage.flipped(Qt::Horizontal);
    scriptSprite = QPixmap::fromImage(flipped);
  } else {
    scriptSprite = QPixmap::fromImage(scriptImage);
  }
  if(!drawing) {
    scriptImage.fill(Qt::transparent);
  }
  if(scriptVars["xvel"] != 0 &&
     scriptVars["yvel"] != 0) {
    falling = true;
    hVel = scriptVars["xvel"];
    vVel = scriptVars["yvel"];
    groundY = pos().y();
  }
}

int Boris::getDistance(const QPointF &p)
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
  return QPoint(pos().x() + (getWidth() / 2),
                pos().y() + (getHeight() / 2));
}

int Boris::getDirectionTo(const QPointF &p)
{
  // Center coordinate of Boris
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
    QImage flipped = settings.behaviours.at(curBehav).frames.at(curFrame).sprite.toImage().flipped(Qt::Horizontal);
    borisSprite = QPixmap::fromImage(flipped);
    dirtSprite = QPixmap::fromImage(dirtPixmap.toImage().flipped(Qt::Horizontal));
    bruisesSprite = QPixmap::fromImage(bruisesPixmap.toImage().flipped(Qt::Horizontal));
  } else {
    borisSprite = settings.behaviours.at(curBehav).frames.at(curFrame).sprite;
    dirtSprite = dirtPixmap;
    bruisesSprite = bruisesPixmap;
  }

  if(settings.sound && settings.behaviours.at(curBehav).frames.at(curFrame).soundBuffer != nullptr) {
    if(settings.behaviours.at(curBehav).pitchLock) {
      soundMixer->playSound(settings.behaviours.at(curBehav).frames.at(curFrame).soundBuffer,
                            pos().x() / settings.desktopWidth * 2.0 - 1.0,
                            (stats->getHyper() / 60.0) + 1);
    } else {
      soundMixer->playSound(settings.behaviours.at(curBehav).frames.at(curFrame).soundBuffer,
                            pos().x() / settings.desktopWidth * 2.0 - 1.0,
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

  // Assemble the Boris sprite from parts
  prepareGeometryChange(); // Needed to make sure the parent scene has its geometry index updated or something... Otherwise boundingRect might report faulty size.
  rendered = QPixmap(borisSprite.width(), borisSprite.height());
  rendered.fill(Qt::transparent);
  QPainter painter(&rendered);
  painter.drawPixmap(0, 0, borisSprite);
  painter.drawPixmap(0, 0, scriptSprite);
  painter.setOpacity(dirtOpacity);
  painter.drawPixmap(0, 0, dirtSprite);
  painter.setOpacity(bruisesOpacity);
  painter.drawPixmap(0, 0, bruisesSprite);
  painter.end();

  // ALso update the shadow here in case the sprite doesn't move which misaligns it
  shadow->updateShadow(rendered, groundY - pos().y(), falling);
  // Make sure 'paint()' is called
  update();

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

void Boris::moveBoris(int dX, int dY, const bool &flipped)
{
  sanityCheck();

  int maxX = settings.desktopWidth - getWidth();
  int minY = 0;
  int maxY = settings.desktopHeight - getHeight();

  if(dX == 666) { // Not pretty but it works
    dX = QRandomGenerator::global()->bounded(maxX);
  } else {
    if(flipped) {
      dX *= -1;
    }
    dX = pos().x() + dX;
  }
  if(dY == 666) { // Not pretty but it works
    dY = QRandomGenerator::global()->bounded(maxY);
  } else {
    dY = pos().y() + dY;
  }

  if(!falling) {
    groundY = dY;
  }
  shadow->updateShadow(rendered, groundY - dY, falling);
  shadow->setPos(0, groundY - dY + 1);
  setPos(dX, dY);

  // if Boris is outside vertical borders
  if(pos().y() > maxY || pos().y() < minY) {
    if(falling) {
      stats->queueStatChange(StatType::HEALTH, -5); // It hurts to hit the borders
      // Physics velocity when hitting borders
      hVel *= 0.2;
      vVel = 0.0;
    } else if(!grabbed) {
      changeBehaviour();
    }
  }
}

void Boris::handleBehaviourChange(QAction* action)
{
  QString behavFile = action->objectName();
  for(const auto &behaviour: settings.behaviours) {
    if(behaviour.file == behavFile) {
      behavQueue.append(behaviour.file);
    }
  }
}

void Boris::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  event->accept();
  if(settings.stats == STATS_MOUSEOVER || settings.stats == STATS_CRITICAL) {
    stats->show();
  }
  stats->underMouse = true;
}

void Boris::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  event->accept();
  if(settings.stats == STATS_MOUSEOVER || settings.stats == STATS_CRITICAL) {
    stats->hide();
  }
  stats->underMouse = false;
}

void Boris::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if(event->button() == Qt::RightButton) {
    behavioursMenu->exec(QCursor::pos());
  }
  if(event->button() == Qt::LeftButton) {
    if(settings.behaviours.at(curBehav).file == "_sleep" && stats->getEnergy() <= 95) {
      stats->queueStatChange(StatType::ENERGY, -25);
      stats->queueStatChange(StatType::FUN, -20);
    }
    setCursor(settings.getCursor("grab.png"));
    grabbed = true;
    changeBehaviour("_grabbed");
    this->setPos(event->scenePos().x() - (getWidth() / 2), 
                 event->scenePos().y());
  }
}

void Boris::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if(grabbed && event->buttons().testFlag(Qt::LeftButton)) {
    this->setPos(event->scenePos().x() - (getWidth() / 2), 
                 event->scenePos().y());
  }
}

void Boris::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if(grabbed && event->button() == Qt::LeftButton) {
    setCursor(settings.getCursor("hover.png"));
    grabbed = false;
    changeBehaviour("_falling");
    falling = true;
    hVel = settings.mouseHVel;
    vVel = settings.mouseVVel;
    groundY = pos().y();
  }
}

void Boris::wheelEvent(QGraphicsSceneWheelEvent *)
{
  if(!falling && !grabbed && !settings.behaviours.at(curBehav).doNotDisturb) {
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
  // Update opacity of dirt and bruises if Boris is alive
  if(isAlive) {
    dirtOpacity = 0.50 - ((qreal)stats->getHygiene()) * 0.01;
    bruisesOpacity = 0.75 - ((qreal)stats->getHealth()) * 0.01;
  }

  // Set scene distance so Boris goes behind other Borises that are in front of him
  setZValue(pos().y() - settings.desktopHeight);

  if(falling && !grabbed) {
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
    if(pos().y() + vVel >= groundY) {
      setPos(pos().x(), groundY); // Necessary to avoid Boris jumping back up if vector becomes too low "underground"
      if(vVel < 5.0) { // This is higher than for an item as it looks weird when he keeps bouncing if it's set at 2.0
        if(settings.behaviours.at(curBehav).file != "_umbrella_falling") {
          changeBehaviour("_landing");
        } else {
          changeBehaviour("_umbrella_landing");
        }
        falling = false;
        moveBoris(0, 0); // Make sure location of Boris and shadow is synchronized
      } else {
        hVel *= 0.5;
        vVel = (vVel * 0.5) * -1;
        changeBehaviour("_bounce");
      }
    }
  }

  QPointF cursorPos = mainWindow->mapToScene(mainWindow->viewport()->mapFromGlobal(QCursor::pos()));
  if(!falling && !grabbed) {
    if(getDistance(cursorPos) < getWidth() * 2) {
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
    groundY = pos().y();
  }
}

void Boris::sanityCheck()
{
  int minX = - getWidth();
  int maxX = settings.desktopWidth;
  int minY = 0;
  int maxY = settings.desktopHeight - getHeight();

  // Make sure Boris is not located outside boundaries
  if(pos().x() > maxX) {
    setPos(minX, pos().y());
  }
  if(pos().x() < minX) {
    setPos(maxX, pos().y());
  }
  if(pos().y() < minY) {
    setPos(pos().x(), minY);
  }
  if(pos().y() > maxY) {
    setPos(pos().x(), maxY);
  }

  // Make sure Boris groundY is not outside bottom boundary
  if(groundY > maxY) {
    groundY = maxY;
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
    stats->setStat(StatType::HEALTH, 100);
  } else {
    qInfo("Boris has died... RIP!\n");
    isAlive = false;
    stats->stopTimers();
    dirtOpacity = 0.0;
    bruisesOpacity = 0.0;
    changeBehaviour("_drop_dead");
  }
}

void Boris::updateDebugInfo()
{
  if(debugAction != nullptr) {
    debugAction->setText("STATS:\n"
                         "Energy: " + QString::number(stats->getEnergy()) + "\n" +
                         "Social: " + QString::number(stats->getSocial()) + "\n" +
                         "Fun: " + QString::number(stats->getFun()) + "\n" +
                         "Hunger: " + QString::number(stats->getHunger()) + "\n" +
                         "Toilet: " + QString::number(stats->getToilet()) + "\n" +
                         "Anxiety: " + QString::number(stats->getAnxiety()) + "\n" +
                         "Health: " + QString::number(stats->getHealth()) + "\n" +
                         "Hygiene: " + QString::number(stats->getHygiene()) + "\n" +
                         "Hyper: " + QString::number(stats->getHyper()) + "\n" +
                         "Independence: " + QString::number(stats->getIndependence()) + "\n");
  }
}

int Boris::getWidth() const
{
  return boundingRect().width();
}

int Boris::getHeight() const
{
  return boundingRect().height();
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
    setWeather(behav);
  }

  if(behav == "" && QRandomGenerator::global()->bounded(2)) {
    // Stat check
    QList<QString> potentials;
    if(stats->getFun() <= 50) {
      // Clamping here to avoid value being 0 which throws an assert
      if(QRandomGenerator::global()->bounded(std::clamp(stats->getFun(), 1, 100)) > stats->getIndependence()) {
        potentials.append("_fun");
      } else {
        potentials.append(getFileFromCategory("Fun"));
      }
    }
    if(stats->getEnergy() <= 50) {
      if(QRandomGenerator::global()->bounded(100) > stats->getIndependence()) {
        potentials.append("_energy");
      } else {
        potentials.append(getFileFromCategory("Energy"));
      }
    }
    if(stats->getSocial() <= 50) {
      if(QRandomGenerator::global()->bounded(100) > stats->getIndependence()) {
        potentials.append("_social");
      } else {
        potentials.append(getFileFromCategory("Social"));
      }
    }
    if(stats->getHygiene() <= 50) {
      if(QRandomGenerator::global()->bounded(100) > stats->getIndependence()) {
        potentials.append("_hygiene");
      } else {
        potentials.append(getFileFromCategory("Hygiene"));
      }
    }
    if(stats->getHealth() <= 50) {
      if(QRandomGenerator::global()->bounded(150 - stats->getHealth()) < stats->getIndependence()) {
        potentials.append("_health");
      }
    }
    // This one is 'inverted' compared to the others. High 'hunger' means Boris is starving.
    if(stats->getHunger() >= 50) {
      if(QRandomGenerator::global()->bounded(100) > stats->getIndependence()) {
        potentials.append("_hunger");
      } else {
        potentials.append(getFileFromCategory("Hunger"));
      }
    }
    // This one is 'inverted' compared to the others. High 'toilet' means Boris has to go to the bathroom.
    if(stats->getToilet() >= 50) {
      if(QRandomGenerator::global()->bounded(100) > stats->getIndependence()) {
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

void Boris::reconfBoris()
{
  // Reset Boris pointer
  borisFriend = nullptr;

  // Set new independence level
  stats->setIndependence(settings.independence);

  // Show or hide stats
  if(settings.stats == STATS_ALWAYS) {
    stats->show();
  } else {
    stats->hide();
  }
}

void Boris::triggerWeather()
{
  // Force showing weather next time processAi is run
  timeForWeather = 666;
}

void Boris::setWeather(QString &behav)
{
  weather->resetWeather();

  weather->show();
  QTimer::singleShot(30000, [this]() { weather->hide(); });

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
  // Check if there are other Borises nearby to interact with
  for(auto &boris: settings.borisList) {
    if(boris != this) {
      if(getDistance(boris->getGlobalCenter()) < (getWidth() * 2)) {
        borisInteract(boris);
        break;
      }
    }
  }

  // Check if user is dragging any items close by
  for(auto &item: settings.itemList) {
    if(interact && !falling && !grabbed && !settings.behaviours.at(curBehav).doNotDisturb &&
       (!item->ignore || item->grabbed) &&
       getDistance(item->getGlobalCenter()) < (getWidth() * 0.75)) {
      itemInteract(item);
      break;
    }
  }

  if((settings.stats == STATS_MOUSEOVER || settings.stats == STATS_CRITICAL) &&
     stats->underMouse) {
    stats->show();
  }
  // This balances the interaction count which is increased when mouse is moved across Boris
  interactions -= 1;
  interactions = std::clamp(interactions, 0, 10);

  annoyance -= 2;
  annoyance = std::clamp(annoyance, 0, ANNOYMAX);

  // Update debug info in idkfa menu here as well if it's enabled
  if(settings.idkfa) {
    updateDebugInfo();
  }

  interactionsTimer.start();
}

void Boris::borisInteract(Boris *otherBoris)
{
  if(borisFriend != nullptr || falling || grabbed || settings.behaviours.at(curBehav).doNotDisturb) {
    return;
  }
  borisFriend = otherBoris;
  // Queue current behaviour so it isn't 'forgotten'
  behavQueue.prepend(settings.behaviours.at(curBehav).file);

  if(settings.behaviours.at(borisFriend->getCurBehav()).file == "_drop_dead") {
    changeBehaviour("_sad");
  } else if(borisFriend->stats->getHygiene() <= 22) {
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
    bubble->initBubble(stats->getHyper(), "I don't know what to do with that.", "_thought");
  }
  enableInteractTimer.setInterval(5000);
  enableInteractTimer.start();
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
