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

#include "SDL2/SDL_mixer.h"

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QAction>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>

class Boris : public QGraphicsView
{
  Q_OBJECT;
public:
  Boris(QWidget *parent);
  ~Boris();
  QTimer behavTimer;
  void updateBoris();
  void collide(Boris *b);
  Boris *borisFriend;
  int getHygiene();
  void moveBoris(int dX, int dY);
  void changeBehaviour(QString behav = "", int time = 0);
  int getCurBehav();
  int size = 64;
                                                        
public slots:
  void earthquake();
  void teleport();
  void triggerWeather();
  void nextBehaviour();
  
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
  void nextFrame();
  void nextWeatherFrame();
  void hideWeather();
  void readyForFriend();
  
private:
  bool falling;
  bool grabbed;
  void showWeather(QString &behav);
  void showBoris();
  void hideBoris();
  void createBehavMenu();
  int staticBehavs;
  int independence;
  
  QMenu* bMenu;
  int curFrame;
  int curBehav;
  QTimer animTimer;
  QTimer weatherTimer;
  QTimer physicsTimer;
  QTimer statTimer;
  QTimer statQueueTimer;
  bool mMoving;
  QPoint mLastMousePosition;
  bool alreadyEvading;

  QGraphicsPixmapItem *sprite;
  QGraphicsPixmapItem *dirt;
  QPixmap origDirt;
  QGraphicsPixmapItem *bruises;
  QPixmap origBruises;
  QGraphicsPixmapItem *weatherSprite;
  
  double vVel;
  double hVel;
  double sinVal = 0.0;
  double mouseVVel;
  double mouseHVel;
  int alt;
  QPoint oldCursor;
  void sanityCheck();
  Stats *stats;
  Chatter *chatter;
  QList<QString> behavQueue;
  int hyperQueue;
  int healthQueue;
  int energyQueue;
  int hungerQueue;
  int bladderQueue;
  int socialQueue;
  int funQueue;
  int hygieneQueue;
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
  
};

#endif // _BORIS_H
