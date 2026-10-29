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

#include <QMap>
#include <QObject>
#include <QPixmap>

#include <SFML/Audio/SoundBuffer.hpp>

class Reaction
{
public:
  Reaction(const Reaction &reaction);
  void operator=(const Reaction &reaction);
  explicit Reaction(const QString &borisBehaviour,
                    const QString &itemLabel = "",
                    const int &xCoord = 0,
                    const int &yCoord = 0);
  QString getBehaviour() const;
  QString getLabel() const;
  int getXCoord() const;
  int getYCoord() const;

private:
  QString borisBehaviour = "";
  QString itemLabel = "";
  int xCoord = 0;
  int yCoord = 0;
};

// All single commands stored in the list with all children blocks replaced by '##1##', '##2##'
class Script
{
public:
  Script(const Script &script);
  void operator=(const Script &script);
  Script();
  QList<QString> commands;
  QMap<QString, Script> blocks; // Contains map of '##1##', '##2##'... which contains a script with the commands of that block inside
};

class Frame
{
public:
  Frame(const Frame &frame);
  void operator=(const Frame &frame);
  Frame();
  QPixmap sprite;
  int time = 0;
  int dx = 0;
  int dy = 0;
  sf::SoundBuffer *soundBuffer = nullptr;
  Script script;
};

class Behaviour: public QObject
{
  Q_OBJECT;

public:
  Behaviour(const Behaviour &behaviour);
  void operator=(const Behaviour &behaviour);
  Behaviour();
  QString absoluteFilePath;
  QString file;
  QString title;
  QString category = "Other";
  QList<Reaction> reactions;
  int hyper = 0;
  int health = 0;
  int energy = 0;
  int hunger = 0;
  int toilet = 0;
  int social = 0;
  int fun = 0;
  int hygiene = 0;
  int anxiety = 0;
  bool oneShot = false;
  bool doNotDisturb = false;
  bool allowFlip = false;
  bool pitchLock = false;
  bool noIgnore = false; // For Item: Tells whether it should be instantly active when spawned
  int coins = 50;
  int weight = 50;
  int bounce = 50;
  QList<Frame> frames;
  QMap<QString, int> labels; // Used for scripts when goto'ing
  QMap<QString, Script> defines; // Used for defines of reusable scripting blocks
};
