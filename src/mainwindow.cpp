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
#include <QSettings>
#include <QDir>
#include <QTextStream>
#include <QDesktopWidget>
#include <QHostInfo>
#include <QLabel>
#include <QVBoxLayout>

#if QT_VERSION >= 0x050a00
#include <QRandomGenerator>
#endif

QList<Item*> itemList;
QList<Behaviour> itemBehaviours;

QList<Boris*> borises;
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
  if(QFileInfo::exists("config_" + QHostInfo::localHostName().toLower() + ".ini")) {
    iniFile = "config_" + QHostInfo::localHostName().toLower() + ".ini";
  }
  QSettings iniSettings(iniFile, QSettings::IniFormat);
  if(!iniSettings.contains("show_welcome")) {
    iniSettings.setValue("show_welcome", true);
  }
  settings.showWelcome = iniSettings.value("show_welcome").toBool();

  if(!iniSettings.contains("stat_logging")) {
    iniSettings.setValue("stat_logging", false);
  }
  settings.statLogging = iniSettings.value("stat_logging").toBool();

  if(!iniSettings.contains("script_output")) {
    iniSettings.setValue("script_output", true);
  }
  settings.scriptOutput = iniSettings.value("script_output").toBool();
  
  if(!iniSettings.contains("sounds_path")) {
    iniSettings.setValue("sounds_path", "data/sfx");
  }
  settings.soundsPath = iniSettings.value("sounds_path").toString();

  if(!iniSettings.contains("behavs_path")) {
    iniSettings.setValue("behavs_path", "data/behavs");
  }
  settings.behavsPath = iniSettings.value("behavs_path").toString();
  
  if(!iniSettings.contains("weathers_path")) {
    iniSettings.setValue("weathers_path", "data/weathers");
  }
  settings.weathersPath = iniSettings.value("weathers_path").toString();

  if(!iniSettings.contains("items_path")) {
    iniSettings.setValue("items_path", "data/items");
  }
  settings.itemsPath = iniSettings.value("items_path").toString();

  if(!iniSettings.contains("sprites_path")) {
    iniSettings.setValue("sprites_path", "data/sprites");
  }
  settings.spritesPath = iniSettings.value("sprites_path").toString();

  // Force weather from ini
  if(iniSettings.contains("weather_force_type")) {
    settings.weatherType = iniSettings.value("weather_force_type").toString();
    settings.forceWeatherType = true;
  }
  if(iniSettings.contains("weather_force_temp")) {
    settings.temperature = iniSettings.value("weather_force_temp").toDouble();
    settings.forceTemperature = true;
  }
  if(iniSettings.contains("weather_force_wind_direction")) {
    settings.windDirection = iniSettings.value("weather_force_wind_direction").toString();
    settings.forceWindDirection = true;
  }
  if(iniSettings.contains("weather_force_wind_speed")) {
    settings.windSpeed = iniSettings.value("weather_force_wind_speed").toDouble();
    settings.forceWindSpeed = true;
  }
  
  if(!iniSettings.contains("boris_x")) {
    iniSettings.setValue("boris_x", QApplication::desktop()->width() / 2);
  }
  settings.borisX = iniSettings.value("boris_x").toInt();

  if(!iniSettings.contains("boris_y")) {
    iniSettings.setValue("boris_y", QApplication::desktop()->height() / 2);
  }
  settings.borisY = iniSettings.value("boris_y").toInt();

  // For use with sound panning in relation to Boris location
  settings.desktopWidth = QApplication::desktop()->width();

  if(!iniSettings.contains("vision")) {
    iniSettings.setValue("vision", false);
  }
  settings.vision = iniSettings.value("vision").toBool();

  if(!iniSettings.contains("clones")) {
    iniSettings.setValue("clones", 2);
  }
  settings.clones = iniSettings.value("clones").toInt();
    
  if(!iniSettings.contains("size")) {
    iniSettings.setValue("size", 64);
  }
  settings.size = iniSettings.value("size").toInt();

  if(!iniSettings.contains("independence")) {
    iniSettings.setValue("independence", 75);
  }
  settings.independence = iniSettings.value("independence").toInt();

  if(!iniSettings.contains("item_timeout")) {
    iniSettings.setValue("item_timeout", 300);
  }
  settings.itemTimeout = iniSettings.value("item_timeout").toInt();

  if(!iniSettings.contains("iddqd")) {
    iniSettings.setValue("iddqd", false);
  }
  settings.iddqd = iniSettings.value("iddqd").toBool();
  
  if(!iniSettings.contains("stats")) {
    iniSettings.setValue("stats", "critical");
  }
  if(iniSettings.value("stats").toString() == "always") {
    settings.stats = STATS_ALWAYS;
  } else if(iniSettings.value("stats").toString() == "critical") {
    settings.stats = STATS_CRITICAL;
  } else if(iniSettings.value("stats").toString() == "mouseover") {
    settings.stats = STATS_MOUSEOVER;
  } else if(iniSettings.value("stats").toString() == "never") {
    settings.stats = STATS_NEVER;
  }
  
  if(!iniSettings.contains("bubbles")) {
    iniSettings.setValue("bubbles", true);
  }
  settings.bubbles = iniSettings.value("bubbles").toBool();

  if(!iniSettings.contains("sound")) {
    iniSettings.setValue("sound", true);
  }
  settings.sound = iniSettings.value("sound").toBool();

  if(!iniSettings.contains("volume")) {
    iniSettings.setValue("volume", 50);
  }
  settings.volume = iniSettings.value("volume").toInt() / 100.0;
  sf::Listener::setGlobalVolume(settings.volume * 100.0);
  
  if(!iniSettings.contains("items")) {
    iniSettings.setValue("items", true);
  }
  settings.items = iniSettings.value("items").toBool();

  if(!iniSettings.contains("lemmy_mode")) {
    iniSettings.setValue("lemmy_mode", false);
  }
  settings.lemmyMode = iniSettings.value("lemmy_mode").toBool();

  if(!iniSettings.contains("feed_url")) {
    iniSettings.setValue("feed_url", "http://rss.slashdot.org/Slashdot/slashdotMain");
  }
  settings.feedUrl = iniSettings.value("feed_url").toString();

  if(!iniSettings.contains("weather_city")) {
    iniSettings.setValue("weather_city", "Copenhagen");
  }
  settings.city = iniSettings.value("weather_city").toString();

  if(!iniSettings.contains("weather_key")) {
    iniSettings.setValue("weather_key", "fe9fe6cf47c03d2640d5063fbfa053a2");
  }
  settings.key = iniSettings.value("weather_key").toString();

  createActions();
  bMenu = new QMenu();
  bMenu->setTitle(tr("Behaviours"));
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

  QTimer::singleShot(100, this, &MainWindow::loadAssets);
}

