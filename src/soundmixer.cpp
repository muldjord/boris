/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            soundmixer.cpp
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

#include "soundmixer.h"

SoundMixer::SoundMixer(const int channels)
{
  for(int a = 0; a < channels; ++a) {
    sf::Sound soundChannel;
    soundChannel.setAttenuation(0.f);
    soundChannels.append(soundChannel);
  }
}

void SoundMixer::playSoundFile(const QString &fileName,
                               const float &panning,
                               const float &pitch)
{
  if(soundFxs.contains(fileName)) {
    playSound(&soundFxs[fileName], panning, pitch);
  }
}

void SoundMixer::playSound(const sf::SoundBuffer *buffer,
                           const float &panning,
                           const float &pitch)
{
  for(auto &channel: soundChannels) {
    if(channel.getStatus() == sf::SoundSource::Status::Stopped) {
      channel.setBuffer(*buffer);
      channel.setPosition(panning, 0.f, 2.f);
      channel.setPitch(pitch);
      channel.play();
      break;
    }
  }
}
