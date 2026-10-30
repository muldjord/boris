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

#include <QObject>

#include <map>
#include <string>
#include <vector>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

class SoundMixer : public QObject
{
  Q_OBJECT;

public:
  SoundMixer();
  ~SoundMixer();
  void setGlobalVolume(const int &volume);
  void playSoundFile(const std::string &fileName,
                     const float &panning,
                     const float &pitch);

  void playSound(const sf::SoundBuffer *data,
                 const float &panning,
                 const float &pitch);
  std::map<std::string, sf::SoundBuffer> soundFxs;

private:
  std::vector<sf::Sound> channels;
};
