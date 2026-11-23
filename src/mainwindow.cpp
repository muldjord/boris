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

#include "about.h"
#include "boris.h"
#include "item.h"
#include "loader.h"
#include "mainwindow.h"
#include "netcomm.h"
#include "soundmixer.h"

#include <QApplication>
#include <QColor>
#include <QCursor>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QGraphicsScene>
#include <QIcon>
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QRandomGenerator>
#include <QSettings>
#include <QSplashScreen>
#include <QString>
#include <QSystemTrayIcon>

MainWindow *mainWindow = nullptr;
SoundMixer *soundMixer = nullptr;

MainWindow::MainWindow(QSplashScreen &splash)
{
  mainWindow = this;

  soundMixer = new SoundMixer();

  loadFromConfig();

  setScene(new QGraphicsScene());

  scene()->setBackgroundBrush(Qt::transparent);
  setStyleSheet("background:transparent");
  setAttribute(Qt::WA_TranslucentBackground);
  // StaysOnTop doesn't work under Wayland but it might work under other conditions so I am leaving it on
  setWindowFlags(Qt::FramelessWindowHint |
                 Qt::WindowStaysOnTopHint);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  showMaximized();

  // Make sure 0,0 is at top-left of view
  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  setTransformationAnchor(QGraphicsView::NoAnchor);
  setResizeAnchor(QGraphicsView::NoAnchor);

  setFrameShape(QFrame::NoFrame);
  setContentsMargins(0, 0, 0, 0);

  // Load all graphics before loading rest of assets, as these are used as icons too
  if(Loader::loadImages(settings.graphicsPath, settings.graphics)) {
    qInfo("Graphics loaded ok... :)\n");
  } else {
    qInfo("Error when loading graphics...\n");
  }

  qApp->processEvents();

  int soundsSize = Loader::getAssetsSize(QDir(settings.soundsPath, "*.wav",
                                             QDir::Name, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable)) / 1024;
  int behavsSize = Loader::getAssetsSize(QDir(settings.behavioursPath, "*.png",
                                             QDir::Name, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable)) / 1024;
  int weathersSize = Loader::getAssetsSize(QDir(settings.weathersPath, "*.png",
                                               QDir::Name, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable)) / 1024;
  int itemsSize = Loader::getAssetsSize(QDir(settings.itemsPath, "*.png",
                                             QDir::Name, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable)) / 1024;

  splash.showMessage("Loading " + QString::number(soundsSize) + " kilobytes of sounds, please wait...", Qt::AlignBottom, QColor(78, 92, 106));
  qApp->processEvents();
  if(Loader::loadSoundFxs(settings.soundsPath, soundMixer->soundFxs, splash)) {
    qInfo("Sounds loaded ok... :)\n");
  } else {
    qInfo("Error when loading some sounds, please check your wav files\n");
  }

  splash.showMessage("Loading " + QString::number(behavsSize) + " kilobytes of behaviours, please wait...", Qt::AlignBottom, QColor(78, 92, 106));
  qApp->processEvents();
  if(Loader::loadBehaviours(settings, settings.behavioursPath, settings.behaviours, soundMixer->soundFxs, splash)) {
    qInfo("Behaviours loaded ok... :)\n");
  } else {
    qInfo("Error when loading some behaviours, please check your png and brs files\n");
  }

  splash.showMessage("Loading " + QString::number(weathersSize) + " kilobytes of weather types, please wait...", Qt::AlignBottom, QColor(78, 92, 106));
  qApp->processEvents();
  if(Loader::loadBehaviours(settings, settings.weathersPath, settings.weathers, soundMixer->soundFxs, splash)) {
    qInfo("Weather types loaded ok... :)\n");
  } else {
    qInfo("Error when loading some weather types, please check your png and brs files\n");
  }

  splash.showMessage("Loading " + QString::number(itemsSize) + " kilobytes of interaction items, please wait...", Qt::AlignBottom, QColor(78, 92, 106));
  qApp->processEvents();
  if(Loader::loadBehaviours(settings, settings.itemsPath, settings.itemBehaviours, soundMixer->soundFxs, splash)) {
    qInfo("Items loaded ok... :)\n");
  } else {
    qInfo("Error when loading some items, please check your png and brs files\n");
  }

  splash.showMessage("Loading cursors, please wait...", Qt::AlignBottom, QColor(78, 92, 106));
  qApp->processEvents();
  if(Loader::loadImages(settings.cursorsPath, settings.cursors)) {
    qInfo("Cursors loaded ok... :)\n");
  } else {
    qInfo("Error when loading cursors...\n");
  }

  splash.showMessage("Loading additional sprites, please wait...", Qt::AlignBottom, QColor(78, 92, 106));
  qApp->processEvents();
  if(Loader::loadSprites(settings)) {
    qInfo("Sprites loaded ok... :)\n");
  } else {
    qInfo("Error when loading sprites...\n");
  }

  splash.showMessage("Loading font, please wait...", Qt::AlignBottom, QColor(78, 92, 106));
  qApp->processEvents();
  if(Loader::loadFont(settings)) {
    qInfo("Font loaded ok... :)\n");
  } else {
    qInfo("Error when loading font...\n");
  }

  createActions();

  createItemsMenu();
  createBehavioursMenu();

  createTrayIcon();

  show();

  netComm = new NetComm(settings);
  connect(netComm, &NetComm::weatherUpdated, this, &MainWindow::updateTrayWeather);

  elapsedTimer.start();
  QTimer::singleShot(0, this, &MainWindow::init);
}

