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
#include "settings.h"

#include <stdio.h>
#include <math.h>
#include <QApplication>
#include <QTime>
#include <QDir>
#include <QTextStream>
#include <QDesktopWidget>
#include <QSoundEffect>
#include <QAudio>

extern Settings settings;

bool Loader::loadBehaviours(QString dataDir,
                            QList<Behaviour> &behaviours,
                            const QMap<QString, QSoundEffect *> &soundFxs)
{
  QDir d(dataDir,
         "*.png",
         QDir::Name,
         QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
  QFileInfoList infoList = d.entryInfoList();
  for(const auto &info: infoList) {
    QFile dat(info.absoluteFilePath().
              left(info.absoluteFilePath().length() - 4) + ".dat");
    if(dat.open(QIODevice::ReadOnly)) {
      // Load all sprites from png
      QList<QPixmap> sprites;

      // Set seasonal color of Boris outfit
      // Default outfit is christmas, so no change for month 12
      QImage rawImage(info.absoluteFilePath());
      setClothesColor(rawImage);
      QPixmap t = QPixmap::fromImage(rawImage);
      for (int i = 0; i < t.width() / 32; ++i) {
        sprites.append(t.copy(32 * i, 0, 32, 32));
      }

      // Create behaviour container
      Behaviour b;
      b.oneShot = false;
      b.doNotDisturb = false;
      b.allowFlip = false;
      b.hyper = 0;
      b.health = 0;
      b.energy = 0;
      b.hunger = 0;
      b.bladder = 0;
      b.social = 0;
      b.fun = 0;
      b.hygiene = 0;
      b.file = info.completeBaseName();
      b.title = info.completeBaseName();
      b.category = "";
      qInfo("Adding behaviour: %s\n", b.title.toStdString().c_str());

      QTextStream in(&dat);
      QString line = in.readLine();
      while (line != "#Frames") {
        if(line == "oneShot") {
          b.oneShot = true;
        }
        if(line == "doNotDisturb") {
          b.doNotDisturb = true;
        }
        if(line == "allowFlip") {
          b.allowFlip = true;
        }
        if(line.contains("title")) {
          b.title = line.mid(6,line.length());
        }
        if(line.contains("category")) {
          b.category = line.mid(9,line.length());
        }
        if(line.contains("hyper")) {
          b.hyper = line.mid(6,line.length()).toInt();
        }
        if(line.contains("health")) {
          b.health = line.mid(7,line.length()).toInt();
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
      qInfo("Hyper: %d\n, Health: %d\nEnergy: %d\nHunger: %d\nBladder: %d\nSocial: %d\nFun: %d\nHygiene: %d\n", b.hyper, b.health, b.energy, b.hunger, b.bladder, b.social, b.fun, b.hygiene);
#endif
      line = in.readLine();
      while (!line.isNull()) {
        Frame f;
        QList<QString> snippets = line.split(";", QString::KeepEmptyParts);
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
        if(snippets.length() >= 7) {
          if(soundFxs.contains(snippets.at(6))) {
            f.soundFx = soundFxs[snippets.at(6)];
#ifdef DEBUG
            qInfo("Added sound FX '%s'\n", snippets.at(6).toStdString().c_str());
#endif
          }
        }
        b.frames.append(f);

        // Read next line and go again!
        line = in.readLine();
      }
      dat.close();
      behaviours.append(b);
    } else {
      qInfo("Error in behaviour: %s\n", info.fileName().toStdString().c_str());
      return false;
    }
  }
  return true;
}

bool Loader::loadSoundFxs(QString dataDir, QMap<QString, QSoundEffect *> &soundFxs)
{
  QDir d(dataDir,
         "*.wav",
         QDir::Name,
         QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
  QFileInfoList infoList = d.entryInfoList();
  for(const auto &info: infoList) {
    qInfo("Added sound: %s\n", info.fileName().toStdString().c_str());
    QSoundEffect *soundFx = new QSoundEffect();
    soundFx->setSource(QUrl::fromLocalFile(info.absoluteFilePath()));
    soundFx->setVolume(QAudio::convertVolume(settings.volume,
                                             QAudio::LogarithmicVolumeScale,
                                             QAudio::LinearVolumeScale));
    soundFxs[dataDir + (dataDir.right(1) == "/"?"":"/") + info.fileName()] = soundFx;
  }
  return true;
}

void Loader::setClothesColor(QImage &image)
{
  if(settings.lemmyMode) {
    image.setColor(1, QColor(255, 0, 0).rgb());
    image.setColor(8, QColor(0, 0, 0).rgb());
    image.setColor(9, QColor(20, 20, 20).rgb());
    image.setColor(10, QColor(40, 40, 40).rgb());
  } else {
    if(QDate::currentDate().month() == 1 || QDate::currentDate().month() == 2) { // Winter outfit
      image.setColor(8, QColor(109, 123, 130).rgb());
      image.setColor(9, QColor(133, 151, 161).rgb());
      image.setColor(10, QColor(160, 179, 189).rgb());
    } else if(QDate::currentDate().month() == 3 || QDate::currentDate().month() == 4 || QDate::currentDate().month() == 5) { // Spring outfit
      image.setColor(8, QColor(13, 121, 0).rgb());
      image.setColor(9, QColor(13, 166, 0).rgb());
      image.setColor(10, QColor(13, 200, 0).rgb());
    } else if(QDate::currentDate().month() == 6 || QDate::currentDate().month() == 7 || QDate::currentDate().month() == 8) { // Summer outfit
      image.setColor(8, QColor(0, 90, 190).rgb());
      image.setColor(9, QColor(0, 121, 221).rgb());
      image.setColor(10, QColor(0, 155, 255).rgb());
    } else if(QDate::currentDate().month() == 9 || QDate::currentDate().month() == 10 || QDate::currentDate().month() == 11) { // Autumn outfit
      image.setColor(8, QColor(105, 47, 0).rgb());
      image.setColor(9, QColor(148, 67, 0).rgb());
      image.setColor(10, QColor(203, 91, 0).rgb());
    }
  }
}
