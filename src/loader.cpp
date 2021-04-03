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
#include <QDir>
#include <QTextStream>
#include <QDesktopWidget>

qint64 Loader::getAssetsSize(const QDir &dir)
{
  qint64 assetsSize = 0;
  for(const auto &asset: dir.entryInfoList()) {
    assetsSize += asset.size();
  }
  return assetsSize;
}

bool Loader::loadSoundFxs(const QString &dataDir,
                          QMap<QString, sf::SoundBuffer> &soundFxs,
                          QProgressBar *progressBar)
{
  QDir d(dataDir,
         "*.wav",
         QDir::Name,
         QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
  QFileInfoList infoList = d.entryInfoList();
  bool noErrors = true;
  for(const auto &info: infoList) {
    sf::SoundBuffer soundFx;
    if(soundFx.loadFromFile(info.absoluteFilePath().toStdString())) {
      soundFxs[dataDir + (dataDir.right(1) == "/"?"":"/") + info.fileName()] = soundFx;
      qInfo("  Added sound: %s\n", info.fileName().toStdString().c_str());
    } else {
      noErrors = false;
      qWarning("  Error when loading sound: %s\n", info.fileName().toStdString().c_str());
    }
    progressBar->setValue(progressBar->value() + info.size());
  }
  return noErrors;
}

bool Loader::loadBehaviours(const Settings &settings,
                            const QString &dataDir,
                            QList<Behaviour> &behaviours,
                            QMap<QString, sf::SoundBuffer> &soundFxs,
                            QProgressBar *progressBar)
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
      setClothesColor(settings, rawImage);
      QPixmap t = QPixmap::fromImage(rawImage);
      if(t.width() % 32 != 0) {
        qWarning("  Sprite does not adhere to 32 pixel width per sprite, can't load...\n");
        return false;
      } else {
        for (int i = 0; i < t.width() / 32; ++i) {
          sprites.append(t.copy(32 * i, 0, 32, 32));
        }
      }

      // Create behaviour container
      Behaviour b;
      b.file = info.completeBaseName();
      b.title = info.completeBaseName();

      while(!dat.atEnd()) {
        QByteArray line = dat.readLine().simplified();
        if(line.isEmpty()) {
          continue;
        }
        if(line.contains("#Frames")) {
          break;
        }
        if(line.contains("define") && line.contains(":")) {
          QString script = QString(line);
          while(script.right(1) == ",") {
            script.append(QString(dat.readLine().simplified()));
          }
          script = script.simplified();
          b.defines[script.split(":").first().split(" ").at(1)] = parseScript(script.split(":").at(1));
          continue;
        }
        if(line == "oneShot") {
          b.oneShot = true;
          continue;
        }
        if(line == "doNotDisturb") {
          b.doNotDisturb = true;
          continue;
        }
        if(line == "allowFlip") {
          b.allowFlip = true;
          continue;
        }
        if(line == "pitchLock") {
          b.pitchLock = true;
          continue;
        }
        if(line.contains("title")) {
          b.title = line.mid(6,line.length());
          continue;
        }
        if(line.contains("category")) {
          b.category = line.mid(9,line.length());
          continue;
        }
        if(line.contains("hyper")) {
          b.hyper = line.mid(6,line.length()).toInt();
          continue;
        }
        if(line.contains("health")) {
          b.health = line.mid(7,line.length()).toInt();
          continue;
        }
        if(line.contains("energy")) {
          b.energy = line.mid(7,line.length()).toInt();
          continue;
        }
        if(line.contains("hunger")) {
          b.hunger = line.mid(7,line.length()).toInt();
          continue;
        }
        if(line.contains("bladder")) {
          b.bladder = line.mid(8,line.length()).toInt();
          continue;
        }
        if(line.contains("social")) {
          b.social = line.mid(7,line.length()).toInt();
          continue;
        }
        if(line.contains("fun")) {
          b.fun = line.mid(4,line.length()).toInt();
          continue;
        }
        if(line.contains("hygiene")) {
          b.hygiene = line.mid(8,line.length()).toInt();
          continue;
        }
      }
      int frames = 0;
      while(!dat.atEnd()) {
        QString line = QString(dat.readLine().simplified());
        if(line.isEmpty() || line.left(1) == "#") {
          continue;
        }
        Frame f;
#if QT_VERSION >= 0x050e00
        QList<QString> snippets = line.split(";", Qt::KeepEmptyParts);
#else
        QList<QString> snippets = line.split(";", QString::KeepEmptyParts);
#endif
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
        if(!snippets.at(4).isEmpty()) {
          if(soundFxs.contains(snippets.at(4))) {
            f.soundBuffer = &soundFxs[snippets.at(4)];
          }
        }
        if(!snippets.at(5).isEmpty()) {
          QString script = snippets.at(5).simplified();
          while(script.right(1) == ",") {
            script.append(QString(dat.readLine().simplified()));
          }
          script = script.simplified();
          f.script = parseScript(script);
          const QList<QString> instructions = script.split(",");
          for(const auto &instruction: instructions) {
            if(instruction.split(" ").count() == 2 &&
               instruction.split(" ").first() == "label") {
              // Point this label to current frame
              b.labels[instruction.split(" ").at(1)] = frames;
            }
          }
        }
        b.frames.append(f);
        frames++;
      }
      dat.close();
      qInfo("  Added behaviour: %s\n", b.title.toStdString().c_str());
      behaviours.append(b);
    } else {
      qWarning("  Error in behaviour: %s\n", info.fileName().toStdString().c_str());
      return false;
    }
    progressBar->setValue(progressBar->value() + info.size());
  }
  return true;
}