MainWindow::~MainWindow()
{
}

void MainWindow::init()
{
  if(settings.showWelcome) {
    About about(iniSettings, settings);
    about.exec();
    resetView();
  }

  addBoris((settings.clones == 0?QRandomGenerator::global()->bounded(99) + 1:settings.clones));

  tickTimer.setInterval(15);
  tickTimer.setSingleShot(false);
  connect(&tickTimer, &QTimer::timeout, this, &MainWindow::tick);
  tickTimer.start();

  itemSpawnTimeMax = settings.itemSpawnInterval * 1000;
  itemSpawnTime = 0;

  updateTrayWeather(); // Set initial state in case no weather is configured
  netComm->updateAll();
}

void MainWindow::tick()
{
  // These are used in BorisScript as 'kdir' to allow keyboard control of Boris behaviours (eg. "_walk")
  keyDirection = -1;
  if(pressedKeys[0] == true && pressedKeys[2] == false && pressedKeys[4] == false && pressedKeys[6] == false) {
    keyDirection = 0;
  }
  if(pressedKeys[0] == true && pressedKeys[2] == true && pressedKeys[4] == false && pressedKeys[6] == false) {
    keyDirection = 1;
  }
  if(pressedKeys[0] == false && pressedKeys[2] == true && pressedKeys[4] == false && pressedKeys[6] == false) {
    keyDirection = 2;
  }
  if(pressedKeys[0] == false && pressedKeys[2] == true && pressedKeys[4] == true && pressedKeys[6] == false) {
    keyDirection = 3;
  }
  if(pressedKeys[0] == false && pressedKeys[2] == false && pressedKeys[4] == true && pressedKeys[6] == false) {
    keyDirection = 4;
  }
  if(pressedKeys[0] == false && pressedKeys[2] == false && pressedKeys[4] == true && pressedKeys[6] == true) {
    keyDirection = 5;
  }
  if(pressedKeys[0] == false && pressedKeys[2] == false && pressedKeys[4] == false && pressedKeys[6] == true) {
    keyDirection = 6;
  }
  if(pressedKeys[0] == true && pressedKeys[2] == false && pressedKeys[4] == false && pressedKeys[6] == true) {
    keyDirection = 7;
  }
  
  qint64 elapsed = getElapsed() - oldElapsed;

  physicsTime += elapsed;
  itemSpawnTime += elapsed;

  if(physicsTimer) {
    if(physicsTime > physicsTimeMax) {
      physicsTime = physicsTime - physicsTimeMax;
      if(physicsTime > physicsTimeMax) {
	physicsTime = 0;
      }
      handlePhysics();
    }
  }

  if(itemSpawnTimer) {
    if(itemSpawnTime > itemSpawnTimeMax) {
      itemSpawnTime = itemSpawnTime - itemSpawnTimeMax;
      if(itemSpawnTime > itemSpawnTimeMax) {
	itemSpawnTime = 0;
      }
      spawnRandomItem();
    }
  }

  oldElapsed = getElapsed();
}

