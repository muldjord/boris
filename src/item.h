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
#ifndef _ITEM_H
#define _ITEM_H

#include "behaviour.h"
#include "settings.h"

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QTimer>

class Item : public QGraphicsView
{
  Q_OBJECT;
public:
  Item(const int &x, const int &y, const int &size, const QString &item, Settings *settings);
  ~Item();
  void moveItem(int dX, int dY, const bool &flipped = false);
  QMap<QString, int> scriptVars;

protected:
  void mousePressEvent(QMouseEvent* event);

private slots:
  void nextFrame();
  QPixmap getShadow(const QPixmap &sprite);
  void setCurFrame(const int &frame);
  void destroy();
  
private:
  Settings *settings;
  bool flipFrames = false;
  QGraphicsPixmapItem *itemSprite;
  QGraphicsPixmapItem *shadowSprite;
  QPixmap origShadow;
  QTimer animTimer;
  QGraphicsPixmapItem *scriptSprite;
  QImage scriptImage = QImage(32, 32, QImage::Format_ARGB32_Premultiplied);
  bool drawing = false;
  int curFrame = 0;
  int curItem = 0;
  int size = 64;
  void runScript(int &stop);
  void sanityCheck();
  int getDistance(const QPoint &p);
  int getSector(const QPoint &p);

  bool stopAndDestroy = false;
};

#endif // _ITEM_H