Script Loader::parseScript(const QString &script)
{
  Script returnScript;
  int blockLevel = 0;
  int blockId = 0;
  bool inBlock = false;
  QString commands = script;
  QString childScript = "";
  for(const auto ch: script) {
    if(ch == '{') {
      blockLevel++;
    }
    if(ch == '}') {
      blockLevel--;
    }
    if(blockLevel >= 1) {
      if(!inBlock) {
        inBlock = true;
        blockId++;
      }
      childScript.append(ch);
    } else {
      if(inBlock && !childScript.isEmpty()) {
        inBlock = false;
        childScript.append(ch); // Add the last '}' as well
        QString cleaned = childScript.mid(1, childScript.length() - 2).simplified();
        QString blockIdStr = "##" + QString::number(blockId) + "##";
        commands.replace(childScript, blockIdStr);
        returnScript.blocks[blockIdStr] = parseScript(cleaned);
        childScript.clear();
      }
    } 
  }
  if(commands.contains("\"")) {
    bool inQuotes = false;
    QString command = "";
    for(const auto &curChar: commands.split("")) {
      // if this = 1 then say "This, that" else if this = 2 say "Something" else goto 10,
      if(curChar == "\"") {
        if(inQuotes) {
          inQuotes = false;
        } else {
          inQuotes = true;
        }
      }
      if(curChar == "," && !inQuotes) {
        returnScript.commands.append(command);
        command.clear();
      } else {
        command.append(curChar);
      }
    }
    returnScript.commands.append(command);
  } else {
    returnScript.commands = commands.split(",");
  }
  return returnScript;
}

bool Loader::loadFont(QMap<QString, QImage> &pfont)
{
  QImage fontSheet(":pfont.png");
  if(!fontSheet.isNull()) {
    QList<QString> fontChars({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Æ", "Ø", "Å", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "æ", "ø", "å", " ", ".", ",", ":", ";", "!", "?", "\"", "\'", "_", "+", "-", "*", "/", "<", ">", "="});
    int w = fontSheet.width();
    int h = fontSheet.height();
    int x1 = 0;
    int x2 = 0;
    // Load all characters from png sprite sheet
    for(const auto &fontChar: fontChars) {
      // Look for first purple non-char pixel
      while(x2 < w && fontSheet.pixelIndex(x2, 0) != 2) {
        x2++;
      }
      pfont[fontChar] = fontSheet.copy(x1, 0, x2 - x1, h);
      // Move past purple non-char area to where next char begins
      while(x2 < w && fontSheet.pixelIndex(x2, 0) == 2) {
        x2++;
      }
      x1 = x2;
    }
  } else {
    return false;
  }
  return true;
}

void Loader::setClothesColor(const Settings &settings, QImage &image)
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