qint64 MainWindow::getElapsed()
{
  return elapsedTimer.elapsed();
}

void MainWindow::loadFromConfig()
{
  iniSettings = new QSettings("config.ini", QSettings::IniFormat);

  if(!iniSettings->contains("show_welcome")) {
    iniSettings->setValue("show_welcome", true);
  }
  settings.showWelcome = iniSettings->value("show_welcome").toBool();

  if(!iniSettings->contains("stat_logging")) {
    iniSettings->setValue("stat_logging", false);
  }
  settings.statLogging = iniSettings->value("stat_logging").toBool();

  if(!iniSettings->contains("script_output")) {
    iniSettings->setValue("script_output", false);
  }
  settings.scriptOutput = iniSettings->value("script_output").toBool();

  if(!iniSettings->contains("data_path")) {
    iniSettings->setValue("data_path", "data");
  }
  QString dataPath = iniSettings->value("data_path").toString();

  settings.behavioursPath = dataPath + "/behaviours";
  settings.itemsPath = dataPath + "/items";
  settings.soundsPath = dataPath + "/sounds";
  settings.spritesPath = dataPath + "/sprites";
  settings.weathersPath = dataPath + "/weathers";
  settings.cursorsPath = dataPath + "/cursors";

  if(!iniSettings->contains("unlocked")) {
    iniSettings->setValue("unlocked", "balloon");
  }
  settings.unlocked = iniSettings->value("unlocked").toString().split(",", Qt::SkipEmptyParts);

  // Force weather from ini
  if(iniSettings->contains("weather_force_type")) {
    settings.weatherType = iniSettings->value("weather_force_type").toString();
    settings.forceWeatherType = true;
  }
  if(iniSettings->contains("weather_force_temp")) {
    settings.temperature = iniSettings->value("weather_force_temp").toDouble();
    settings.forceTemperature = true;
  }
  if(iniSettings->contains("weather_force_wind_direction")) {
    settings.windDirection = iniSettings->value("weather_force_wind_direction").toString();
    settings.forceWindDirection = true;
  }
  if(iniSettings->contains("weather_force_wind_speed")) {
    settings.windSpeed = iniSettings->value("weather_force_wind_speed").toDouble();
    settings.forceWindSpeed = true;
  }

  if(!iniSettings->contains("coins")) {
    iniSettings->setValue("coins", 100);
  }
  settings.coins = iniSettings->value("coins").toInt();

  if(!iniSettings->contains("clones")) {
    iniSettings->setValue("clones", 2);
  }
  settings.clones = iniSettings->value("clones").toInt();

  if(!iniSettings->contains("size")) {
    iniSettings->setValue("size", 64);
  }
  settings.size = iniSettings->value("size").toInt();
  settings.sizeFactor = settings.size / 32.0;

  if(!iniSettings->contains("independence")) {
    iniSettings->setValue("independence", 75);
  }
  settings.independence = iniSettings->value("independence").toInt();

  if(!iniSettings->contains("items")) {
    iniSettings->setValue("items", true);
  }
  settings.items = iniSettings->value("items").toBool();

  if(!iniSettings->contains("item_spawn_timer")) {
    iniSettings->setValue("item_spawn_timer", 120);
  }
  settings.itemSpawnInterval = iniSettings->value("item_spawn_timer").toInt();

  if(!iniSettings->contains("item_timeout")) {
    iniSettings->setValue("item_timeout", 480);
  }
  settings.itemTimeout = iniSettings->value("item_timeout").toInt();

  if(!iniSettings->contains("idkfa")) {
    iniSettings->setValue("idkfa", false);
  }
  settings.idkfa = iniSettings->value("idkfa").toBool();

  if(!iniSettings->contains("stats")) {
    iniSettings->setValue("stats", "critical");
  }
  if(iniSettings->value("stats").toString() == "always") {
    settings.stats = STATS_ALWAYS;
  } else if(iniSettings->value("stats").toString() == "critical") {
    settings.stats = STATS_CRITICAL;
  } else if(iniSettings->value("stats").toString() == "mouseover") {
    settings.stats = STATS_MOUSEOVER;
  } else if(iniSettings->value("stats").toString() == "never") {
    settings.stats = STATS_NEVER;
  }

  if(!iniSettings->contains("bubbles")) {
    iniSettings->setValue("bubbles", true);
  }
  settings.bubbles = iniSettings->value("bubbles").toBool();

  if(!iniSettings->contains("sound")) {
    iniSettings->setValue("sound", true);
  }
  settings.sound = iniSettings->value("sound").toBool();

  if(!iniSettings->contains("make_invincible")) {
    iniSettings->setValue("make_invincible", false);
  }
  settings.isInvincible = iniSettings->value("make_invincible").toBool();

  if(!iniSettings->contains("volume")) {
    iniSettings->setValue("volume", 50);
  }
  settings.volume = iniSettings->value("volume").toInt();
  soundMixer->setGlobalVolume(settings.volume);

  if(!iniSettings->contains("lemmy_mode")) {
    iniSettings->setValue("lemmy_mode", false);
  }
  settings.lemmyMode = iniSettings->value("lemmy_mode").toBool();

  if(!iniSettings->contains("feed_url")) {
    iniSettings->setValue("feed_url", "http://rss.slashdot.org/Slashdot/slashdotMain");
  }
  settings.feedUrl = iniSettings->value("feed_url").toString();

  if(!iniSettings->contains("weather_city")) {
    iniSettings->setValue("weather_city", "Copenhagen");
  }
  settings.city = iniSettings->value("weather_city").toString();

  if(!iniSettings->contains("weather_key")) {
    iniSettings->setValue("weather_key", "fe9fe6cf47c03d2640d5063fbfa053a2");
  }
  settings.key = iniSettings->value("weather_key").toString();
}

