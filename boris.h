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

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QSoundEffect>
#include <QAction>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>

class Boris : public QGraphicsView
{
  Q_OBJECT;
public:
  Boris(QList<Behaviour> *behaviours, QWidget *parent);
  ~Boris();
  QTimer behavTimer;
  QList<Behaviour> *behaviours;
  void changeSize(int newSize);
  void soundEnable(bool enabled);
  void statsEnable(bool enabled);
  void setIndependence(int value);
                                
public slots:
  void nextFrame();
  void earthquake();
  void walkUp();
  void walkDown();
  void walkLeft();
  void walkRight();
  
signals:
  void _moveBoris(int dx, int dy);
  void _showBoris();
  void _hideBoris();

protected:
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void enterEvent(QEvent *event);
  void leaveEvent(QEvent *event);

private slots:
  void moveBoris(int dX, int dY);
  void showBoris();
  void hideBoris();
  void handleBehaviourChange(QAction* a);
  void handlePhysics();
  void changeBehaviour(QString behav = "", int time = 0);
  void statProgress();

private:
  int staticBehavs;
  int independence;
  
  int energy;
  int hunger;
  int bladder;
  int hygiene;
  int social;
  int fun;

  QMenu* bMenu;
  int borisSize;
  bool soundEnabled;
  int curFrame;
  int curBehav;
  QTimer animTimer;
  QTimer physicsTimer;
  QTimer statTimer;
  bool mMoving;
  QPoint mLastMousePosition;
  bool alreadyEvading;
  QGraphicsPixmapItem *sprite;
  double vVel;
  double hVel;
  int alt;
  bool falling;
  QPoint oldCursor;
  bool grabbed;
  void sanityCheck();
  Stats *stats;
  bool showStats;
  QList<QString> behavQueue;
  QString chooseFromCategory(QString category);
  
};

#endif // _BORIS_H

// behaviours.at(1).
