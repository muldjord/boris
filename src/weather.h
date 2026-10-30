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

#pragma once

#include "settings.h"

#include <QGraphicsObject>
#include <QPixmap>
#include <QTimer>

class QPainter;
class QRectF;
class QStyleOptionGraphicsItem;
class QWidget;
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
  QPixmap rendered = QPixmap(32, 32);
};
