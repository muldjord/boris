/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            main.cpp
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

#include "mainwindow.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QHostInfo>

void customMessageHandler(QtMsgType type, const QMessageLogContext&, const QString &msg)
{
  // Add timestamp to debug message
  QString txt = QDateTime::currentDateTime().toString("dd MMM, hh:mm:ss");
  // Decide which type of debug message it is, and add string to signify it
  // Then append the debug message itself to the same string.
  switch (type) {
  case QtInfoMsg:
    txt += QString(": Info: %1").arg(msg);
    break;
  case QtDebugMsg:
    txt += QString(": Debug: %1").arg(msg);
    break;
  case QtWarningMsg:
    txt += QString(": Warning: %1").arg(msg);
    break;
  case QtCriticalMsg:
    txt += QString(": Critical: %1").arg(msg);
    break;
  case QtFatalMsg:
    txt += QString(": Fatal: %1").arg(msg);
    abort();
  }
  printf("%s", txt.toStdString().c_str());
}

int main(int argc, char *argv[])
{
  SDL_Init(SDL_INIT_AUDIO);
  
  QApplication app(argc, argv);
  
  app.setQuitOnLastWindowClosed(false);

  qInstallMessageHandler(customMessageHandler);

  QTranslator translator;
  translator.load("boris_" + QLocale::system().name());
  app.installTranslator(&translator);

  QDir::setCurrent(QApplication::applicationDirPath());

  MainWindow window;

  return app.exec();
  Mix_CloseAudio();
  SDL_Quit();
}
