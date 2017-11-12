/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            mainwindow.h
 *
 *  Tue Nov 26 16:56:00 CEST 2013
 *  Copyright 2013 Lars Muldjord
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
#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include "boris.h"
#include "behaviour.h"
#include "netcomm.h"

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>

class MainWindow : public QWidget
{
  Q_OBJECT;
public:
  MainWindow();
  ~MainWindow();

protected:
  void mousePressEvent(QMouseEvent* event);

private slots:
  void aboutBox();
  void checkCollisions();
  void killAll();
  //void weatherReply(QNetworkReply *r);
  void updateWeather();
  void updateChatLines();
  
private:
  NetComm *netComm;
  bool loadBehaviours();
  QList<QString> extractSnippets(QString line);
  QString *aboutText;
  void createTrayIcon();
  void createActions();
  QAction *aboutAction;
  QAction *earthquakeAction;
  QAction *teleportAction;
  QAction *weatherAction;
  QAction *quitAction;
  QSystemTrayIcon *trayIcon;
  QMenu *trayIconMenu;
  QList<Boris*> borises;
  QList<Behaviour> *behaviours;
  QList<Behaviour> *weathers;
  QList<ChatLine> *chatLines;
  int clones;
  void addBoris(int clones);
  void removeBoris(int clones);
  QTimer collisTimer;
  QString weatherIcon;
  double weatherTemp;
  void loadChatter();
  
};

#endif // _MAINWINDOW_H
