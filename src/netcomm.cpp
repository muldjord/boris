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
#include <QFile>
#include <QDomDocument>

NetComm::NetComm(Settings &settings) : settings(settings)
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
  if(settings.city.toInt()) {
    weatherRequest.setUrl(QUrl("http://api.openweathermap.org/data/2.5/weather?id=" + settings.city + "&mode=xml&units=metric&appid=" + settings.key));
  } else {
    weatherRequest.setUrl(QUrl("http://api.openweathermap.org/data/2.5/weather?q=" + settings.city + "&mode=xml&units=metric&appid=" + settings.key));
  }
  get(weatherRequest);
  feedRequest.setUrl(QUrl(settings.feedUrl));
  get(feedRequest);
}

void NetComm::netReply(QNetworkReply *r)
{
  QDomDocument doc;
  doc.setContent(r->readAll());
  r->close();
  if(r->request() == weatherRequest) {
    qInfo("Updating weather:\n");
    if(!settings.forceWeatherType) {
      settings.weatherType = doc.elementsByTagName("weather").at(0).toElement().attribute("icon");
    }
    if(!settings.forceWindSpeed) {
      settings.windSpeed = doc.elementsByTagName("speed").at(0).toElement().attribute("value").toDouble();
    }
    if(!settings.forceWindDirection) {
      settings.windDirection = doc.elementsByTagName("direction").at(0).toElement().attribute("code");
    }
    if(!settings.forceTemperature) {
      settings.temperature = doc.elementsByTagName("temperature").at(0).toElement().attribute("value").toDouble();
    }

    if(settings.weatherType.isEmpty()) {
      settings.weatherType = "11d";
    }
    if(settings.temperature == 0.0) {
      settings.temperature = -42;
    }
    if(settings.windDirection.isEmpty()) {
      settings.windDirection = "E";
    }

    //qInfo("%s\n", rawData.data());
    qInfo("  Icon: %s\n", settings.weatherType.toStdString().c_str());
    qInfo("  Temp: %f\n", settings.temperature);
    qInfo("  Wind: %fm/s from %s\n", settings.windSpeed, settings.windDirection.toStdString().c_str());

    emit weatherUpdated();
  } else if(r->request() == feedRequest) {
    settings.rssLines.clear();
    qInfo("Updating feed:\n");
    QDomNodeList titles = doc.elementsByTagName("item");
    for(int a = 0; a < titles.length(); ++a) {
      RssLine rssLine;
      rssLine.text = titles.at(a).firstChildElement("title").text().trimmed();
      rssLine.url = QUrl(titles.at(a).firstChildElement("link").text());
      qInfo("  '%s'\n", rssLine.text.toStdString().c_str());
      settings.rssLines.append(rssLine);
    }
  }
  netTimer.start();
  r->deleteLater();
}
