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

  int getHyper();
  int getHealth();
  int getEnergy();
  int getHunger();
  int getToilet();
  int getSocial();
  int getFun();
  int getHygiene();
  int getAnxiety();
  void deltaHyper(int value);
  void deltaHealth(int value);
  void deltaEnergy(int value);
  void deltaHunger(int value);
  void deltaToilet(int value);
  void deltaSocial(int value);
  void deltaFun(int value);
  void deltaHygiene(int value);
  void deltaAnxiety(int value);
  void updateStats();
  bool underMouse;

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;

private slots:
  void logStats();

private:
  Settings &settings;

  int flashes;

  void setRect(QRect &rect, qreal x, qreal y, qreal width, qreal height);

  QRect energyRect;
  QRect hungerRect;
  QRect toiletRect;
  QRect socialRect;
  QRect funRect;
  QPixmap flashIcon;
  QPoint flashIconPos = QPoint(0, 0);
  bool showFlashIcon = false;

  QPixmap statsPixmap;
  QPixmap rendered;

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
