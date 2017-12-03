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

#ifndef _STATS_H
#define _STATS_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QFile>
#include <QTimer>

class Stats : public QGraphicsView
{
  Q_OBJECT;
public:
  Stats(int health, int energy, int hunger, int bladder, int social, int fun, int hygiene, QWidget *parent);
  ~Stats();
  void flashStat(QString stat = "none");
  int getHealth();
  int getEnergy();
  int getHunger();
  int getBladder();
  int getSocial();
  int getFun();
  int getHygiene();
  void deltaHealth(int value);
  void deltaEnergy(int value);
  void deltaHunger(int value);
  void deltaBladder(int value);
  void deltaSocial(int value);
  void deltaFun(int value);
  void deltaHygiene(int value);
  void updateStats();
  bool underMouse;
                    
private slots:
  void logStats();

private:
  int flashes;
  QImage image;
  QGraphicsPixmapItem *sprite;
  QGraphicsRectItem *energyBar;
  QGraphicsRectItem *hungerBar;
  QGraphicsRectItem *bladderBar;
  QGraphicsRectItem *socialBar;
  QGraphicsRectItem *funBar;
  QGraphicsPixmapItem *flashIcon;
  bool visibility;
  int health;
  int energy;
  int hunger;
  int bladder;
  int social;
  int fun;
  int hygiene;
  
  QFile statLog;
  QTimer statTimer;
  
};

#endif // _STATS_H
