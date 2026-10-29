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

#include "boris.h"
#include "behaviour.h"
#include "netcomm.h"
#include "settings.h"

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QSettings>
#include <QTimer>
#include <QSplashScreen>

class MainWindow : public QGraphicsView
{
  Q_OBJECT;
public:
  MainWindow(QSplashScreen &splash);
  ~MainWindow();

protected:
  void resizeEvent(QResizeEvent *event) override;

signals:
  void reconfBoris();
  void queueBehavFromFile(const QString &file);
  void handlePhysics();

private slots:
  void init();
  void progressPhysics();
  void aboutBox();
  void killAll();
  void updateWeather();
  void triggerBehaviour(QAction *a);
  void spawnRandomItem();
  void spawnItem(QAction *a);
  void deltaCoins(const int &coins);

private:
  void loadFromConfig();
  void stall(const int &ms);
  void resetView();
  QSettings *iniSettings = nullptr;
  Settings settings;
  QIcon *icon = nullptr;
  QIcon *coinIcon = nullptr;
  QTimer itemTimer;
  QTimer physicsTimer;
  QMenu *behavioursMenu;
  QMenu *itemsMenu;
  NetComm *netComm;
  QList<QString> extractSnippets(QString line);
  QString *aboutText;
  void createTrayIcon();
  void createBehavioursMenu();
  void createItemsMenu();
  void unlockItem(QAction *action);
  void createActions();
  QAction *earthquakeAction;
  //QAction *behavioursAction;
  QAction *weatherAction;
  int coins = 0;

  QPoint oldCursor;

  QAction *coinsAction;
  QSystemTrayIcon *trayIcon;
  QMenu *trayIconMenu;
  void addBoris(int clones);
  void removeBoris(int clones);
};
