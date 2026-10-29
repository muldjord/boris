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

#include <QGraphicsPixmapItem>
#include <QRect>
#include <QFile>
#include <QTimer>

class Settings;

enum StatType
{
  NONE = -1,
  ENERGY = 0,
  SOCIAL = 1,
  FUN = 2,
  HUNGER = 3,
  TOILET = 4,
  ANXIETY = 5,
  HEALTH = 6,
  HYGIENE = 7,
  HYPER = 8
};

class Stats : public QGraphicsObject
{
public:
  Stats(Settings &settings,
        const int &energy,
        const int &social,
        const int &fun,
        const int &hunger,
        const int &toilet,
        const int &anxiety,
        const int &health,
        const int &hygiene,
        const int &hyper,
        const int &independence);
  ~Stats();
  void flashStat(QString stat = "none");
  void stopTimers();
  void updateStats();

  // Getters for main stats
  int getEnergy();
  int getSocial();
  int getFun();
  int getHunger();
  int getToilet();

  // Getters for visual and reaction only stats
  int getAnxiety();
  int getHealth();
  int getHygiene();
  int getHyper();

  // Getters for other stats
  int getIndependence();

  // Setters 
  void setStat(const StatType &stat, const int &value); // Set stat to exact value instantly
  void setIndependence(const int &value);

  void deltaStat(const StatType &stat, const int &value); // Change stat with delta of value instantly
  
  bool showStats = false;
  bool underMouse = false;

public slots:
  void queueStatChange(const StatType &stat, const int &value); // Queue stat change that will be added over time
  
protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;

private slots:
  void statProgress();
  void statQueueProgress();

private:
  QColor getBarColor(const int &value, const bool &inverse = false);
  Settings &settings;

  QTimer statProgressTimer;
  QTimer statQueueTimer;

  QColor energyColor = QColor(0, 0, 0);
  QColor socialColor = QColor(0, 0, 0);
  QColor funColor = QColor(0, 0, 0);
  QColor hungerColor = QColor(0, 0, 0);
  QColor toiletColor = QColor(0, 0, 0);

  QPixmap flashIcon;
  QPoint flashIconPos = QPoint(0, 0);
  bool showFlashIcon = false;
  int flashes = 0;

  QPixmap statsPixmap;

  QPixmap rendered = QPixmap(80, 80); // +1 for the shadow

  // Stats
  int energy = 0;
  int social = 0;
  int fun = 0;
  int hunger = 0;
  int toilet = 0;

  int anxiety = 0;
  int health = 0;
  int hygiene = 0;
  int hyper = 0;
  int independence = 0;

  // Queued stat changes
  int energyQueue = 0;
  int socialQueue = 0;
  int funQueue = 0;
  int hungerQueue = 0;
  int toiletQueue = 0;

  int anxietyQueue = 0;
  int healthQueue = 0;
  int hygieneQueue = 0;
  int hyperQueue = 0;
  
  QTimer statTimer;
};
