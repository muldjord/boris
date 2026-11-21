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

#include <QAction>
#include <QElapsedTimer>
#include <QGraphicsView>
#include <QTimer>

class NetComm;
class QIcon;
class QKeyEvent;
class QMenu;
class QResizeEvent;
class QSettings;
class QSplashScreen;
class QString;
class QSystemTrayIcon;

class MainWindow : public QGraphicsView
{
  Q_OBJECT;
public:
  MainWindow(QSplashScreen &splash);
  ~MainWindow();
  qint64 getElapsed();
  int getKeyDirection();
  QTimer tickTimer;

protected:
  void resizeEvent(QResizeEvent *) override;
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  
signals:
  void reconfBoris();
  void queueBehavFromFile(const QString &file);
  //void handlePhysics();

private slots:
  void init();
  void tick();
  void handlePhysics();
  void aboutBox();
  void killAll();
  void updateTrayWeather();
  void triggerBehaviour(QAction *action);
  void spawnRandomItem();
  void spawnItem(QAction *action);
  void deltaCoins(const int &coins);

private:
  void loadFromConfig();
  void stall(const int &ms);
  void resetView();
  QList<QString> extractSnippets(QString line);
  void createTrayIcon();
  void createBehavioursMenu();
  void createItemsMenu();
  void unlockItem(QAction *action);
  void createActions();
  void addBoris(int clones);
  void removeBoris(int clones);

  QSettings *iniSettings = nullptr;
  Settings settings;
  QIcon *icon = nullptr;
  QIcon *coinIcon = nullptr;

  qint64 physicsTime = 0;
  qint64 physicsTimeMax = 30;
  bool physicsTimer = true;

  qint64 itemSpawnTime = 0;
  qint64 itemSpawnTimeMax = 3600 * 1000;
  bool itemSpawnTimer = true;

  qint64 oldElapsed = 0;
  
  QElapsedTimer elapsedTimer;

  QMenu *behavioursMenu = nullptr;
  QMenu *itemsMenu = nullptr;
  NetComm *netComm = nullptr;
  QAction *earthquakeAction = nullptr;
  QAction *weatherAction = nullptr;

  QPoint oldCursor = QPoint(0, 0);

  QAction *coinsAction = nullptr;
  QSystemTrayIcon *trayIcon = nullptr;
  QMenu *trayIconMenu = nullptr;

  QMap<int, bool> pressedKeys;
  int keyDirection = -1;
};
