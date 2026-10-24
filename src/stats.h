/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            stats.h
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

#pragma once

#include "settings.h"

#include <QGraphicsPixmapItem>
#include <QRect>
#include <QFile>
#include <QTimer>

class Settings;

class Stats : public QGraphicsObject
{
public:
  Stats(Settings &settings,
        const int &hyper,
        const int &health,
        const int &energy,
        const int &hunger,
        const int &toilet,
        const int &social,
        const int &fun,
        const int &hygiene,
        const int &anxiety);
  ~Stats();
  void flashStat(QString stat = "none");
  bool showStats = false;

  int getEnergy();
  int getSocial();
  int getFun();
  int getHunger();
  int getToilet();

  int getHyper();
  int getHealth();
  int getHygiene();
  int getAnxiety();

  void deltaEnergy(int value);
  void deltaSocial(int value);
  void deltaFun(int value);
  void deltaHunger(int value);
  void deltaToilet(int value);

  void deltaHyper(int value);
  void deltaHealth(int value);
  void deltaHygiene(int value);
  void deltaAnxiety(int value);

  void updateStats();
  bool underMouse;

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;

private:
  QColor getBarColor(const int &value, const bool &inverse = false);
  Settings &settings;

  int flashes = 0;

  QColor energyColor = QColor(0, 0, 0);
  QColor socialColor = QColor(0, 0, 0);
  QColor funColor = QColor(0, 0, 0);
  QColor hungerColor = QColor(0, 0, 0);
  QColor toiletColor = QColor(0, 0, 0);

  QPixmap flashIcon;
  QPoint flashIconPos = QPoint(0, 0);
  bool showFlashIcon = false;

  QPixmap statsPixmap;

  QPixmap rendered = QPixmap(80, 80); // +1 for the shadow

  bool visibility;
  int hyper;
  int health;
  int energy;
  int hunger;
  int toilet;
  int social;
  int fun;
  int hygiene;
  int anxiety;

  QTimer statTimer;

};
