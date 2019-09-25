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

#include <QUrl>

constexpr int STATS_ALWAYS = 42;
constexpr int STATS_CRITICAL = 43;
constexpr int STATS_MOUSEOVER = 44;
constexpr int STATS_NEVER = 45;

struct ChatLine
{
  QString type;
  QString text;
  QUrl url;
};

struct Settings {
  bool showWelcome = true;
  bool statLogging = false;
  int desktopWidth = 1920;
  
  // Boris related
  int borisX = 64;
  int borisY = 64;
  int clones = 2;
  int size = 64;
  int independence = 75;
  int stats = STATS_CRITICAL;
  bool sound = true;
  double volume = 0.5;
  bool chatter = true;

  // Secret / only in config.ini
  bool iddqd = false;
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
  QList<ChatLine> chatLines;
  QString chatFile = "chatter.dat";
  QString feedUrl = "http://rss.slashdot.org/Slashdot/slashdotMain";
  
};

#endif // SETTINGS_H
