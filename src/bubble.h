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
