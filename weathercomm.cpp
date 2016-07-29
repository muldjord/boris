/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            weathercomm.cpp
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

#include "weathercomm.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QSettings>

//#define DEBUG

// How to get weather
// http://api.openweathermap.org/data/2.5/weather?q=Aarhus&appid=fe9fe6cf47c03d2640d5063fbfa053a2

extern QSettings *settings;

WeatherComm::WeatherComm()
{
  connect(this, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(weatherReply(QNetworkReply*)));

  weatherTimer.setInterval(1800000);
  weatherTimer.setSingleShot(true);
  connect(&weatherTimer, SIGNAL(timeout()), this, SLOT(getWeather()));
  // Timer is started after initial weather update

  QTimer::singleShot(0, this, SLOT(getWeather()));
}

WeatherComm::~WeatherComm()
{
}

void WeatherComm::getWeather()
{
  QString weatherRequest = "http://api.openweathermap.org/data/2.5/weather?q=" + settings->value("weather_city", "Copenhagen").toString() + "&appid=" + settings->value("weather_key", "fe9fe6cf47c03d2640d5063fbfa053a2").toString();
  get(QNetworkRequest(QUrl(weatherRequest)));
  //weatherReply();
}
   
// More info: http://openweathermap.org/weather-conditions
void WeatherComm::weatherReply(QNetworkReply *r)
{
  //QByteArray rawData = "{\"coord\":{\"lon\":10.21,\"lat\":56.16},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04n\"}],\"base\":\"cmc stations\",\"main\":{\"temp\":286.364,\"pressure\":1028.65,\"humidity\":67,\"temp_min\":286.364,\"temp_max\":286.364,\"sea_level\":1032.6,\"grnd_level\":1028.65},\"wind\":{\"speed\":5.62,\"deg\":207.502},\"clouds\":{\"all\":88},\"dt\":1462215751,\"sys\":{\"message\":0.0027,\"country\":\"DK\",\"sunrise\":1462159790,\"sunset\":1462215813},\"id\":2624652,\"name\":\"Arhus\",\"cod\":200}";
  QByteArray rawData = r->readAll();
  r->close();
  qDebug("Parsing weather:\n");

  if(settings->contains("weather_force_type")) {
    weatherIcon = settings->value("weather_force_type", "09d").toString();
  } else {
    weatherIcon = rawData.mid(rawData.indexOf("icon\":\"") + 7, 3);
  }

  if(settings->contains("weather_force_temp")) {
    weatherTemp = settings->value("weather_force_temp", "20.").toDouble();
  } else {
  weatherTemp = rawData.mid(rawData.indexOf("temp\":") + 6, rawData.indexOf(",\"pressure") - (rawData.indexOf("temp\":") + 6)).toDouble() - 273.15;
  }
  
  //qDebug("%s\n", rawData.data());
  qDebug("Icon: %s\n", weatherIcon.toStdString().c_str());
  qDebug("Temp: %f\n", weatherTemp);
  emit weatherUpdated();
  weatherTimer.start();
}

double WeatherComm::getTemp()
{
  return weatherTemp;
}

QString WeatherComm::getIcon()
{
  return weatherIcon;
}
