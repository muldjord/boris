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

#include "behaviour.h"

Reaction::Reaction(const Reaction &reaction)
{
  this->borisBehaviour = reaction.borisBehaviour;
  this->itemLabel = reaction.itemLabel;
  this->xCoord = reaction.xCoord;
  this->yCoord = reaction.yCoord;
}

void Reaction::operator=(const Reaction &reaction)
{
  this->borisBehaviour = reaction.borisBehaviour;
  this->itemLabel = reaction.itemLabel;
  this->xCoord = reaction.xCoord;
  this->yCoord = reaction.yCoord;
}

Reaction::Reaction(const QString &borisBehaviour,
                   const QString &itemLabel,
                   const int &xCoord,
                   const int &yCoord)
{
  this->borisBehaviour = borisBehaviour;
  this->itemLabel = itemLabel;
  this->xCoord = xCoord;
  this->yCoord = yCoord;
}

QString Reaction::getBehaviour() const
{
  return borisBehaviour;
}

QString Reaction::getLabel() const
{
  return itemLabel;
}

int Reaction::getXCoord() const
{
  return xCoord;
}

int Reaction::getYCoord() const
{
  return yCoord;
}

Script::Script(const Script &script)
{
  this->commands = script.commands;
  this->blocks = script.blocks;
}

void Script::operator=(const Script &script)
{
  this->commands = script.commands;
  this->blocks = script.blocks;
}

Script::Script()
{
}

Frame::Frame(const Frame &frame)
{
  this->sprite = frame.sprite;
  this->time = frame.time;
  this->dx = frame.dx;
  this->dy = frame.dy;
  this->soundBuffer = frame.soundBuffer;
  this->script = frame.script;
}

void Frame::operator=(const Frame &frame)
{
  this->sprite = frame.sprite;
  this->time = frame.time;
  this->dx = frame.dx;
  this->dy = frame.dy;
  this->soundBuffer = frame.soundBuffer;
  this->script = frame.script;
}

Frame::Frame()
{
}

Behaviour::Behaviour()
{
}

Behaviour::Behaviour(const Behaviour &behaviour)
{
  this->absoluteFilePath = behaviour.absoluteFilePath;
  this->file = behaviour.file;
  this->title = behaviour.title;
  this->category = behaviour.category;
  this->reactions = behaviour.reactions;
  this->coins = behaviour.coins;
  this->weight = behaviour.weight;
  this->bounce = behaviour.bounce;
  this->hyper = behaviour.hyper;
  this->health = behaviour.health;
  this->energy = behaviour.energy;
  this->hunger = behaviour.hunger;
  this->toilet = behaviour.toilet;
  this->social = behaviour.social;
  this->fun = behaviour.fun;
  this->hygiene = behaviour.hygiene;
  this->anxiety = behaviour.anxiety;
  this->oneShot = behaviour.oneShot;
  this->doNotDisturb = behaviour.doNotDisturb;
  this->allowFlip = behaviour.allowFlip;
  this->pitchLock = behaviour.pitchLock;
  this->noIgnore = behaviour.noIgnore;
  this->frames = behaviour.frames;
  this->labels = behaviour.labels;
  this->defines = behaviour.defines;
}

void Behaviour::operator=(const Behaviour &behaviour)
{
  this->absoluteFilePath = behaviour.absoluteFilePath;
  this->file = behaviour.file;
  this->title = behaviour.title;
  this->category = behaviour.category;
  this->reactions = behaviour.reactions;
  this->coins = behaviour.coins;
  this->weight = behaviour.weight;
  this->bounce = behaviour.bounce;
  this->hyper = behaviour.hyper;
  this->health = behaviour.health;
  this->energy = behaviour.energy;
  this->hunger = behaviour.hunger;
  this->toilet = behaviour.toilet;
  this->social = behaviour.social;
  this->fun = behaviour.fun;
  this->hygiene = behaviour.hygiene;
  this->anxiety = behaviour.anxiety;
  this->oneShot = behaviour.oneShot;
  this->doNotDisturb = behaviour.doNotDisturb;
  this->allowFlip = behaviour.allowFlip;
  this->pitchLock = behaviour.pitchLock;
  this->noIgnore = behaviour.noIgnore;
  this->frames = behaviour.frames;
  this->labels = behaviour.labels;
  this->defines = behaviour.defines;
}
