/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            item.cpp
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

#include "item.h"

#include <stdio.h>

#include <QTimer>

Item::Item(const int &x, const int &y, const int &size, const QString &item)
{
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background:transparent");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setScene(new QGraphicsScene);
  scene()->setSceneRect(0.0, 0.0, size, size);

  itemSprite = this->scene()->addPixmap(QPixmap(item));
  itemSprite->setPos(0, 0);

  setCursor(QCursor(QPixmap(":mouse_closet.png"), 15, 16));

  setFixedSize(size, size);
  scale(size / 32.0, size / 32.0);
  move(x, y);
  QTimer::singleShot(300000, this, &Item::close); // Destroy item after 5 minutes
  show();
}

Item::~Item()
{
}

void Item::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton) {
    emit close();
  }
}
