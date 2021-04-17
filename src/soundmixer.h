/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            soundmixer.h
 *
 *  Tue Nov 23 09:36:00 CEST 2019
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

#ifndef _SOUNDMIXER_H
#define _SOUNDMIXER_H

#include "SFML/Audio.hpp"

#include <QObject>
#include <QMap>

class SoundMixer : public QObject
{
  Q_OBJECT;

public:
  explicit SoundMixer(const int channels);
  QMap<QString, sf::SoundBuffer> soundFxs;
  void playSoundFile(const QString &fileName,
                     const float &panning,
                     const float &pitch);
  void playSound(const sf::SoundBuffer *buffer,
                 const float &panning,
                 const float &pitch);

private:
  QList<sf::Sound> soundChannels;
};

#endif // _SOUNDMIXER_H
