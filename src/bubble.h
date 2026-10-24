/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            bubble.h
 *
 *  Thu Oct 27 18:47:00 CEST 2016
 *  Copyright 2016 Lars Muldjord
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

#ifndef _BUBBLE_H
#define _BUBBLE_H

#include "settings.h"

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QUrl>
#include <QTimer>

class Settings;

class Bubble : public QGraphicsObject
{
  Q_OBJECT;

public:
  explicit Bubble(Settings &settings);
  ~Bubble();
  void initBubble(const int &hyper,
                  const QString &text,
                  const QString &type = "_chat",
                  const QUrl &rssUrl = QUrl());
  bool showBubble = false;

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;

private slots:
  void hideBubble();
  
private:
  Settings &settings;
  QPixmap bubbleTopLeft;
  QPixmap bubbleTop;
  QPixmap bubbleTopRight;
  QPixmap bubbleLeft;
  QPixmap bubbleRight;
  QPixmap bubbleBottomLeft;
  QPixmap bubbleBottom;
  QPixmap bubbleBottomRight;

  QPixmap bubbleThought;
  QPixmap bubbleSpeech;

  QPixmap rendered;

  QTimer hideTimer;
  
  QUrl rssUrl = QUrl();
};

#endif // _BUBBLE_H
