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

#include "weather.h"

#include <stdio.h>
#include <math.h>
#include <QUuid>
#include <QPainter>

Weather::Weather(Settings &settings)
  : settings(settings)
{
  weatherTimer.setSingleShot(true);
  connect(&weatherTimer, &QTimer::timeout, this, &Weather::updateWeather);
  rendered.fill(Qt::transparent);
}

Weather::~Weather()
{
}

void Weather::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->drawPixmap(0, 0, rendered);
}

QRectF Weather::boundingRect() const
{
  return QRectF(0, 0, rendered.width(), rendered.height());
}

void Weather::resetWeather()
{
  curFrame = 0; 
  weatherTimer.setInterval(0);
  weatherTimer.start();
}

void Weather::updateWeather()
{
  rendered = settings.weathers.at(settings.weatherIdx).frames.at(curFrame).sprite;
  weatherTimer.setInterval(settings.weathers.at(settings.weatherIdx).frames.at(curFrame).time);
  curFrame++;
  if(curFrame >= settings.weathers.at(settings.weatherIdx).frames.length()) {
    curFrame = 0;
  }

  update();

  weatherTimer.start();
}