void MainWindow::resetView()
{
  resetTransform();
  scale(settings.sizeFactor, settings.sizeFactor);
  scene()->setSceneRect(0, 0,
                        viewport()->width() / transform().m11(),
                        viewport()->height() / transform().m22());
  int newWidth = scene()->sceneRect().width();
  int newHeight = scene()->sceneRect().height();
  if(settings.desktopWidth != newWidth ||
     settings.desktopHeight != newHeight) {
    settings.desktopWidth = newWidth;
    settings.desktopHeight = newHeight;
    // Move all Borises randomly around the screen to avoid some Borises being at the edge
    for(auto *boris: settings.borisList) {
      boris->setPos(QRandomGenerator::global()->bounded(settings.desktopWidth), QRandomGenerator::global()->bounded(settings.desktopHeight));
    }
  }
}

void MainWindow::addBoris(int clones)
{
  qInfo("Spawning %d clone(s)\n", clones);
  while(clones--) {
    Boris *newBoris = new Boris(settings);
    connect(&tickTimer, &QTimer::timeout, newBoris, &Boris::tick);
    connect(this, &MainWindow::reconfBoris, newBoris, &Boris::reconfBoris);
    connect(earthquakeAction, &QAction::triggered, newBoris, &Boris::earthquake);
    connect(this, &MainWindow::queueBehavFromFile, newBoris, &Boris::queueBehavFromFile);
    connect(weatherAction, &QAction::triggered, newBoris, &Boris::triggerWeather);
    connect(newBoris, &Boris::deltaCoins, this, &MainWindow::deltaCoins);
    scene()->addItem(newBoris);
    settings.borisList.append(newBoris);
  }
  earthquakeAction->trigger();
}

void MainWindow::removeBoris(int clones)
{
  // Reset all Boris collide pointers within all existing clones to prevent crash
  for(const auto &boris: settings.borisList) {
    boris->borisFriends.clear();
  }
  while(clones--) {
    settings.borisList.takeLast()->deleteLater();
  }
}

