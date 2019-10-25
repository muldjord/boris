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
#include "chatter.h"
#include "settings.h"

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QAction>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>

namespace Direction {
  constexpr int None = -1;
  constexpr int East = 0;
  constexpr int NorthEast = 45;
  constexpr int North = 90;
  constexpr int NorthWest = 135;
  constexpr int West = 180;
  constexpr int SouthWest = 225;
  constexpr int South = 270;
  constexpr int SouthEast = 315;
};

class Boris : public QGraphicsView
{
  Q_OBJECT;
public:
  Boris(Settings *settings);
  ~Boris();
  QTimer behavTimer;
  void collide(Boris *b);
  Boris *borisFriend;
  int getHygiene();
  void moveBoris(int dX, int dY, const bool &flipped = false, const bool &vision = true);
  void changeBehaviour(QString behav = "", int time = 0);
  int getCurBehav();
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
  int curFrame;
  int curBehav;
                                                        
public slots:
  void earthquake();
  void teleport();
  void triggerWeather();
  void nextBehaviour();
  void updateBoris();
  
protected:
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void enterEvent(QEvent *event);
  void leaveEvent(QEvent *event);

signals:
  void playSound(const sf::SoundBuffer *buffer,
                 const float &panning,
                 const float &pitch);

private slots:
  void handleBehaviourChange(QAction* a);
  void handlePhysics();
  void statProgress();
  void statQueueProgress();
  QPixmap getShadow(const QPixmap &sprite);
  void nextFrame();
  void nextWeatherFrame();
  void hideWeather();
  void readyForFriend();
  
private:
  Settings *settings;

  bool falling;
  bool grabbed;
  void showWeather(QString &behav);
  void runScript();
  void createBehavMenu();
  int staticBehavs;
  int independence;
  
  QMenu* bMenu;
  QTimer animTimer;
  QTimer weatherTimer;
  QTimer physicsTimer;
  QTimer statTimer;
  QTimer statQueueTimer;
  bool mMoving;
  QPoint mLastMousePosition;
  bool alreadyEvading;

  QGraphicsPixmapItem *shadowSprite;
  QPixmap origShadow;
  QGraphicsPixmapItem *borisSprite;
  QGraphicsPixmapItem *scriptSprite;
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
  Chatter *chatter;
  QList<QString> behavQueue;
  QString getFileFromCategory(QString category);
  int getIdxFromCategory(QString category);
  void processVision();
  void processAi(QString &behav, int &time);
  int curWeather;
  int curWeatherFrame;
  int timeForWeather;

  bool isAlive;
  bool flipFrames;
  int tooLateForLoo;

  void killBoris();

  QList<QString> scriptInstructions;
  
};

#endif // _BORIS_H
