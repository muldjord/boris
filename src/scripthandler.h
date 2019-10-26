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

#include <QObject>

class ScriptHandler : public QObject
{
  Q_OBJECT;

public:
  ScriptHandler(QImage *image, bool *drawing, Boris *boris);
  void runScript(const QList<QString> &script, int &stop);

private:
  Boris *boris = nullptr;
  QImage *image = nullptr;
  bool *drawing = nullptr;
  
  // Main command parser
  void runCommand(QList<QString> &parameters, int &stop);

  // Command handlers
  void handleIf(QList<QString> &parameters, int &stop);
  void handleGoto(QList<QString> &parameters, int &stop);
  void handleVar(QList<QString> &parameters);
  void handleStat(QList<QString> &parameters);
  void handlePrint(QList<QString> &parameters);
  void handleDraw(QList<QString> &parameters);
  void handleBreak(int &stop);

  // Helpers
  void condition(QList<QString> &parameters, bool &isTrue, bool &compare);
  void drawText(QPainter &painter, const Qt::GlobalColor &color,
                const int &x, const int &y, const QString &text);
  int getValue(const QString &value);
  
};

#endif // _SCRIPTHANDLER_H
