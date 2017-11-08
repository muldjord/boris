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

// How to get weather
// http://api.openweathermap.org/data/2.5/weather?q=Aarhus&appid=fe9fe6cf47c03d2640d5063fbfa053a2

extern QSettings *settings;

NetComm::NetComm()
{
  connect(this, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(netReply(QNetworkReply*)));

  netTimer.setInterval(3600000);
  netTimer.setSingleShot(true);
  connect(&netTimer, SIGNAL(timeout()), this, SLOT(updateAll()));

  QTimer::singleShot(0, this, SLOT(updateAll()));
}

NetComm::~NetComm()
{
}

void NetComm::updateAll()
{
  weatherRequest.setUrl(QUrl("http://api.openweathermap.org/data/2.5/weather?q=" + settings->value("weather_city", "Copenhagen").toString() + "&appid=" + settings->value("weather_key", "fe9fe6cf47c03d2640d5063fbfa053a2").toString()));
  feedRequest.setUrl(QUrl(settings->value("feed_url", "http://rss.slashdot.org/Slashdot/slashdotMain").toString()));

  get(feedRequest);
  get(weatherRequest);
}
   
void NetComm::netReply(QNetworkReply *r)
{
  QByteArray rawData = r->readAll();
  r->close();
  if(r->request() == weatherRequest) {
    qInfo("Parsing weather:\n");

    if(!rawData.contains("Error")) {
      weatherIcon = rawData.mid(rawData.indexOf("icon\":\"") + 7, 3);
      weatherTemp = rawData.mid(rawData.indexOf("temp\":") + 6, rawData.indexOf(",\"pressure") - (rawData.indexOf("temp\":") + 6)).toDouble() - 273.15;
    } else {
      weatherIcon = "11d";
      weatherTemp = 66.6;
    }
    
    // Overrule weather if forced from config.ini
    if(settings->contains("weather_force_type")) {
      weatherIcon = settings->value("weather_force_type", "11d").toString();
    }
    if(settings->contains("weather_force_temp")) {
      weatherTemp = settings->value("weather_force_temp", "20.0").toDouble();
    }
    
    //qDebug("%s\n", rawData.data());
    qInfo("Icon: %s\n", weatherIcon.toStdString().c_str());
    qInfo("Temp: %f\n", weatherTemp);
    emit weatherUpdated();
  } 
  if(r->request() == feedRequest) {
    feedLines.clear();
    qInfo("Updating feed:\n");
    QDomDocument doc;
    doc.setContent(rawData, false);
    QDomNodeList titles = doc.elementsByTagName("item");
    for(int a = 0; a < titles.length(); ++a) {
      QString feedLine = titles.at(a).firstChildElement("title").text();
      qInfo("'%s'\n", feedLine.toStdString().c_str());
      feedLines.append(feedLine);
    }
    emit feedUpdated();
  }
  netTimer.start();
  r->deleteLater();
}

double NetComm::getTemp()
{
  return weatherTemp;
}

QString NetComm::getIcon()
{
  return weatherIcon;
}

QList<QString> NetComm::getFeedLines()
{
  return feedLines;
}