MainWindow::~MainWindow()
{
  QString iniFile = "config.ini";
  if(QFileInfo::exists("config_" + QHostInfo::localHostName().toLower() + ".ini")) {
    iniFile = "config_" + QHostInfo::localHostName().toLower() + ".ini";
  }
  QSettings iniSettings(iniFile, QSettings::IniFormat);
  iniSettings.setValue("boris_x", settings.borisX);
  iniSettings.setValue("boris_y", settings.borisY);
  delete trayIcon;
  for(auto &boris: borises) {
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

  createBehavMenu();
}

void MainWindow::addBoris(int clones)
{
  qInfo("Spawning %d clone(s)\n", clones);
  while(clones--) {
    borises << new Boris(&settings);
    connect(this, &MainWindow::updateBoris, borises.last(), &Boris::updateBoris);
    connect(earthquakeAction, &QAction::triggered, borises.last(), &Boris::earthquake);
    connect(this, &MainWindow::queueBehavFromFile, borises.last(), &Boris::queueBehavFromFile);
    connect(weatherAction, &QAction::triggered, borises.last(), &Boris::triggerWeather);
    borises.last()->show();
    borises.last()->earthquake();
  }
}

void MainWindow::removeBoris(int clones)
{
  // Reset all Boris collide pointers within all existing clones to prevent crash
  for(const auto &boris: borises) {
    boris->borisFriend = nullptr;
  }
  while(clones--) {
    delete borises.takeLast();
  }
}

void MainWindow::createActions()
{
  aboutAction = new QAction(tr("&Config / about..."), this);
  aboutAction->setIcon(QIcon(":icon_about.png"));
  connect(aboutAction, &QAction::triggered, this, &MainWindow::aboutBox);

  earthquakeAction = new QAction(tr("&Earthquake!!!"), this);
  earthquakeAction->setIcon(QIcon(":earthquake.png"));

  weatherAction = new QAction(tr("Updating weather..."), this);
  
  quitAction = new QAction(tr("&Quit"), this);
  quitAction->setIcon(QIcon(":icon_quit.png"));
  connect(quitAction, &QAction::triggered, this, &MainWindow::killAll);
}

void MainWindow::createTrayIcon()
{
  trayIcon = new QSystemTrayIcon;

  trayIconMenu = new QMenu;
  trayIconMenu->addAction(aboutAction);
  trayIconMenu->addAction(earthquakeAction);
  trayIconMenu->addMenu(bMenu);
  trayIconMenu->addAction(weatherAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  QImage iconImage(":icon.png");
  Loader::setClothesColor(settings, iconImage);
  QIcon icon(QPixmap::fromImage(iconImage));
  trayIcon->setToolTip("Boris");
  trayIcon->setIcon(icon);
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
  if(borises.count() > newClones) {
    removeBoris(borises.count() - newClones);
  } else if(borises.count() < newClones) {
    addBoris(newClones - borises.count());
  }
  
  netComm->updateAll();
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
  for(auto &boris: borises) {
    boris->changeBehaviour("casual_wave");
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

void MainWindow::createBehavMenu()
{
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
  connect(healthMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
  connect(energyMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
  connect(hungerMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
  connect(bladderMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
  connect(hygieneMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
  connect(socialMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
  connect(funMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
  connect(movementMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
  connect(iddqdMenu, &QMenu::triggered, this, &MainWindow::triggerBehaviour);
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
  bMenu->addMenu(healthMenu);
  bMenu->addMenu(energyMenu);
  bMenu->addMenu(hungerMenu);
  bMenu->addMenu(bladderMenu);
  bMenu->addMenu(hygieneMenu);
  bMenu->addMenu(socialMenu);
  bMenu->addMenu(funMenu);
  bMenu->addMenu(movementMenu);
  if(settings.iddqd) {
    bMenu->addMenu(iddqdMenu);
  }
}

void MainWindow::triggerBehaviour(QAction* a)
{
  for(const auto &behaviour: behaviours) {
    if(behaviour.title == a->text()) {
      emit queueBehavFromFile(behaviour.file);
    }
  }
}
