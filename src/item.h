/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            item.h
 *
 *  Mon Nov 11 12:54:00 CEST 2019
 *  Copyright 2019 Lars Muldjord
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

#include "boris.h"
#include "behaviour.h"
#include "settings.h"

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QTimer>
#include <QBasicTimer>

struct Settings;
class Shadow;
class Boris;

class Item : public QGraphicsObject
{
  Q_OBJECT;
public:
  Item(const QString &item, Settings &settings, const bool &ignore = true);
  ~Item();
  int getWidth() const;
  int getHeight() const;
  void moveItem(int dX, int dY, const bool &flipped = false);
  QPoint getGlobalCenter();
  QString getReactionBehaviour();
  QString getCategory();
  void interact(Boris *boris);
  QMap<QString, int> scriptVars;
  bool grabbed = false;
  bool ignore;

public slots:
  void destroy();
  void handlePhysics();

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;
  void timerEvent(QTimerEvent *event);

  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private slots:
  void setCurFrame(const int &frame);
  void dontIgnore();

private:
  Settings &settings;
  bool flipFrames = false;
  QPixmap itemSprite;
  QBasicTimer animTimer;
  QPixmap scriptSprite;
  QImage scriptImage = QImage(32, 32, QImage::Format_ARGB32_Premultiplied);
  bool drawing = false;
  int curFrame = 0;
  int curItem = 0;

  int borisHyper = 0;
  int interactReaction = -1;

  QPixmap rendered = QPixmap(32, 32);

  Shadow *shadow = nullptr;

  QTimer ignoreTimer;
  double vVel = 0.0;
  double hVel = 0.0;
  int groundY = 0;
  bool falling = false;

  void runScript(int &stop, const bool &init = false);
  void sanityCheck();
  int getDistance(const QPointF &p);
  int getDirectionTo(const QPointF &p);

  bool stopAndDestroy = false;
};
