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

#include "netcomm.h"

#include "settings.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomNodeList>
#include <QNetworkReply>
#include <QUrl>

NetComm::NetComm(Settings &settings) : settings(settings)
{
  connect(this, &NetComm::finished, this, &NetComm::netReply);

  netTimer.setInterval(3600000);
  netTimer.setSingleShot(true);
  connect(&netTimer, &QTimer::timeout, this, &NetComm::updateAll);
}

NetComm::~NetComm()
{
}

void NetComm::updateAll()
{
  if(!settings.city.trimmed().isEmpty() &&
     !settings.key.trimmed().isEmpty()) {
    bool isId = false;
    int cityId = settings.city.toInt(&isId);
    
    if(isId) {
      weatherRequest.setUrl(QUrl("http://api.openweathermap.org/data/2.5/weather?id=" + QString::number(cityId) + "&mode=xml&units=metric&appid=" + settings.key));
    } else {
      weatherRequest.setUrl(QUrl("http://api.openweathermap.org/data/2.5/weather?q=" + settings.city + "&mode=xml&units=metric&appid=" + settings.key));
    }
    get(weatherRequest);
  }

  if(!settings.rssUrl.trimmed().isEmpty()) {
    feedRequest.setUrl(QUrl(settings.rssUrl));
    get(feedRequest);
  } else {
    settings.rssLines.clear();
  }
}

void NetComm::netReply(QNetworkReply *reply)
{
  QDomDocument doc;
  doc.setContent(reply->readAll());
  reply->close();
  if(reply->request() == weatherRequest) {
    qInfo("Setting weather:\n");
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
    qInfo("  Icon: %s\n", qPrintable(settings.weatherType));
    qInfo("  Temp: %f\n", settings.temperature);
    qInfo("  Wind: %fm/s from %s\n", settings.windSpeed, qPrintable(settings.windDirection));

    emit weatherUpdated();
  } else if(reply->request() == feedRequest) {
    settings.rssLines.clear();
    qInfo("Updating feed:\n");
    QDomNodeList titles = doc.elementsByTagName("item");
    for(int a = 0; a < titles.length(); ++a) {
      RssLine rssLine;
      rssLine.text = titles.at(a).firstChildElement("title").text().trimmed();
      rssLine.url = QUrl(titles.at(a).firstChildElement("link").text());
      qInfo("  '%s'\n", qPrintable(rssLine.text));
      settings.rssLines.append(rssLine);
    }
  }
  netTimer.start();
  reply->deleteLater();
}
