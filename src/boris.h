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

//#include <QBasicTimer>
#include <QGraphicsObject>
#include <QImage>
#include <QList>
#include <QMap>
#include <QPoint>
#include <QTimer>

class Boris;
class Bubble;
class DebugAction;
class Item;
class QAction;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QMenu;
class QPainter;
class QPointF;
class QString;
class QStyleOptionGraphicsItem;
class QTimerEvent;
class QWidget;
class Settings;
class Shadow;
class Stats;
class Weather;

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

class Boris : public QGraphicsObject
{
  Q_OBJECT;
public:
  explicit Boris(Settings &settings);
  ~Boris();
  int getWidth() const;
  int getHeight() const;
  void moveBoris(int dX, int dY, const bool &flipped = false);
  int getCurBehav();
  void changeBehaviour(QString behav = "", int time = -1);
  QPoint getGlobalCenter();
  QMap<QString, int> scriptVars;

  int curFrame = 0;
  int curBehav = 0;

  QList<Boris *> borisFriends;
  Stats *stats = nullptr;

public slots:
  void tick();
  void earthquake();
  void triggerWeather();
  void nextBehaviour();
  void reconfBoris();
  void createBehavioursMenu();
  void queueBehavFromFile(const QString &file);

protected:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
  QRectF boundingRect() const override;
  //void timerEvent(QTimerEvent *event);

  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
  void wheelEvent(QGraphicsSceneWheelEvent *) override;

signals:
  void deltaCoins(const int &coins);

private slots:
  void nextFrame();
  void handleBehaviourChange(QAction* a);
  void updateDebugInfo();
  void checkInteractions();
  void behavFromFile(const QString &file);
  void setCurFrame(const int &frame);

private:
  void handlePhysics();
  QString getFileFromCategory(const QString &category);
  int getIdxFromCategory(const QString &category);
  void processAi(QString &behav);
  int getDistance(const QPointF &p);
  int getDirectionTo(const QPointF &p);
  void borisInteract(Boris *boris);
  void itemInteract(Item *item);
  void killBoris();
  void runScript(int &stop, const bool &init = false);
  void sanityCheck();
  void setWeather(QString &behav);

  Settings &settings;

  QList<Item *> interactingItems;
  QPoint interactingItemPos = QPoint(); // This HAS to be separate from interactingItem due to items often getting deleted causing us to then ask for pos from a deleted object... crash!

  bool falling = false;
  bool grabbed = false;
  int staticBehaviours = 0;
  int interactions = 0;
  int annoyance = 0;

  DebugAction *debugAction = nullptr;
  QMenu *behavioursMenu = nullptr;

  // Tick timers
  qint64 behavTime = 0;
  qint64 behavTimeMax = 0;
  bool behavTimer = true;

  qint64 frameTime = 0;
  qint64 frameTimeMax = 0;
  bool frameTimer = true;

  qint64 interactionsTime = 0;
  qint64 interactionsTimeMax = 150;
  bool interactionsTimer = true;

  qint64 physicsTime = 0;
  qint64 physicsTimeMax = 30;
  bool physicsTimer = true;

  qint64 acknowledgeItemTime = 0;
  qint64 acknowledgeItemTimeMax = 90 * 1000; // This timeout can be quite long as it simply clears the list of all interacted items to make Boris interact with them again
  bool acknowledgeItemTimer = true;

  qint64 acknowledgeBorisTime = 0;
  qint64 acknowledgeBorisTimeMax = 90 * 1000; // This timeout can be quite long as it simply clears the list of all interacted Borises to make Boris interact with them again
  bool acknowledgeBorisTimer = true;

  qint64 oldElapsed = 0;
  //QTimer behavTimer;
  //QBasicTimer animTimer;

  //QTimer interactionsTimer;
  //QTimer acknowledgeBorisesTimer;
  //QTimer acknowledgeItemsTimer;

  QPoint mLastMousePosition;
  bool mouseHovering = false;

  QPixmap borisSprite = QPixmap(32, 32);
  QPixmap scriptSprite = QPixmap(32, 32);
  QImage scriptImage = QImage(32, 32, QImage::Format_ARGB32_Premultiplied);
  bool drawing = false;

  QPixmap dirtSprite = QPixmap(32, 32);;
  qreal dirtOpacity = 0.0;

  QPixmap bruisesSprite = QPixmap(32, 32);;
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
  int timeForWeather = 0;

  bool isAlive = true;
  bool flipFrames = false;

  bool breakToNextBehaviour = false;
};
