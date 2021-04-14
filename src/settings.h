/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            settings.h
 *
 *  Wed Jun 14 12:00:00 CEST 2017
 *  Copyright 2017 Lars Muldjord
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "item.h"
#include "behaviour.h"
#include "sprite.h"
#include "boris.h"
#include "soundmixer.h"

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QString>
#include <QImage>

constexpr int STATS_ALWAYS = 42;
constexpr int STATS_CRITICAL = 43;
constexpr int STATS_MOUSEOVER = 44;
constexpr int STATS_NEVER = 45;

struct RssLine
{
  QString text;
  QUrl url;
};

class Item;
class Boris;

class Settings : public QObject
{
  Q_OBJECT;
public:  
  Settings();
  bool showWelcome = true;
  bool statLogging = false;
  int desktopWidth = 1920;
  bool scriptOutput = false;

  // If set to true the vision processing will take place, wall detection and such
  bool vision = false;
  
  // Asset folders
  QString graphicsPath = "data/graphics";
  QString soundsPath = "data/sounds";
  QString behavioursPath = "data/behaviours";
  QString weathersPath = "data/weathers";
  QString itemsPath = "data/items";
  QString spritesPath = "data/sprites";
  QString cursorsPath = "data/cursors";

  // Boris related
  int borisX = 64;
  int borisY = 64;
  int clones = 2;
  int size = 64;
  int independence = 75;
  int stats = STATS_CRITICAL;
  bool sound = true;
  double volume = 0.5;
  bool bubbles = true;

  // Secret / only in config.ini
  bool idkfa = false;
  bool lemmyMode = false;

  // Weather
  bool forceWeatherType = false;
  QString weatherType = "11d";
  bool forceTemperature = false;
  double temperature = -42;
  bool forceWindDirection = false;
  QString windDirection = "N";
  bool forceWindSpeed = false;
  double windSpeed = 0.0;
  QString city = "Copenhagen";
  QString key = "fe9fe6cf47c03d2640d5063fbfa053a2";

  // Chatlines
  QList<RssLine> rssLines;
  QString feedUrl = "http://rss.slashdot.org/Slashdot/slashdotMain";

  // Items
  bool items = true;
  int itemSpawnInterval = 240; // Seconds between random item spawn. 0 for never
  int itemTimeout = 300; // Seconds before items disappear. 0 for never

  // Unlock related
  int coins = 0;
  QList<QString> unlocked;

  // Resources
  QPixmap missingPixmap;
  
  QList<Boris*> borisList;
  QList<Behaviour> behaviours;
  
  QList<Item*> itemList;
  QList<Behaviour> itemBehaviours;
 
  QList<Behaviour> weathers;

  QMap<QString, Sprite> sprites;

  QMap<QString, QImage> pixelFont;

  QMap<QString, QPixmap> graphics;
  QMap<QString, QPixmap> cursors;

  QPixmap& getPixmap(const QString &name);
  QCursor getCursor(const QString &name);
};

#endif // SETTINGS_H
