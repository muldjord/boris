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

#include "behaviour.h"

#include <string>

#include <QObject>
#include <QPixmap>
#include <QString>

#include <SFML/Audio/SoundBuffer.hpp>

class QDir;
class QSplashScreen;
class Settings;

class Loader: public QObject
{
  Q_OBJECT;

public:
  static qint64 getAssetsSize(const QDir &dir);

  static bool loadSoundFxs(const QString &soundsPath,
                           std::map<std::string, sf::SoundBuffer> &soundFxs,
                           QSplashScreen &splash);

  static bool loadBehaviours(const Settings &settings,
                             const QString &behavioursPath,
                             QList<Behaviour> &behaviours,
                             std::map<std::string, sf::SoundBuffer> &soundFxs,
                             QSplashScreen &splash);

  static bool loadFont(Settings &settings);

  static bool loadSprites(Settings &settings);

  static bool loadImages(const QString &path, QMap<QString, QPixmap> &images);

  static Script parseScript(const QString &script);

  static void setClothesColor(const Settings &settings, QImage &image);
};
