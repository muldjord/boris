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
#include "settings.h"

#include <stdio.h>
#include <math.h>
#include <QApplication>
#include <QTime>
#include <QSettings>
#include <QDir>
#include <QTextStream>
#include <QDesktopWidget>
#include <QHostInfo>
#include <QAudio>

QList<Behaviour> behaviours;
QList<Behaviour> weathers;
Settings settings;

MainWindow::MainWindow()
{
  qsrand((uint)QTime::currentTime().msec());

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

  if(!iniSettings.contains("behavs_path")) {
    iniSettings.setValue("behavs_path", "data/behavs");
  }
  
  if(!iniSettings.contains("weather_path")) {
    iniSettings.setValue("weather_path", "data/weathers");
  }

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
  
  if(!iniSettings.contains("sounds_path")) {
    iniSettings.setValue("sounds_path", "data/sfx");
  }
  
  if(!iniSettings.contains("boris_x")) {
    iniSettings.setValue("boris_x", QApplication::desktop()->width() / 2);
  }
  settings.borisX = iniSettings.value("boris_x").toInt();

  if(!iniSettings.contains("boris_y")) {
    iniSettings.setValue("boris_y", QApplication::desktop()->height() / 2);
  }
  settings.borisY = iniSettings.value("boris_y").toInt();

  if(!iniSettings.contains("clones")) {
    iniSettings.setValue("clones", 2);
  }
  if(!iniSettings.contains("size")) {
    iniSettings.setValue("size", 64);
  }
  settings.size = iniSettings.value("size").toInt();

  if(!iniSettings.contains("independence")) {
    iniSettings.setValue("independence", 75);
  }
  settings.independence = iniSettings.value("independence").toInt();
  
  if(!iniSettings.contains("iddqd")) {
    iniSettings.setValue("iddqd", false);
  }
  settings.iddqd = iniSettings.value("iddqd").toBool();

  
  if(!iniSettings.contains("stats")) {
    iniSettings.setValue("stats", "mouseover");
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
  
  if(!iniSettings.contains("chatter")) {
    iniSettings.setValue("chatter", true);
  }
  settings.chatter = iniSettings.value("chatter").toBool();

  if(!iniSettings.contains("chat_file")) {
    iniSettings.setValue("chat_file", "data/chatter.dat");
  }
  settings.chatFile = iniSettings.value("chat_file").toString();
  
  if(!iniSettings.contains("sound")) {
    iniSettings.setValue("sound", true);
  }
  settings.sound = iniSettings.value("sound").toBool();

  if(!iniSettings.contains("volume")) {
    iniSettings.setValue("volume", 50);
  }
  settings.volume = iniSettings.value("volume").toInt() / 100.0;

  if(!iniSettings.contains("lemmy_mode")) {
    iniSettings.setValue("lemmy_mode", false);
  }

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

  if(iniSettings.value("show_welcome").toBool()) {
    About about(this);
    about.exec();
  }
  
  if(Loader::loadSoundFxs(iniSettings.value("sounds_path").toString(),
                          soundFxs)) {
    qInfo("Sounds loaded ok... :)\n");
  } else {
    qInfo("Error when loading some sounds, please check your wav files\n");
  }

  if(Loader::loadBehaviours(iniSettings.value("behavs_path").toString(),
                            behaviours, soundFxs)) {
    qInfo("Behaviours loaded ok... :)\n");
  } else {
    qInfo("Error when loading some behaviours, please check your png and dat files\n");
  }
  
  if(Loader::loadBehaviours(iniSettings.value("weather_path").toString(),
                            weathers, soundFxs)) {
    qInfo("Weather types loaded ok... :)\n");
  } else {
    qInfo("Error when loading some weather types, please check your png and dat files\n");
  }

  createActions();
  createTrayIcon();
  trayIcon->show();

  netComm = new NetComm();
  connect(netComm, &NetComm::weatherUpdated, this, &MainWindow::updateWeather);

  settings.clones = iniSettings.value("clones").toInt();
  addBoris((settings.clones == 0?qrand() % 99 + 1:settings.clones));

  collisTimer.setInterval(1000);
  collisTimer.setSingleShot(true);
  connect(&collisTimer, &QTimer::timeout, this, &MainWindow::checkCollisions);
  collisTimer.start();
}

MainWindow::~MainWindow()
{
  delete trayIcon;
  for(auto &boris: borises) {
    delete boris;
  }
}

void MainWindow::addBoris(int clones)
{
  qInfo("Spawning %d clone(s)\n", clones);
  while(clones--) {
    borises << new Boris(this);
    connect(earthquakeAction, &QAction::triggered, borises.last(), &Boris::earthquake);
    connect(teleportAction, &QAction::triggered, borises.last(), &Boris::teleport);
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
  teleportAction = new QAction(tr("&Beam me up, Scotty!"), this);
  teleportAction->setIcon(QIcon(":teleport.png"));

  weatherAction = new QAction(tr("Updating weather..."), this);
  
  quitAction = new QAction(tr("&Quit"), this);
  quitAction->setIcon(QIcon(":icon_quit.png"));
  connect(quitAction, &QAction::triggered, this, &MainWindow::killAll);
}

void MainWindow::createTrayIcon()
{
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(aboutAction);
  trayIconMenu->addAction(earthquakeAction);
  trayIconMenu->addAction(teleportAction);
  trayIconMenu->addAction(weatherAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  trayIcon = new QSystemTrayIcon(this);

  QImage iconImage(":icon.png");
  Loader::setClothesColor(iconImage);
  QIcon icon(QPixmap::fromImage(iconImage));
  trayIcon->setToolTip("Boris");
  trayIcon->setIcon(icon);
  trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::aboutBox()
{
  About about(this);
  about.exec();
  for(auto &boris: borises) {
    boris->updateBoris();
    for(const auto &key: soundFxs.keys()) {
      soundFxs[key]->setVolume(QAudio::convertVolume(settings.volume,
                                                     QAudio::LogarithmicVolumeScale,
                                                     QAudio::LinearVolumeScale));
    }
  }
  int newClones = settings.clones;
  if(newClones == 0) {
    newClones = (qrand() % 99) + 1;
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

void MainWindow::checkCollisions()
{
  for(auto &borisA: borises) {
    for(auto &borisB: borises) {
      if(borisA != borisB) {
        int xA = borisA->pos().x();
        int yA = borisA->pos().y();
        int xB = borisB->pos().x();
        int yB = borisB->pos().y();
        double hypotenuse = sqrt((yB - yA) * (yB - yA) + (xB - xA) * (xB - xA));
        if(hypotenuse < borisB->size * 2) {
          borisA->collide(borisB);
          break;
        }
      }
    }
  }
  collisTimer.start();
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
