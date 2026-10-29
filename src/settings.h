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

#pragma once

#include "behaviour.h"
//#include "boris.h"
//#include "item.h"

//#include <QGraphicsView>
//#include <QImage>
//#include <QMap>
#include <QPixmap>
#include <QObject>
#include <QString>
#include <QUrl>

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
  qreal desktopWidth = 1920.0;
  qreal desktopHeight = 1080.0;
  bool scriptOutput = false;

  // Asset folders
  QString graphicsPath = "data/graphics";
  QString soundsPath = "data/sounds";
  QString behavioursPath = "data/behaviours";
  QString weathersPath = "data/weathers";
  QString itemsPath = "data/items";
  QString spritesPath = "data/sprites";
  QString cursorsPath = "data/cursors";

  // Boris related
  int clones = 2;
  int baseSize = 32; // This is just the basic size of all Boris sprites
  int size = 64; // This is the on-screen size of Boris
  double sizeFactor = 2.0;
  int independence = 75;
  bool isInvincible = false;
  int stats = STATS_CRITICAL;
  bool sound = true;
  int volume = 50; // 0-100
  bool bubbles = true;
  double mouseVVel = 0.0;
  double mouseHVel = 0.0;

  // Secret / only in config.ini
  bool idkfa = false;
  bool lemmyMode = false;

  // Weather
  bool forceWeatherType = false;
  QString weatherType = "11d";
  int weatherIdx = -1;
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

  QMap<QString, QList<QImage> > sprites;

  QMap<QString, QImage> pixelFont;

  QMap<QString, QPixmap> graphics;
  QMap<QString, QPixmap> cursors;

  QPixmap& getPixmap(const QString &name);
  QCursor getCursor(const QString &name);
};
