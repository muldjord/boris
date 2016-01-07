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

#include <stdio.h>
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
  if(!settings->contains("clones")) {
    settings->setValue("clones", "1");
  }
  if(!settings->contains("size")) {
    settings->setValue("size", "64");
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

  if(settings->value("show_welcome", "true").toBool()) {
    About about(this);
    about.exec();
  }
  
  behaviours = new QList<Behaviour>;

  if(loadBehaviours()) {
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
}

MainWindow::~MainWindow()
{
  delete trayIcon;
}

bool MainWindow::loadBehaviours()
{
  QDir d(settings->value("data_folder", "data").toString(),
         "*.png",
         QDir::Name,
         QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
  QFileInfoList infoList = d.entryInfoList();
  QFileInfo info;
  foreach(info, infoList) {
    QFile dat(info.absoluteFilePath().
              left(info.absoluteFilePath().length() - 4) + ".dat");
    if(dat.open(QIODevice::ReadOnly)) {
      // Load all sprites from png
      QList<QPixmap> sprites;

      // Set seasonal color of Boris outfit
      // Default outfit is christmas, so no change for month 12
      QImage rawImage(info.absoluteFilePath());
      if(settings->value("lemmy_mode", "true").toBool()) {
        rawImage.setColor(1, QColor(255, 0, 0).rgb());
        rawImage.setColor(8, QColor(0, 0, 0).rgb());
        rawImage.setColor(9, QColor(20, 20, 20).rgb());
        rawImage.setColor(10, QColor(40, 40, 40).rgb());
      } else {
        if(QDate::currentDate().month() == 1 || QDate::currentDate().month() == 2) { // Winter outfit
          rawImage.setColor(8, QColor(109, 123, 130).rgb());
          rawImage.setColor(9, QColor(133, 151, 161).rgb());
          rawImage.setColor(10, QColor(160, 179, 189).rgb());
        } else if(QDate::currentDate().month() == 3 || QDate::currentDate().month() == 4 || QDate::currentDate().month() == 5) { // Spring outfit
          rawImage.setColor(8, QColor(13, 121, 0).rgb());
          rawImage.setColor(9, QColor(13, 166, 0).rgb());
          rawImage.setColor(10, QColor(13, 200, 0).rgb());
        } else if(QDate::currentDate().month() == 6 || QDate::currentDate().month() == 7 || QDate::currentDate().month() == 8) { // Summer outfit
          rawImage.setColor(8, QColor(0, 90, 190).rgb());
          rawImage.setColor(9, QColor(0, 121, 221).rgb());
          rawImage.setColor(10, QColor(0, 155, 255).rgb());
        } else if(QDate::currentDate().month() == 9 || QDate::currentDate().month() == 10 || QDate::currentDate().month() == 11) { // Autumn outfit
          rawImage.setColor(8, QColor(148, 81, 0).rgb());
          rawImage.setColor(9, QColor(176, 97, 0).rgb());
          rawImage.setColor(10, QColor(217, 119, 0).rgb());
        }
      }

      QPixmap t = QPixmap::fromImage(rawImage);
      for (int i = 0; i < t.width() / 32; ++i) {
        sprites.append(t.copy(32 * i, 0, 32, 32));
      }

      // Create behaviour container
      Behaviour b;
      b.oneShot = false;
      b.energy = 0;
      b.hunger = 0;
      b.bladder = 0;
      b.social = 0;
      b.fun = 0;
      b.file = info.completeBaseName();
      b.title = info.completeBaseName();
      b.category = "";
      qDebug("Adding behaviour: %s\n", b.title.toStdString().c_str());

      QTextStream in(&dat);
      QString line = in.readLine();
      while (line != "#Frames") {
        if(line == "oneShot") {
          b.oneShot = true;
        }
        if(line.contains("title")) {
          b.title = line.mid(6,line.length());
        }
        if(line.contains("category")) {
          b.category = line.mid(9,line.length());
        }
        if(line.contains("energy")) {
          b.energy = line.mid(7,line.length()).toInt();
        }
        if(line.contains("hunger")) {
          b.hunger = line.mid(7,line.length()).toInt();
        }
        if(line.contains("bladder")) {
          b.bladder = line.mid(8,line.length()).toInt();
        }
        if(line.contains("social")) {
          b.social = line.mid(7,line.length()).toInt();
        }
        if(line.contains("fun")) {
          b.fun = line.mid(4,line.length()).toInt();
        }
        line = in.readLine();
      }
#ifdef DEBUG
      qDebug("Energy: %d\nHunger: %d\nBladder: %d\nSocial: %d\nFun: %d\n", b.energy, b.hunger, b.bladder, b.social, b.fun);
#endif
      line = in.readLine();
      qreal volume = (qreal)settings->value("volume", "100").toInt() / 100.0;
      while (!line.isNull()) {
        Frame f;
        QList<QString> snippets = extractSnippets(line);
        f.sprite = sprites.at(snippets.at(0).toInt());
        f.time = snippets.at(1).toInt();
        f.dx = snippets.at(2).toInt();
        f.dy = snippets.at(3).toInt();
        f.hide = snippets.at(4).toInt();
        f.show = snippets.at(5).toInt();
        f.change = snippets.at(6).toInt();
        if(snippets.length() == 8) {
          f.soundFx = new QSoundEffect();
          f.soundFx->setSource(QUrl::fromLocalFile(snippets.at(7)));
          f.soundFx->setVolume(volume);
#ifdef DEBUG
          qDebug("Added sound FX '%s'\n", snippets.at(7).toStdString().c_str());
#endif
        } else {
          f.soundFx = NULL;
        }
        b.behaviour.append(f);

        // Read next line and go again!
        line = in.readLine();
      }
      dat.close();
      behaviours->append(b);
    } else {
      qDebug("Error in behaviour: %s\n", info.fileName().toStdString().c_str());
      return false;
    }
  }
  return true;
}

QList<QString> MainWindow::extractSnippets(QString line)
{
  QList<QString> snippets;
  QString temp;
  for(int a = 0; a < line.length(); ++a) {
    if(line.at(a) != ';') {
      temp.append(line.at(a));
    } else {
      snippets.append(temp);
      temp = "";
    }
  }
  return snippets;
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
  connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
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
  qreal volume = (qreal)settings->value("volume", "100").toInt() / 100.0;
  for(int a = 0; a < borises.length(); ++a) {
    borises.at(a)->changeSize(newSize);
    borises.at(a)->statsEnable(statsEnable);
    borises.at(a)->soundEnable(soundEnable);
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
