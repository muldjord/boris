/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            loader.cpp
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

bool Loader::loadBehaviours(QString dataDir, QList<Behaviour> *behaviours, QWidget *parent)
{
  QDir d(dataDir,
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
      b.hygiene = 0;
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
        if(line.contains("hygiene")) {
          b.hygiene = line.mid(8,line.length()).toInt();
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
        if(snippets.at(2) == "rand") {
          f.dx = 666; // This is an ugly way of setting it to random since it's an int
        } else {
          f.dx = snippets.at(2).toInt();
        }
        if(snippets.at(3) == "rand") {
          f.dy = 666; // This is an ugly way of setting it to random since it's an int
        } else {
          f.dy = snippets.at(3).toInt();
        }
        f.hide = snippets.at(4).toInt();
        f.show = snippets.at(5).toInt();
        f.change = snippets.at(6).toInt();
        if(snippets.length() == 8) {
          f.soundFx = new QSoundEffect(parent);
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

QList<QString> Loader::extractSnippets(QString line)
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
