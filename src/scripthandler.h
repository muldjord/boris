/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            scripthandler.h
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
#ifndef _SCRIPTHANDLER_H
#define _SCRIPTHANDLER_H

#include "boris.h"

#include <QObject>

class ScriptHandler : public QObject
{
  Q_OBJECT;

public:
  ScriptHandler(Boris *boris);
  bool runScript(const QList<QString> &script);

private:
  Boris *boris = nullptr;

  // Main command parser
  bool runCommand(QList<QString> &parameters);

  // Command handlers
  bool handleIf(QList<QString> &parameters);
  void handleConditions(QList<QString> &parameters, bool &cond, const bool &compare);
  bool handleGoto(QList<QString> &parameters);
  void handleVar(QList<QString> &parameters);
  void handleStat(QList<QString> &parameters);
  void handlePrint(QList<QString> &parameters);
  void handleBreak();
  
};

#endif // _SCRIPTHANDLER_H
