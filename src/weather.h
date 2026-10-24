/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            weather.h
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

#pragma once

#include "settings.h"

#include <QGraphicsPixmapItem>
#include <QRect>
#include <QFile>
#include <QTimer>

class Settings;

class Weather : public QGraphicsObject
{
public:
  Weather(Settings &settings);
  ~Weather();
  void resetWeather();

public slots:
  void updateWeather();

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;

private:
  Settings &settings;
  QTimer weatherTimer;
  int curFrame = 0;
  QPixmap weatherPixmap = QPixmap(32, 32);
  QPixmap rendered = QPixmap(32, 32);
};