void MainWindow::createActions()
{
  earthquakeAction = new QAction("&Earthquake!!!", this);
  earthquakeAction->setIcon(QIcon(settings.getPixmap("earthquake.png")));

  weatherAction = new QAction("No weather set...", this);

  coinsAction = new QAction(QString::number(settings.coins) + " coins", this);
  coinsAction->setIcon(QIcon(settings.getPixmap("coin.png")));
}

void MainWindow::createTrayIcon()
{
  trayIcon = new QSystemTrayIcon(this);

  trayIconMenu = new QMenu();
  trayIconMenu->addAction(QIcon(settings.getPixmap("about.png")), "&Config / about...", this, &MainWindow::aboutBox);
  trayIconMenu->addSeparator();
  trayIconMenu->addMenu(behavioursMenu);
  trayIconMenu->addSeparator();
  trayIconMenu->addMenu(itemsMenu);
  trayIconMenu->addAction(coinsAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(earthquakeAction);
  trayIconMenu->addAction(weatherAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(QIcon(settings.getPixmap("quit.png")), "&Quit", this, &MainWindow::killAll);

  coinIcon = new QIcon(settings.getPixmap("coin.png"));

  QImage iconImage(":icon.png");
  Loader::setClothesColor(settings, iconImage);
  icon = new QIcon(QPixmap::fromImage(iconImage));
  trayIcon->setIcon(*icon);
  trayIcon->setToolTip("Boris");
  trayIcon->show();
  trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::aboutBox()
{
  About about(iniSettings, settings);
  about.exec();
  emit reconfBoris();
  resetView();

  int newClones = settings.clones;
  if(newClones == 0) {
    newClones = QRandomGenerator::global()->bounded(19) + 1;
  }
  if(settings.borisList.count() > newClones) {
    removeBoris(settings.borisList.count() - newClones);
  } else if(settings.borisList.count() < newClones) {
    addBoris(newClones - settings.borisList.count());
  }

  updateTrayWeather(); // Set initial state in case no weather is configured
  netComm->updateAll();

  itemSpawnTimeMax = settings.itemSpawnInterval * 1000;
  itemSpawnTime = 0;
}

void MainWindow::handlePhysics()
{
  settings.mouseHVel = (QCursor::pos().x() - oldCursor.x()) * 0.25; // 0.25 is just an abstract factor that relates well to the 30 fps physics
  settings.mouseVVel = (QCursor::pos().y() - oldCursor.y()) * 0.25;
  oldCursor = QCursor::pos();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
  resetView();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Up) {
    pressedKeys[0] = true;
  }
  if(event->key() == Qt::Key_Right) {
    pressedKeys[2] = true;
  }
  if(event->key() == Qt::Key_Down) {
    pressedKeys[4] = true;
  }
  if(event->key() == Qt::Key_Left) {
    pressedKeys[6] = true;
  }
  event->accept();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Up) {
    pressedKeys[0] = false;
  }
  if(event->key() == Qt::Key_Right) {
    pressedKeys[2] = false;
  }
  if(event->key() == Qt::Key_Down) {
    pressedKeys[4] = false;
  }
  if(event->key() == Qt::Key_Left) {
    pressedKeys[6] = false;
  }
  event->accept();
}

void MainWindow::killAll()
{
  QTimer::singleShot(2000, qApp, &QApplication::quit);
  // Reset all Boris collide pointers within all existing clones to prevent crash
  for(const auto &boris: settings.borisList) {
    boris->borisFriends.clear();
  }
  for(auto &boris: settings.borisList) {
    boris->changeBehaviour("_wave");
  }
}

void MainWindow::updateTrayWeather()
{
  weatherAction->setText(QString::number(settings.temperature) + "°C, " + QString::number(settings.windSpeed) + "m/s " + settings.windDirection);
  weatherAction->setIcon(QIcon(settings.getPixmap(settings.weatherType + ".png")));
}

void MainWindow::createItemsMenu()
{
  itemsMenu = new QMenu();
  itemsMenu->setTitle("Items");
  connect(itemsMenu, &QMenu::triggered, this, &MainWindow::spawnItem);

  QMenu *idkfaMenu = new QMenu("Idkfa", itemsMenu);
  idkfaMenu->setIcon(QIcon(settings.getPixmap("idkfa.png")));

  QList<QMenu*> subMenus;

  for(const auto &item: settings.itemBehaviours) {
    QString title = item.title;
    QPixmap iconPixmap(item.category.isEmpty()?settings.getPixmap("other.png"):settings.getPixmap(item.category.toLower() + ".png"));
    QMenu *menu = nullptr;

    // Find correct menu to put item into

    // Item that doesn't belong anywhere
    if((item.file.left(1) == "_" && (item.category.isEmpty() || item.hidden)) ||
       item.category.isEmpty() ||
       item.hidden) {
      menu = idkfaMenu;
    }

    // Item with an ordinary category such as "Fun", Hunger", "Social" and so on
    for(auto &subMenu: subMenus) {
      if(subMenu->title() == item.category && !item.hidden) {
        menu = subMenu;
        break;
      }
    }

    // If no menu exists for this item category create it
    if(menu == nullptr) {
      menu = new QMenu(item.category, itemsMenu);
      menu->setIcon(QIcon(iconPixmap));
      subMenus.append(menu);
    }

    // Append coin price to title if it's not already unlocked
    if(!settings.unlocked.contains(item.file)) {
      title.append(" (" + QString::number(item.coins) + "c)");
      iconPixmap = settings.getPixmap("coin.png");
    }

    QAction *itemAction = new QAction(QIcon(iconPixmap), title);
    itemAction->setObjectName(item.file);
    itemAction->setData(item.category.toLower());

    menu->addAction(itemAction);
  }

  for(auto &subMenu: subMenus) {
    if(!subMenu->isEmpty()) {
      itemsMenu->addMenu(subMenu);
    }
  }

  if(settings.idkfa && !idkfaMenu->isEmpty()) {
    itemsMenu->addMenu(idkfaMenu);
  }

  if(itemsMenu->isEmpty()) {
    itemsMenu->addAction(QIcon(settings.getPixmap("idkfa.png")), "No behaviours!");
  }
}

void MainWindow::createBehavioursMenu()
{
  behavioursMenu = new QMenu();
  behavioursMenu->setTitle("Behaviours");
  connect(behavioursMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);

  QList<QMenu*> subMenus;

  QMenu *idkfaMenu = new QMenu("Idkfa", behavioursMenu);
  idkfaMenu->setIcon(QIcon(settings.getPixmap("idkfa.png")));

  for(const auto &behaviour: settings.behaviours) {
    QString title = behaviour.title;
    QPixmap iconPixmap(behaviour.category.isEmpty()?settings.getPixmap("other.png"):settings.getPixmap(behaviour.category.toLower() + ".png"));
    QMenu *menu = nullptr;

    // Behaviour that don't belong anywhere
    if((behaviour.file.left(1) == "_" && (behaviour.category.isEmpty() || behaviour.hidden)) ||
       behaviour.category.isEmpty() ||
       behaviour.hidden ||
       behaviour.category == "Locomotion") {
      menu = idkfaMenu;
    }

    // Behaviour with an ordinary category such as "Hunger", "Social" and so on
    if(menu == nullptr) {
      for(auto &subMenu: subMenus) {
        if(subMenu->title() == behaviour.category) {
          menu = subMenu;
          break;
        }
      }
    }

    // If no menu exists for this behaviour category create it
    if(menu == nullptr) {
      menu = new QMenu(behaviour.category, behavioursMenu);
      menu->setIcon(QIcon(iconPixmap));
      subMenus.append(menu);
    }

    QAction *behavAction = new QAction(QIcon(iconPixmap), title);
    behavAction->setObjectName(behaviour.file);

    menu->addAction(behavAction);
  }

  for(auto &subMenu: subMenus) {
    if(!subMenu->isEmpty()) {
      behavioursMenu->addMenu(subMenu);
    }
  }

  if(settings.idkfa && !idkfaMenu->isEmpty()) {
    behavioursMenu->addMenu(idkfaMenu);
  }

  if(behavioursMenu->isEmpty()) {
    behavioursMenu->addAction(QIcon(settings.getPixmap("idkfa.png")), "No behaviours!");
  }
}

void MainWindow::triggerBehaviour(QAction* action)
{
  QString behaviourFile = action->objectName();
  for(const auto &behaviour: settings.behaviours) {
    if(behaviour.file == behaviourFile) {
      emit queueBehavFromFile(behaviour.file);
      break;
    }
  }
}

void MainWindow::spawnRandomItem()
{
  if(!settings.items) {
    return;
  }

  int randomItem = 0;
  do {
    randomItem = QRandomGenerator::global()->bounded(settings.itemBehaviours.count());
  } while(settings.itemBehaviours.at(randomItem).reactions.isEmpty() ||
	  settings.itemBehaviours.at(randomItem).hidden);

  // Spawn in a random location with a span of settings.baseSize so it doesn't spawn right at the edges
  Item *spawnedItem = new Item(settings.itemBehaviours.at(randomItem).file, settings,
			       QRandomGenerator::global()->bounded(settings.desktopWidth - (settings.baseSize * 3)) + settings.baseSize,
			       QRandomGenerator::global()->bounded(settings.desktopHeight - (settings.baseSize * 3)) + settings.baseSize);
  connect(&tickTimer, &QTimer::timeout, spawnedItem, &Item::tick);
  scene()->addItem(spawnedItem);
  settings.itemList.append(spawnedItem);
}

void MainWindow::spawnItem(QAction* action)
{
  if(!settings.items) {
    QMessageBox::information(nullptr, "Items disabled!", "Items are disabled! Please enable them in the configuration in order to spawn them.");
    return;
  }

  QString itemFile = action->objectName();
  for(const auto &item: settings.itemBehaviours) {
    if(item.file == itemFile) {
      if(settings.unlocked.contains(item.file)) {
	// Spawn in a random location with a span of settings.baseSize so it doesn't spawn right at the edges
        Item *spawnedItem = new Item(item.file, settings,
				     QRandomGenerator::global()->bounded(settings.desktopWidth - (settings.baseSize * 3)) + settings.baseSize,
				     QRandomGenerator::global()->bounded(settings.desktopHeight - (settings.baseSize * 3)) + settings.baseSize);
	connect(&tickTimer, &QTimer::timeout, spawnedItem, &Item::tick);
        scene()->addItem(spawnedItem);
        settings.itemList.append(spawnedItem);
      } else {
        if(settings.coins - item.coins >= 0) {
          if(QMessageBox::question(nullptr, "Buy?", "Do you want to buy '" + item.title + "' " + "for " + QString::number(item.coins) + " coins?") == QMessageBox::Yes) {
            deltaCoins(- item.coins);
            unlockItem(action);
          }
        } else {
	  if(settings.sound) {
	    soundMixer->playSoundFile("nocoin.wav", 0, 1);
	  }
          QMessageBox::information(nullptr, "Not enough coins!", "You can't afford the '" + item.title + "' " + "item.\n\nInitiate behaviours that corresponds to Boris' needs to earn coins. You can then use those coins to unlock new items.");
        }
      }
      break;
    }
  }
}

void MainWindow::unlockItem(QAction *action)
{
  settings.unlocked.append(action->objectName());
  action->setIcon(QIcon(settings.getPixmap(action->data().toString().toLower() + ".png")));

  QString newUnlocked = "";
  for(const auto &unlock: settings.unlocked) {
    newUnlocked.append(unlock + ",");
  }
  newUnlocked = newUnlocked.left(newUnlocked.length() - 1);
  iniSettings->setValue("unlocked", newUnlocked);
}

void MainWindow::deltaCoins(const int &coins)
{
  if(settings.sound) {
    soundMixer->playSoundFile("coinup.wav", 0, 1);
  }
  settings.coins += coins;
  coinsAction->setText(QString::number(settings.coins) + " " + "coins" + " (" + QString::number(coins) + ")");
  trayIcon->setIcon(*coinIcon);
  QTimer::singleShot(10000, this, [this]() { coinsAction->setText(QString::number(settings.coins) + " coins"); trayIcon->setIcon(*icon); });
  iniSettings->setValue("coins", settings.coins);
}

int MainWindow::getKeyDirection()
{
  return keyDirection;
}
