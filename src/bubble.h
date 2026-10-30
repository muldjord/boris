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

#include <QGraphicsObject>
#include <QPixmap>
#include <QTimer>
#include <QUrl>

class QGraphicsSceneMouseEvent;
class QPainter;
class QRectF;
class QString;
class QStyleOptionGraphicsItem;
class QWidget;
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
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
  Settings &settings;
  QPixmap bubbleTopLeft = QPixmap(6, 6);
  QPixmap bubbleTop = QPixmap(1, 6);
  QPixmap bubbleTopRight = QPixmap(6, 6);
  QPixmap bubbleLeft = QPixmap(6, 6);
  QPixmap bubbleRight = QPixmap(6, 6);
  QPixmap bubbleBottomLeft = QPixmap(6, 6);
  QPixmap bubbleBottom = QPixmap(1, 6);
  QPixmap bubbleBottomRight = QPixmap(6, 6);

  QPixmap bubbleThought = QPixmap(6, 6);
  QPixmap bubbleSpeech = QPixmap(6, 6);

  QPixmap rendered = QPixmap(32, 24); // Just an approximate it's not really important as it's quickly set to something else

  QTimer hideTimer;

  QUrl rssUrl = QUrl();
};
