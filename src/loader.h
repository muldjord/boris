/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            loader.h
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
#ifndef _LOADER_H
#define _LOADER_H

#include "behaviour.h"
#include "mainwindow.h"
#include "sprite.h"

#include "SFML/Audio.hpp"

#include <QObject>
#include <QProgressBar>

class Loader: public QObject
{
  Q_OBJECT;

public:
  static qint64 getAssetsSize(const QDir &dir);
  static bool loadSoundFxs(const QString &soundsPath,
                           QMap<QString, sf::SoundBuffer> &soundFxs,
                           QProgressBar *progressBar);
  static bool loadBehaviours(const Settings &settings,
                             const QString &behavioursPath,
                             QList<Behaviour> &behaviours,
                             QMap<QString, sf::SoundBuffer> &soundFxs,
                             QProgressBar *progressBar);
  static bool loadFont(Settings &settings);
  static bool loadSprites(Settings &settings);
  static bool loadImages(const QString &path, QMap<QString, QPixmap> &images);
  static Script parseScript(const QString &script);
  static void setClothesColor(const Settings &settings, QImage &image);
};

#endif // _LOADER_H
