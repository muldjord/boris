/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            boris.h
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
#ifndef _BORIS_H
#define _BORIS_H

#include "behaviour.h"
#include "stats.h"
#include "bubble.h"
#include "settings.h"
#include "item.h"

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QBasicTimer>
#include <QAction>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>

namespace Direction {
  constexpr int None = -1;
  constexpr int North = 0;
  constexpr int NorthEast = 1;
  constexpr int East = 2;
  constexpr int SouthEast = 3;
  constexpr int South = 4;
  constexpr int SouthWest = 5;
  constexpr int West = 6;
  constexpr int NorthWest = 7;
};

class Boris : public QGraphicsView
{
  Q_OBJECT;
public:
  Boris(Settings *settings);
  ~Boris();
  void collide(Boris *boris);
  Boris *borisFriend;
  int getHygiene();
  void moveBoris(int dX, int dY, const bool &flipped = false, const bool &vision = true);
  int getCurBehav();
  void changeBehaviour(QString behav = "", int time = -1);
  QPoint getGlobalCenter();
  int size = 64;
  QMap<QString, int> scriptVars;
  int hyperQueue;
  int healthQueue;
  int energyQueue;
  int hungerQueue;
  int bladderQueue;
  int socialQueue;
  int funQueue;
  int hygieneQueue;
  int curFrame = 0;
  int curBehav = 0;
                                                        
public slots:
  void earthquake();
  void teleport();
  void triggerWeather();
  void nextBehaviour();
  void updateBoris();
  void queueBehavFromFile(const QString &file);
  
protected:
  void timerEvent(QTimerEvent *event);
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void enterEvent(QEvent *event);
  void leaveEvent(QEvent *event);
  void wheelEvent(QWheelEvent *);

private slots:
  void handleBehaviourChange(QAction* a);
  void handlePhysics();
  void statProgress();
  void statQueueProgress();
  QPixmap getShadow(const QPixmap &sprite);
  void nextWeatherFrame();
  void hideWeather();
  void readyForFriend();
  void checkInteractions();
  void statChange(const QString &type, const int &amount);
  void behavFromFile(const QString &file);
  void setCurFrame(const int &frame);
  
private:
  Settings *settings;

  bool falling = false;
  bool grabbed = false;
  void showWeather(QString &behav);
  void runScript(int &stop, const bool &init = false);
  void createBehavMenu();
  int staticBehavs;
  int independence;
  
  QMenu* behavioursMenu;
  QTimer behavTimer;
  QBasicTimer animTimer;
  QTimer weatherTimer;
  QTimer physicsTimer;
  QTimer statTimer;
  QTimer statQueueTimer;
  QTimer interactionsTimer;
  int interactions = 0;
  int annoyance = 0;
  bool mMoving;
  QPoint mLastMousePosition;
  bool mouseHovering = false;

  QGraphicsPixmapItem *shadowSprite;
  QPixmap origShadow;
  QGraphicsPixmapItem *borisSprite;
  QGraphicsPixmapItem *scriptSprite;
  QImage scriptImage = QImage(32, 32, QImage::Format_ARGB32_Premultiplied);
  bool drawing = false;
  QGraphicsPixmapItem *dirtSprite;
  QPixmap origDirt;
  QGraphicsPixmapItem *bruisesSprite;
  QPixmap origBruises;
  QGraphicsPixmapItem *weatherSprite;
  
  double vVel = 0.0;
  double hVel = 0.0;
  double sinVal = 0.0;
  double mouseVVel = 0.0;
  double mouseHVel = 0.0;
  int alt;
  QPoint oldCursor;
  void sanityCheck();
  Stats *stats;
  Bubble *bubble;
  QList<QString> behavQueue;
  QString getFileFromCategory(QString category);
  int getIdxFromCategory(QString category);
  void processVision();
  void processAi(QString &behav);
  int getDistance(const QPoint &p);
  int getSector(const QPoint &p);
  void itemInteract(Item *item);
  int curWeather = 0;
  int curWeatherFrame = 0;
  int timeForWeather = 0;

  bool isAlive = true;
  bool flipFrames = false;

  bool stopNextBehaviour = false;

  void killBoris();
};

#endif // _BORIS_H
