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
#include "settings.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QFile>
#include <QDomDocument>

extern Settings settings;

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
  weatherRequest.setUrl(QUrl("http://api.openweathermap.org/data/2.5/weather?q=" + settings.city + "&mode=xml&units=metric&appid=" + settings.key));
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
    qDebug("%s\n", doc.toString().toStdString().c_str());

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
    qInfo("Icon: %s\n", settings.weatherType.toStdString().c_str());
    qInfo("Temp: %f\n", settings.temperature);
    qInfo("Wind: %fm/s from %s\n", settings.windSpeed, settings.windDirection.toStdString().c_str());

    emit weatherUpdated();
  } else if(r->request() == feedRequest) {
    settings.chatLines.clear();
    qInfo("Updating feed:\n");
    qDebug("%s\n", doc.toString().toStdString().c_str());
    QDomNodeList titles = doc.elementsByTagName("item");
    for(int a = 0; a < titles.length(); ++a) {
      ChatLine feedLine;
      feedLine.type = "_whisper";
      feedLine.text = titles.at(a).firstChildElement("title").text().trimmed();
      feedLine.url = QUrl(titles.at(a).firstChildElement("link").text());
      qInfo("'%s'\n", feedLine.text.toStdString().c_str());
      settings.chatLines.append(feedLine);
    }

    QFile chatFile(settings.chatFile);
    if(chatFile.open(QIODevice::ReadOnly)) {
      do {
        QList<QString> snippets = QString(chatFile.readLine()).split(";");
        ChatLine chatLine;
        chatLine.type = snippets.at(0);
        chatLine.text = snippets.at(1).simplified();
        settings.chatLines.append(chatLine);
      } while(chatFile.canReadLine());
    }
    chatFile.close();
  }
  netTimer.start();
  r->deleteLater();
}
