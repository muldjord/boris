/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            scripthandler.h
 *
 *  Fri Oct 11 20:00:00 CEST 2019
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
#ifndef _SCRIPTHANDLER_H
#define _SCRIPTHANDLER_H

#include "boris.h"
#include "settings.h"
#include "stats.h"
#include "behaviour.h"
#include "bubble.h"
#include "tinyexpr.h"

#include <QObject>

class ScriptHandler : public QObject
{
  Q_OBJECT;

public:
  ScriptHandler(QImage *image,
                bool *drawing,
                Settings &settings,
                Bubble *bubble,
                const QMap<QString, int> &labels,
                const QMap<QString, Script> &defines,
                QMap<QString, int> &scriptVars,
                const QPoint parentPos,
                const int &size);
  void runScript(int &stop, const Script &script);

signals:
  void playSoundFile(const QString &fileName,
                     const float &panning,
                     const float &pitch);
  void statChange(const QString &type, const int &amount);
  void behavFromFile(const QString &file);
  void setCurFrame(const int &frame);

private:
  te_parser tep;

  QImage *image = nullptr;
  bool *drawing = nullptr;
  Settings &settings;
  Bubble *bubble = nullptr;
  bool returnSet = false;

  const QMap<QString, int> &labels;
  const QMap<QString, Script> &defines;
  QMap<QString, int> &scriptVars;
  const QPoint parentPos;
  const int &size;

  // Main command handler
  void runCommand(QList<QString> &parameters, int &stop, const Script &script);

  // Command handlers
  void handleIf(QList<QString> &parameters, int &stop, const Script &script);
  void handleGoto(QList<QString> &parameters, int &stop);
  void handleVar(QList<QString> &parameters);
  void handleStat(QList<QString> &parameters);
  void handlePrint(QList<QString> &parameters);
  void handleSpawn(QList<QString> &parameters);
  void handleDraw(QList<QString> &parameters);
  void handleReturn(int &stop);
  void handleBreak(int &stop);
  void handleStop(int &stop);
  void handleBehav(QList<QString> &parameters, int &stop);
  void handleCall(QList<QString> &parameters, int &stop);
  void handleSound(QList<QString> &parameters);
  void handleSay(QList<QString> &parameters);
  void handleThink(QList<QString> &parameters);
  void handleThrow(QList<QString> &parameters);

  // Helpers
  void condition(QList<QString> &parameters, bool &isTrue, bool &compare);
  void drawText(QPainter &painter, const int &x, const int &y, const QString &text);
  int getValue(QList<QString> &parameters);

};

#endif // _SCRIPTHANDLER_H
