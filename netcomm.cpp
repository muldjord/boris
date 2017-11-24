/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            netcomm.cpp
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

#include "netcomm.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QSettings>
#include <QFile>
#include <QDomDocument>

//#define DEBUG

extern QSettings *settings;

NetComm::NetComm()
{
  connect(this, &NetComm::finished, this, &NetComm::netReply);

  netTimer.setInterval(3600000);
  netTimer.setSingleShot(true);
  connect(&netTimer, &QTimer::timeout, this, &NetComm::updateAll);

  QTimer::singleShot(0, this, SLOT(updateAll()));
}

NetComm::~NetComm()
{
}

void NetComm::updateAll()
{
  weatherRequest.setUrl(QUrl("http://api.openweathermap.org/data/2.5/weather?q=" + settings->value("weather_city", "Copenhagen").toString() + "&mode=xml&units=metric&appid=" + settings->value("weather_key", "fe9fe6cf47c03d2640d5063fbfa053a2").toString()));
  feedRequest.setUrl(QUrl(settings->value("feed_url", "http://rss.slashdot.org/Slashdot/slashdotMain").toString()));

  get(feedRequest);
  get(weatherRequest);
}
   
void NetComm::netReply(QNetworkReply *r)
{
  QDomDocument doc;
  doc.setContent(r->readAll());
  r->close();
  if(r->request() == weatherRequest) {
    qInfo("Parsing weather:\n%s\n", doc.toString().toStdString().c_str());

    weather.icon = doc.elementsByTagName("weather").at(0).toElement().attribute("icon");
    weather.windSpeed = doc.elementsByTagName("speed").at(0).toElement().attribute("value").toDouble();
    weather.windDirection = doc.elementsByTagName("direction").at(0).toElement().attribute("code");
    weather.temp = doc.elementsByTagName("temperature").at(0).toElement().attribute("value").toDouble();

    if(weather.icon.isEmpty()) {
      weather.icon = "11d";
    }
    if(weather.temp == 0.0) {
      weather.temp = 66.6;
    }
    
    // Overrule weather if forced from config.ini
    if(settings->contains("weather_force_type")) {
      weather.icon = settings->value("weather_force_type", "11d").toString();
    }
    if(settings->contains("weather_force_temp")) {
      weather.temp = settings->value("weather_force_temp", "20.0").toDouble();
    }
    if(settings->contains("weather_force_wind_speed")) {
      weather.windSpeed = settings->value("weather_force_wind_speed", "0.0").toDouble();
    }
    if(settings->contains("weather_force_wind_direction")) {
      weather.windDirection = settings->value("weather_force_wind_direction", "E").toDouble();
    }
    
    //qInfo("%s\n", rawData.data());
    qInfo("Icon: %s\n", weather.icon.toStdString().c_str());
    qInfo("Temp: %f\n", weather.temp);
    qInfo("Wind: %fm/s from %s\n", weather.windSpeed, weather.windDirection.toStdString().c_str());
    emit weatherUpdated();
  } 
  if(r->request() == feedRequest) {
    chatLines.clear();
    qInfo("Updating feed:\n");
    QDomNodeList titles = doc.elementsByTagName("item");
    for(int a = 0; a < titles.length(); ++a) {
      ChatLine feedLine;
      feedLine.type = "_whisper";
      feedLine.text = titles.at(a).firstChildElement("title").text().trimmed();
      feedLine.url = QUrl(titles.at(a).firstChildElement("link").text());
      qInfo("'%s'\n", feedLine.text.toStdString().c_str());
      chatLines.append(feedLine);
    }
    emit feedUpdated();
  }
  netTimer.start();
  r->deleteLater();
}

Weather NetComm::getWeather()
{
  return weather;
}

QList<ChatLine> NetComm::getFeedLines()
{
  return chatLines;
}
