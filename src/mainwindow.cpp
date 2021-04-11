/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.cpp
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

#include "mainwindow.h"
#include "about.h"
#include "loader.h"
#include "soundmixer.h"
#include "item.h"

#include "SFML/Audio.hpp"

#include <stdio.h>
#include <math.h>
#include <QApplication>
#include <QTime>
#include <QDir>
#include <QTextStream>
#include <QDesktopWidget>
#include <QHostInfo>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>

#if QT_VERSION >= 0x050a00
#include <QRandomGenerator>
#endif

QList<Item*> itemList;
QList<Behaviour> itemBehaviours;

QList<Boris*> borisList;
QList<Behaviour> behaviours;

QList<Behaviour> weathers;
QMap<QString, Sprite> sprites;
QMap<QString, QImage> pixelFont;
SoundMixer soundMixer(24);

MainWindow::MainWindow()
{
#if QT_VERSION < 0x050a00
  qsrand(QTime::currentTime().msec());
#endif

  QString iniFile = "config.ini";

  iniSettings = new QSettings(iniFile, QSettings::IniFormat);
  
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
  
  if(!iniSettings->contains("sounds_path")) {
    iniSettings->setValue("sounds_path", "data/sfx");
  }
  settings.soundsPath = iniSettings->value("sounds_path").toString();

  if(!iniSettings->contains("behavs_path")) {
    iniSettings->setValue("behavs_path", "data/behavs");
  }
  settings.behavsPath = iniSettings->value("behavs_path").toString();
  
  if(!iniSettings->contains("weathers_path")) {
    iniSettings->setValue("weathers_path", "data/weathers");
  }
  settings.weathersPath = iniSettings->value("weathers_path").toString();

  if(!iniSettings->contains("items_path")) {
    iniSettings->setValue("items_path", "data/items");
  }
  settings.itemsPath = iniSettings->value("items_path").toString();

  if(!iniSettings->contains("sprites_path")) {
    iniSettings->setValue("sprites_path", "data/sprites");
  }
  settings.spritesPath = iniSettings->value("sprites_path").toString();

  if(!iniSettings->contains("unlocked")) {
    iniSettings->setValue("unlocked", "whistle");
  }
  settings.unlocked = iniSettings->value("unlocked").toString().split(",");

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
    iniSettings->setValue("coins", coins);
  }
  settings.coins = iniSettings->value("coins").toInt();

  if(!iniSettings->contains("boris_x")) {
    iniSettings->setValue("boris_x", QApplication::desktop()->width() / 2);
  }
  settings.borisX = iniSettings->value("boris_x").toInt();

  if(!iniSettings->contains("boris_y")) {
    iniSettings->setValue("boris_y", QApplication::desktop()->height() / 2);
  }
  settings.borisY = iniSettings->value("boris_y").toInt();

  // For use with sound panning in relation to Boris location
  settings.desktopWidth = QApplication::desktop()->width();

  if(!iniSettings->contains("vision")) {
    iniSettings->setValue("vision", false);
  }
  settings.vision = iniSettings->value("vision").toBool();

  if(!iniSettings->contains("clones")) {
    iniSettings->setValue("clones", 2);
  }
  settings.clones = iniSettings->value("clones").toInt();
    
  if(!iniSettings->contains("size")) {
    iniSettings->setValue("size", 64);
  }
  settings.size = iniSettings->value("size").toInt();

  if(!iniSettings->contains("independence")) {
    iniSettings->setValue("independence", 75);
  }
  settings.independence = iniSettings->value("independence").toInt();

  if(!iniSettings->contains("items")) {
    iniSettings->setValue("items", true);
  }
  settings.items = iniSettings->value("items").toBool();

  if(!iniSettings->contains("item_spawn_timer")) {
    iniSettings->setValue("item_spawn_timer", 240);
  }
  settings.itemSpawnInterval = iniSettings->value("item_spawn_timer").toInt();

  if(!iniSettings->contains("item_timeout")) {
    iniSettings->setValue("item_timeout", 900);
  }
  settings.itemTimeout = iniSettings->value("item_timeout").toInt();

  if(!iniSettings->contains("idkfa")) {
    iniSettings->setValue("idkfa", false);
  }
  settings.idkfa = iniSettings->value("idkfa").toBool();
  
  if(!iniSettings->contains("stats")) {
    iniSettings->setValue("stats", "mouseover");
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

  if(!iniSettings->contains("volume")) {
    iniSettings->setValue("volume", 50);
  }
  settings.volume = iniSettings->value("volume").toInt() / 100.0;
  sf::Listener::setGlobalVolume(settings.volume * 100.0);
  
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

  createActions();
  behavioursMenu = new QMenu();
  behavioursMenu->setTitle(tr("Behaviours"));
  connect(behavioursMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
  itemsMenu = new QMenu();
  itemsMenu->setTitle(tr("Items"));
  createTrayIcon();
  trayIcon->show();

  netComm = new NetComm(&settings);
  connect(netComm, &NetComm::weatherUpdated, this, &MainWindow::updateWeather);

  loadWidget = new QWidget;
  loadWidget->setWindowIcon(QIcon(":icon.png"));
  loadWidget->setWindowTitle("Boris v" VERSION);
  QLabel *progressLabel = new QLabel(tr("Looking for Boris, please wait...\n"));
  progressBar = new QProgressBar;
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(progressLabel);
  layout->addWidget(progressBar);

  loadWidget->setLayout(layout);
  loadWidget->show();
  qApp->processEvents();
  loadWidget->move(settings.borisX - (loadWidget->width() / 2) + (settings.size / 2),
                   settings.borisY);

  itemTimer.setInterval(settings.itemSpawnInterval * 1000);
  itemTimer.setSingleShot(true);
  connect(&itemTimer, &QTimer::timeout, this, &MainWindow::spawnRandomItem);
  if(settings.items && settings.itemSpawnInterval) {
    itemTimer.start();
  }

  QTimer::singleShot(100, this, &MainWindow::loadAssets);
}

MainWindow::~MainWindow()
{
  iniSettings->setValue("boris_x", settings.borisX);
  iniSettings->setValue("boris_y", settings.borisY);
  delete trayIcon;
  for(auto &boris: borisList) {
    delete boris;
  }
}

void MainWindow::loadAssets()
{
  int totalAssetsSize = 0;

  totalAssetsSize += Loader::getAssetsSize(QDir(settings.soundsPath, "*.wav", QDir::Name,
                                                      QDir::Files | QDir::NoDotAndDotDot | QDir::Readable));
  
  totalAssetsSize += Loader::getAssetsSize(QDir(settings.behavsPath, "*.png", QDir::Name, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable));

  totalAssetsSize += Loader::getAssetsSize(QDir(settings.weathersPath, "*.png", QDir::Name, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable));

  totalAssetsSize += Loader::getAssetsSize(QDir(settings.itemsPath, "*.png", QDir::Name, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable));

  qInfo("Loading %d kilobytes of assets, please wait...\n", totalAssetsSize / 1024);

  progressBar->setMaximum(totalAssetsSize);
  
  if(Loader::loadSoundFxs(settings.soundsPath, soundMixer.soundFxs, progressBar)) {
    qInfo("Sounds loaded ok... :)\n");
  } else {
    qInfo("Error when loading some sounds, please check your wav files\n");
  }

  if(Loader::loadBehaviours(settings, settings.behavsPath, behaviours, soundMixer.soundFxs, progressBar)) {
    qInfo("Behaviours loaded ok... :)\n");
  } else {
    qInfo("Error when loading some behaviours, please check your png and dat files\n");
  }
  
  if(Loader::loadBehaviours(settings, settings.weathersPath, weathers, soundMixer.soundFxs, progressBar)) {
    qInfo("Weather types loaded ok... :)\n");
  } else {
    qInfo("Error when loading some weather types, please check your png and dat files\n");
  }

  if(Loader::loadBehaviours(settings, settings.itemsPath, itemBehaviours, soundMixer.soundFxs, progressBar)) {
    qInfo("Items loaded ok... :)\n");
  } else {
    qInfo("Error when loading some items, please check your png and dat files\n");
  }

  if(Loader::loadSprites(settings.spritesPath, sprites)) {
    qInfo("Sprites loaded ok... :)\n");
  } else {
    qInfo("Error when loading sprites...\n");
  }

  if(Loader::loadFont(pixelFont)) {
    qInfo("Font loaded ok... :)\n");
  } else {
    qInfo("Error when loading font...\n");
  }

  if(settings.showWelcome) {
    About about(&settings);
    about.exec();
  }

#if QT_VERSION >= 0x050a00
  addBoris((settings.clones == 0?QRandomGenerator::global()->generate() % 99 + 1:settings.clones));
#else
  addBoris((settings.clones == 0?qrand() % 99 + 1:settings.clones));
#endif
  progressBar->setValue(progressBar->maximum());
  
  delete progressBar;
  delete loadWidget;

  updateBehavioursMenu();
  createItemsMenu();
}

void MainWindow::addBoris(int clones)
{
  qInfo("Spawning %d clone(s)\n", clones);
  while(clones--) {
    borisList.append(new Boris(&settings));
    connect(this, &MainWindow::updateBoris, borisList.last(), &Boris::updateBoris);
    connect(this, &MainWindow::updateBorisBehavioursMenu, borisList.last(), &Boris::updateBehavioursMenu);
    connect(earthquakeAction, &QAction::triggered, borisList.last(), &Boris::earthquake);
    connect(this, &MainWindow::queueBehavFromFile, borisList.last(), &Boris::queueBehavFromFile);
    connect(weatherAction, &QAction::triggered, borisList.last(), &Boris::triggerWeather);
    connect(borisList.last(), &Boris::addCoins, this, &MainWindow::addCoins);
    borisList.last()->show();
    borisList.last()->earthquake();
  }
}

void MainWindow::removeBoris(int clones)
{
  // Reset all Boris collide pointers within all existing clones to prevent crash
  for(const auto &boris: borisList) {
    boris->borisFriend = nullptr;
  }
  while(clones--) {
    delete borisList.takeLast();
  }
}

void MainWindow::createActions()
{
  earthquakeAction = new QAction(tr("&Earthquake!!!"), this);
  earthquakeAction->setIcon(QIcon(":earthquake.png"));

  weatherAction = new QAction(tr("Updating weather..."), this);

  coinsAction = new QAction(QString::number(settings.coins) + tr(" coins"), this);
  coinsAction->setIcon(QIcon(":icon_coins.png"));
}

void MainWindow::createTrayIcon()
{
  trayIcon = new QSystemTrayIcon;

  trayIconMenu = new QMenu;
  trayIconMenu->addAction(QIcon(":icon_about.png"), tr("&Config / about..."), this, &MainWindow::aboutBox);
  trayIconMenu->addAction(earthquakeAction);
  trayIconMenu->addMenu(behavioursMenu);
  trayIconMenu->addMenu(itemsMenu);
  trayIconMenu->addAction(weatherAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(coinsAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(QIcon(":icon_quit.png"), tr("&Quit"), this, &MainWindow::killAll);

  coinIcon = new QIcon(QPixmap::fromImage(QImage(":icon_coins.png")));

  QImage iconImage(":icon.png");
  Loader::setClothesColor(settings, iconImage);
  icon = new QIcon(QPixmap::fromImage(iconImage));
  trayIcon->setIcon(*icon);
  trayIcon->setToolTip("Boris");
  trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::aboutBox()
{
  About about(&settings);
  about.exec();
  emit updateBoris();

  int newClones = settings.clones;
  if(newClones == 0) {
#if QT_VERSION >= 0x050a00
    newClones = (QRandomGenerator::global()->generate() % 99) + 1;
#else
    newClones = (qrand() % 99) + 1;
#endif
  }
  if(borisList.count() > newClones) {
    removeBoris(borisList.count() - newClones);
  } else if(borisList.count() < newClones) {
    addBoris(newClones - borisList.count());
  }
  
  netComm->updateAll();

  itemTimer.setInterval(settings.itemSpawnInterval * 1000);
  if(settings.items && settings.itemSpawnInterval) {
    itemTimer.start();
  } else {
    itemTimer.stop();
  }
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::RightButton) {
    trayIconMenu->exec(QCursor::pos());
  }
}

void MainWindow::killAll()
{
  QTimer::singleShot(2000, qApp, SLOT(quit()));
  for(auto &boris: borisList) {
    boris->changeBehaviour("wave");
  }
}

void MainWindow::updateWeather()
{
  if(settings.temperature != -42) {
    weatherAction->setText(QString::number(settings.temperature) + "°C, " + QString::number(settings.windSpeed) + "m/s " + settings.windDirection);
  } else {
    weatherAction->setText(tr("Couldn't find city"));
  }
  weatherAction->setIcon(QIcon(":" + settings.weatherType + ".png"));
}

void MainWindow::updateBehavioursMenu()
{
  behavioursMenu->clear();
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
  QMenu *idkfaMenu = new QMenu(tr("Idkfa"), behavioursMenu);
  idkfaMenu->setIcon(QIcon(":idkfa.png"));
  for(const auto &behaviour: behaviours) {
    if(settings.unlocked.contains(behaviour.file)) {
      if(behaviour.category == "Movement") {
        QAction *tempAction = movementMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Energy") {
        QAction *tempAction = energyMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Hunger") {
        QAction *tempAction = hungerMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Bladder") {
        QAction *tempAction = bladderMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Social") {
        QAction *tempAction = socialMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Fun") {
        QAction *tempAction = funMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Hygiene") {
        QAction *tempAction = hygieneMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Health") {
        QAction *tempAction = healthMenu->addAction(QIcon(":" + behaviour.category.toLower() + ".png"), behaviour.title);
        tempAction->setData(behaviour.file);
      }
    } else if(behaviour.file.left(1) != "_" || behaviour.category != "Hidden") {
      if(behaviour.category == "Movement") {
        QAction *tempAction = movementMenu->addAction(QIcon(*coinIcon), behaviour.title + " (10c)");
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Energy") {
        QAction *tempAction = energyMenu->addAction(QIcon(*coinIcon), behaviour.title + " (10c)");
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Hunger") {
        QAction *tempAction = hungerMenu->addAction(QIcon(*coinIcon), behaviour.title + " (10c)");
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Bladder") {
        QAction *tempAction = bladderMenu->addAction(QIcon(*coinIcon), behaviour.title + " (10c)");
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Social") {
        QAction *tempAction = socialMenu->addAction(QIcon(*coinIcon), behaviour.title + " (10c)");
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Fun") {
        QAction *tempAction = funMenu->addAction(QIcon(*coinIcon), behaviour.title + " (10c)");
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Hygiene") {
        QAction *tempAction = hygieneMenu->addAction(QIcon(*coinIcon), behaviour.title + " (10c)");
        tempAction->setData(behaviour.file);
      } else if(behaviour.category == "Health") {
        QAction *tempAction = healthMenu->addAction(QIcon(*coinIcon), behaviour.title + " (10c)");
        tempAction->setData(behaviour.file);
      }
    } else {
      QAction *tempAction = idkfaMenu->addAction(QIcon(":idkfa.png"), behaviour.title);
      tempAction->setData(behaviour.file);
    }
  }
  if(!healthMenu->isEmpty()) {
    behavioursMenu->addMenu(healthMenu);
  }
  if(!energyMenu->isEmpty()) {
    behavioursMenu->addMenu(energyMenu);
  }
  if(!hungerMenu->isEmpty()) {
    behavioursMenu->addMenu(hungerMenu);
  }
  if(!bladderMenu->isEmpty()) {
    behavioursMenu->addMenu(bladderMenu);
  }
  if(!hygieneMenu->isEmpty()) {
    behavioursMenu->addMenu(hygieneMenu);
  }
  if(!socialMenu->isEmpty()) {
    behavioursMenu->addMenu(socialMenu);
  }
  if(!funMenu->isEmpty()) {
    behavioursMenu->addMenu(funMenu);
  }
  if(!movementMenu->isEmpty()) {
    behavioursMenu->addMenu(movementMenu);
  }
  if(settings.idkfa) {
    behavioursMenu->addMenu(idkfaMenu);
  }
}

void MainWindow::createItemsMenu()
{
  QMenu *healthMenu = new QMenu(tr("Health"), itemsMenu);
  healthMenu->setIcon(QIcon(":health.png"));
  QMenu *energyMenu = new QMenu(tr("Energy"), itemsMenu);
  energyMenu->setIcon(QIcon(":energy.png"));
  QMenu *hungerMenu = new QMenu(tr("Food"), itemsMenu);
  hungerMenu->setIcon(QIcon(":hunger.png"));
  QMenu *bladderMenu = new QMenu(tr("Toilet"), itemsMenu);
  bladderMenu->setIcon(QIcon(":bladder.png"));
  QMenu *hygieneMenu = new QMenu(tr("Hygiene"), itemsMenu);
  hygieneMenu->setIcon(QIcon(":hygiene.png"));
  QMenu *socialMenu = new QMenu(tr("Social"), itemsMenu);
  socialMenu->setIcon(QIcon(":social.png"));
  QMenu *funMenu = new QMenu(tr("Fun"), itemsMenu);
  funMenu->setIcon(QIcon(":fun.png"));
  connect(itemsMenu, &QMenu::triggered, this, &MainWindow::spawnItem);
  for(const auto &item: itemBehaviours) {
    if(item.file.left(1) != "_" && !item.reactions.isEmpty()) {
      if(item.category == "Health") {
        QAction *tempAction =
          healthMenu->addAction(QIcon(QPixmap::fromImage(QImage(item.absoluteFilePath).copy(0, 0, 32, 32))), item.title);
        tempAction->setData(item.file);
      } else if(item.category == "Energy") {
        QAction *tempAction =
          energyMenu->addAction(QIcon(QPixmap::fromImage(QImage(item.absoluteFilePath).copy(0, 0, 32, 32))), item.title);
        tempAction->setData(item.file);
      } else if(item.category == "Hunger") {
        QAction *tempAction =
          hungerMenu->addAction(QIcon(QPixmap::fromImage(QImage(item.absoluteFilePath).copy(0, 0, 32, 32))), item.title);
        tempAction->setData(item.file);
      } else if(item.category == "Toilet") {
        QAction *tempAction =
          bladderMenu->addAction(QIcon(QPixmap::fromImage(QImage(item.absoluteFilePath).copy(0, 0, 32, 32))), item.title);
        tempAction->setData(item.file);
      } else if(item.category == "Hygiene") {
        QAction *tempAction =
          hygieneMenu->addAction(QIcon(QPixmap::fromImage(QImage(item.absoluteFilePath).copy(0, 0, 32, 32))), item.title);
        tempAction->setData(item.file);
      } else if(item.category == "Social") {
        QAction *tempAction =
          socialMenu->addAction(QIcon(QPixmap::fromImage(QImage(item.absoluteFilePath).copy(0, 0, 32, 32))), item.title);
        tempAction->setData(item.file);
      } else if(item.category == "Fun") {
        QAction *tempAction =
          funMenu->addAction(QIcon(QPixmap::fromImage(QImage(item.absoluteFilePath).copy(0, 0, 32, 32))), item.title);
        tempAction->setData(item.file);
      } else {
        QAction *tempAction =
          itemsMenu->addAction(QIcon(QPixmap::fromImage(QImage(item.absoluteFilePath).copy(0, 0, 32, 32))), item.title);
        tempAction->setData(item.file);
      }
    }
  }
  if(!healthMenu->isEmpty()) {
    itemsMenu->addMenu(healthMenu);
  }
  if(!energyMenu->isEmpty()) {
    itemsMenu->addMenu(energyMenu);
  }
  if(!hungerMenu->isEmpty()) {
    itemsMenu->addMenu(hungerMenu);
  }
  if(!bladderMenu->isEmpty()) {
    itemsMenu->addMenu(bladderMenu);
  }
  if(!hygieneMenu->isEmpty()) {
    itemsMenu->addMenu(hygieneMenu);
  }
  if(!socialMenu->isEmpty()) {
    itemsMenu->addMenu(socialMenu);
  }
  if(!funMenu->isEmpty()) {
    itemsMenu->addMenu(funMenu);
  }
}

void MainWindow::triggerBehaviour(QAction* a)
{
  QString itemFile = a->data().toString();
  for(const auto &behaviour: behaviours) {
    if(behaviour.file == itemFile) {
      if(settings.unlocked.contains(behaviour.file)) {
        emit queueBehavFromFile(behaviour.file);
      } else {
        if(settings.coins - 10 >= 0) {
          if(QMessageBox::question(nullptr, tr("Buy?"), tr("Do you want to buy '") + " " + behaviour.title + "' " + tr("for 10 coins?")) == QMessageBox::Yes) {
            addCoins("-10", - 10);
            settings.unlocked.append(behaviour.file);
            QString unlocked = "";
            for(const auto &unlock: settings.unlocked) {
              unlocked.append(unlock + ",");
            }
            unlocked = unlocked.left(unlocked.length() - 1);
            iniSettings->setValue("unlocked", unlocked);
            updateBehavioursMenu();
            emit updateBorisBehavioursMenu();
          }
        } else {
          soundMixer.playSound(&soundMixer.soundFxs["data/sfx/nocoin.wav"], 0, 1);
          QMessageBox::information(nullptr, tr("Not enough coins!"), tr("You can't afford the '") + " " + behaviour.title + "' " + tr("behaviour.\n\nPlace items that corresponds to Boris' needs to earn coins. You can then use those coins to unlock new right-click behaviours."));
        }
      }
      break;
    }
  }
}

void MainWindow::spawnRandomItem()
{
  int randomItem = 0;
  do {
    randomItem = QRandomGenerator::global()->bounded(itemBehaviours.count());
  } while(itemBehaviours.at(randomItem).reactions.isEmpty());
  
  itemList.append(new Item(QRandomGenerator::global()->bounded(QApplication::desktop()->width()),
                           QRandomGenerator::global()->bounded(QApplication::desktop()->height()),
                           settings.size,
                           itemBehaviours.at(randomItem).file,
                           &settings,
                           false));
  
  if(settings.items && settings.itemSpawnInterval) {
    itemTimer.start();
  }
}

void MainWindow::spawnItem(QAction* a)
{
  QString itemFile = a->data().toString();
  for(const auto &item: itemBehaviours) {
    if(item.file == itemFile) {
      itemList.append(new Item(QRandomGenerator::global()->bounded(QApplication::desktop()->width()),
                               QRandomGenerator::global()->bounded(QApplication::desktop()->height()),
                               settings.size,
                               item.file,
                               &settings,
                               false));
      break;
    }
  }
}

void MainWindow::addCoins(const QString &message, const int &coins)
{
  soundMixer.playSound(&soundMixer.soundFxs["data/sfx/coinup.wav"], 0, 1);
  settings.coins += coins;
  coinsAction->setText(QString::number(settings.coins) + " " + tr("coins") + " (" + message + ")");
  trayIcon->setIcon(*coinIcon);
  QTimer::singleShot(10000, this, &MainWindow::removeCoinsMessage);
  iniSettings->setValue("coins", settings.coins);
}

void MainWindow::removeCoinsMessage()
{
  coinsAction->setText(QString::number(settings.coins) + tr(" coins"));
  trayIcon->setIcon(*icon);
}
