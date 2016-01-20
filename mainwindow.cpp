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

#include <stdio.h>
#include <math.h>
#include <QApplication>
#include <QTime>
#include <QSettings>
#include <QDir>
#include <QTextStream>
#include <QDesktopWidget>

//#define DEBUG

extern QSettings *settings;

MainWindow::MainWindow()
{
  QTime time = QTime::currentTime();
  qsrand((uint)time.msec());

  if(!settings->contains("boris_x")) {
    settings->setValue("boris_x", QApplication::desktop()->width() / 2);
  }
  if(!settings->contains("boris_y")) {
    settings->setValue("boris_y", QApplication::desktop()->height() / 2);
  }
  if(!settings->contains("show_welcome")) {
    settings->setValue("show_welcome", "true");
  }
  if(!settings->contains("behavs_path")) {
    settings->setValue("behavs_path", "data/behavs");
  }
  if(!settings->contains("clones")) {
    settings->setValue("clones", "2");
  }
  if(!settings->contains("size")) {
    settings->setValue("size", "64");
  }
  if(!settings->contains("independence")) {
    settings->setValue("independence", "75");
  }
  if(!settings->contains("stats")) {
    settings->setValue("stats", "false");
  }
  if(!settings->contains("sound")) {
    settings->setValue("sound", "true");
  }
  if(!settings->contains("volume")) {
    settings->setValue("volume", "50");
  }
  if(!settings->contains("lemmy_mode")) {
    settings->setValue("lemmy_mode", "false");
  }
  if(!settings->contains("timeFactor")) {
    settings->setValue("timeFactor", "1");
  }

  if(settings->value("show_welcome", "true").toBool()) {
    About about(this);
    about.exec();
  }
  
  behaviours = new QList<Behaviour>;

  if(Loader::loadBehaviours(settings->value("behavs_path", "data/behavs").toString(), behaviours, this)) {
    qDebug("Behaviours loaded ok... :)\n");
  } else {
    qDebug("Error when loading some behaviours, please check your png and dat files\n");
  }

  createActions();
  createTrayIcon();
  trayIcon->show();

  clones = settings->value("clones", "4").toInt();
  qDebug("Spawning %d clone(s)\n", clones);
  addBoris(settings->value("clones", "4").toInt());

  collisTimer.setInterval(1000);
  collisTimer.setSingleShot(true);
  connect(&collisTimer, SIGNAL(timeout()), this, SLOT(checkCollisions()));
  collisTimer.start();
}

MainWindow::~MainWindow()
{
  delete trayIcon;
}

void MainWindow::addBoris(int clones)
{
  for(int a = 0; a < clones; ++a) {
    borises.append(new Boris(behaviours, this));
    connect(earthquakeAction, SIGNAL(triggered()), borises.last(), SLOT(earthquake()));
    connect(upAction, SIGNAL(triggered()), borises.last(), SLOT(walkUp()));
    connect(downAction, SIGNAL(triggered()), borises.last(), SLOT(walkDown()));
    connect(leftAction, SIGNAL(triggered()), borises.last(), SLOT(walkLeft()));
    connect(rightAction, SIGNAL(triggered()), borises.last(), SLOT(walkRight()));
    borises.last()->show();
    borises.last()->earthquake();
  }
}

void MainWindow::removeBoris(int clones)
{
  // Reset all Boris collide pointers within all existing clones to prevent crash
  for(int a = 0; a < borises.length(); ++a) {
    borises.at(a)->boris = NULL;
  }
  for(int a = 0; a < clones; ++a) {
    delete borises.last();
    borises.removeLast();
  }
}

void MainWindow::createActions()
{
  aboutAction = new QAction(tr("&Config / about..."), this);
  aboutAction->setIcon(QIcon(":icon_about.png"));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutBox()));

  earthquakeAction = new QAction(tr("&Earthquake"), this);
  upAction = new QAction(tr("Walk up"), this);
  downAction = new QAction(tr("Walk down"), this);
  leftAction = new QAction(tr("Walk left"), this);
  rightAction = new QAction(tr("Walk right"), this);

  quitAction = new QAction(tr("&Quit"), this);
  quitAction->setIcon(QIcon(":icon_quit.png"));
  connect(quitAction, SIGNAL(triggered()), this, SLOT(killAll()));
}

void MainWindow::createTrayIcon()
{
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(aboutAction);
  trayIconMenu->addAction(earthquakeAction);
  trayIconMenu->addAction(upAction);
  trayIconMenu->addAction(downAction);
  trayIconMenu->addAction(leftAction);
  trayIconMenu->addAction(rightAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  trayIcon = new QSystemTrayIcon(this);
  QIcon icon(":icon.png");
  trayIcon->setToolTip("Boris");
  trayIcon->setIcon(icon);
  trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::aboutBox()
{
  About about(this);
  about.exec();
  int newSize = settings->value("size", "32").toInt();
  bool soundEnable = settings->value("sound", "true").toBool();
  bool statsEnable = settings->value("stats", "true").toBool();
  int independence = settings->value("independence", "0").toInt();
  qreal volume = (qreal)settings->value("volume", "100").toInt() / 100.0;
  for(int a = 0; a < borises.length(); ++a) {
    borises.at(a)->changeSize(newSize);
    borises.at(a)->statsEnable(statsEnable);
    borises.at(a)->soundEnable(soundEnable);
    borises.at(a)->setIndependence(independence);
    for(int b = 0; b < behaviours->length(); ++b) {
      for(int c = 0; c < behaviours->at(b).behaviour.length(); ++c) {
        if(behaviours->at(b).behaviour.at(c).soundFx != NULL) {
          behaviours->at(b).behaviour.at(c).soundFx->setVolume(volume);
        }
      }
    }
  }
  if(settings->value("clones", "2").toInt() != clones) {
    if(clones > settings->value("clones", "2").toInt()) {
      removeBoris(clones - settings->value("clones", "2").toInt());
    } else if(clones < settings->value("clones", "2").toInt()) {
      addBoris(settings->value("clones", "2").toInt() - clones);
    }
    clones = settings->value("clones", "2").toInt();
  }
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::RightButton) {
    trayIconMenu->exec(QCursor::pos());
  }
}

void MainWindow::checkCollisions()
{
  for(int a = 0; a < borises.length(); ++a) {
    for(int b = 0; b < borises.length(); ++b) {
      if(a == b) {
        continue;
      }
      int borisSize = borises.at(a)->borisSize;
      if(borises.at(b)->borisSize > borisSize) {
        borisSize = borises.at(b)->borisSize;
      }
      int xA = borises.at(a)->pos().x();
      int yA = borises.at(a)->pos().y();
      int xB = borises.at(b)->pos().x();
      int yB = borises.at(b)->pos().y();
      double hypotenuse = sqrt((yB - yA) * (yB - yA) + (xB - xA) * (xB - xA));
      if(fabs(hypotenuse) < borisSize) {
        borises.at(a)->collide(borises.at(b));
        borises.at(b)->collide(borises.at(a));
      }
    }
  }
  collisTimer.start();
}

void MainWindow::killAll()
{
  QTimer::singleShot(5000, qApp, SLOT(quit()));
  for(int a = 0; a < borises.length(); ++a) {
    borises.at(a)->changeBehaviour("_drop_dead");
  }
}
