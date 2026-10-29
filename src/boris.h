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

#include "behaviour.h"
#include "bubble.h"
#include "debugaction.h"
#include "item.h"
#include "settings.h"
#include "shadow.h"
#include "stats.h"
#include "weather.h"

#include <QAction>
#include <QGraphicsObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>
#include <QTimer>

namespace Direction {
  constexpr int None = -1;
  constexpr int North = 0;
  constexpr int NorthEast = 1;
  constexpr int East = 2;
  constexpr int SouthEast = 3;
  constexpr int South = 4;
  constexpr int SouthWest = 5;
  constexpr int West = 6;
  constexpr int NorthWest = 7;
};

class Stats;
class Bubble;
class Weather;
class Item;
class Shadow;

class Boris : public QGraphicsObject
{
  Q_OBJECT;
public:
  explicit Boris(Settings &settings);
  ~Boris();
  Boris *borisFriend;
  Stats *stats = nullptr;
  int getWidth() const;
  int getHeight() const;
  void moveBoris(int dX, int dY, const bool &flipped = false);
  int getCurBehav();
  void changeBehaviour(QString behav = "", int time = -1);
  QPoint getGlobalCenter();
  QMap<QString, int> scriptVars;

  int curFrame = 0;
  int curBehav = 0;

public slots:
  void handlePhysics();
  void earthquake();
  void triggerWeather();
  void nextBehaviour();
  void reconfBoris();
  void createBehavioursMenu();
  void queueBehavFromFile(const QString &file);

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;
  void timerEvent(QTimerEvent *event);

  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void wheelEvent(QGraphicsSceneWheelEvent *) override;

signals:
  void deltaCoins(const int &coins);

private slots:
  void handleBehaviourChange(QAction* a);
  void updateDebugInfo();
  void enableInteract();
  void checkInteractions();
  void behavFromFile(const QString &file);
  void setCurFrame(const int &frame);

private:
  void runScript(int &stop, const bool &init = false);
  void sanityCheck();
  void setWeather(QString &behav);

  Settings &settings;

  bool falling = false;
  bool grabbed = false;
  bool interact = true;
  int staticBehaviours = 0;
  int interactions = 0;
  int annoyance = 0;

  DebugAction *debugAction = nullptr;
  QMenu *behavioursMenu = nullptr;

  QTimer behavTimer;
  QBasicTimer animTimer;
  QTimer interactionsTimer;
  QTimer enableInteractTimer;

  QPoint mLastMousePosition;
  bool mouseHovering = false;

  QPixmap borisSprite;
  QPixmap scriptSprite;
  QImage scriptImage = QImage(32, 32, QImage::Format_ARGB32_Premultiplied);
  bool drawing = false;

  QPixmap dirtSprite;
  QPixmap origDirt;
  qreal dirtOpacity = 0.0;

  QPixmap bruisesSprite;
  QPixmap origBruises;
  qreal bruisesOpacity = 0.0;

  QPixmap rendered = QPixmap(32, 32);

  double vVel = 0.0;
  double hVel = 0.0;
  int groundY = 0;

  Bubble *bubble = nullptr;

  bool showStats = false;

  Weather *weather = nullptr;
  bool showWeather = false;

  Shadow *shadow = nullptr;

  QList<QString> behavQueue;
  QString getFileFromCategory(const QString &category);
  int getIdxFromCategory(const QString &category);
  void processAi(QString &behav);
  int getDistance(const QPointF &p);
  int getDirectionTo(const QPointF &p);
  void borisInteract(Boris *boris);
  void itemInteract(Item *item);
  int timeForWeather = 0;

  bool isAlive = true;
  bool flipFrames = false;

  bool stopNextBehaviour = false;

  void killBoris();
};
