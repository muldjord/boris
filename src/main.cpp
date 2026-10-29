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

#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QHostInfo>
#include <QMessageBox>
#include <QSplashScreen>

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
  QApplication app(argc, argv);

  QDir::setCurrent(QApplication::applicationDirPath());

  qInstallMessageHandler(customMessageHandler);

  QTranslator translator;
  if(translator.load("boris_" + QLocale::system().name())) {
    app.installTranslator(&translator);
  }

  QPixmap pixmap(":splash.png");
  QSplashScreen splash(pixmap.scaledToWidth(496));
  splash.show();
  app.processEvents();

  MainWindow window(splash);
  window.show();
  splash.finish(&window);
  return app.exec();
}
