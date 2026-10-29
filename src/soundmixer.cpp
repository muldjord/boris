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

#include "soundmixer.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Listener.hpp>

constexpr int numOfChannels = 64;

SoundMixer::SoundMixer()
{
  sf::SoundBuffer buffer;
  for(int a = 0; a < numOfChannels; ++a) {
    sf::Sound channel(buffer);
    channels.push_back(channel);
  }
}

SoundMixer::~SoundMixer()
{
}

void SoundMixer::setGlobalVolume(const int &volume)
{
  sf::Listener::setGlobalVolume(std::clamp(volume, 1, 100));
}

void SoundMixer::playSoundFile(const std::string &fileName,
                               const float &panning,
                               const float &pitch)
{
  if(soundFxs.find(fileName) == soundFxs.end()) {
    return;
  }
  playSound(&soundFxs[fileName], panning, pitch);
}

void SoundMixer::playSound(const sf::SoundBuffer *data,
                           const float &panning,
                           const float &pitch)
{
  for(auto &channel: channels) {
    if(channel.getStatus() == sf::SoundSource::Status::Stopped) {
      channel.setBuffer(*data);
      channel.setPan(panning);
      channel.setPitch(pitch);
      channel.play();
      break;
    }
  }
}
