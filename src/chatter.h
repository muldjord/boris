/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            chatter.h
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

#ifndef _CHATTER_H
#define _CHATTER_H

#include "settings.h"

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QUrl>

class Chatter : public QGraphicsView
{
  Q_OBJECT;

public:
  Chatter(Settings *settings);
  ~Chatter();
  void initChatter(const int x, const int y,
                   const int &borisSize,
                   const QString &bubbleText,
                   const QString &bubbleType,
                   const QUrl &rssUrl = QUrl());
  void moveChatter(const int x, const int y, const int &borisSize);
  
protected:
  void mousePressEvent(QMouseEvent *event);
  
private:
  Settings *settings;
  QGraphicsPixmapItem *bubbleSprite;
  QUrl rssUrl = QUrl();
};

#endif // _CHATTER_H
