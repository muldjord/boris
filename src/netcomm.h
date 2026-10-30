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

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QTimer>

class Settings;
class QNetworkReply;

class NetComm : public QNetworkAccessManager
{
  Q_OBJECT;
public:
  explicit NetComm(Settings &settings);
  ~NetComm();

public slots:
  void updateAll();

signals:
  void weatherUpdated();
  void feedUpdated();

private slots:
  void netReply(QNetworkReply *reply);

private:
  Settings &settings;

  QTimer netTimer;
  QNetworkRequest weatherRequest;
  QNetworkRequest feedRequest;
};
