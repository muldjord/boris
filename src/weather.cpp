/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            weather.cpp
 *
 *  Sat Oct 24 16:56:00 CEST 2026
 *  Copyright 2026 Lars Muldjord
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
