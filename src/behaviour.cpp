/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            behaviour.cpp
 *
 *  Thu Sep 19 16:56:00 CEST 2019
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

#include "behaviour.h"

Command::Command()
{
}

bool Command::hasChildren() const
{
  return !children.isEmpty();
}

Frame::Frame(const Frame &frame): QObject()
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

Behaviour::Behaviour(const Behaviour &behaviour) : QObject()
{
  this->file = behaviour.file;
  this->title = behaviour.title;
  this->category = behaviour.category;
  this->hyper = behaviour.hyper;
  this->health = behaviour.health;
  this->energy = behaviour.energy;
  this->hunger = behaviour.hunger;
  this->bladder = behaviour.bladder;
  this->social = behaviour.social;
  this->fun = behaviour.fun;
  this->hygiene = behaviour.hygiene;
  this->oneShot = behaviour.oneShot;
  this->doNotDisturb = behaviour.doNotDisturb;
  this->allowFlip = behaviour.allowFlip;
  this->pitchLock = behaviour.pitchLock;
  this->frames = behaviour.frames;
  this->labels = behaviour.labels;
  this->defines = behaviour.defines;
}

void Behaviour::operator=(const Behaviour &behaviour)
{
  this->file = behaviour.file;
  this->title = behaviour.title;
  this->category = behaviour.category;
  this->hyper = behaviour.hyper;
  this->health = behaviour.health;
  this->energy = behaviour.energy;
  this->hunger = behaviour.hunger;
  this->bladder = behaviour.bladder;
  this->social = behaviour.social;
  this->fun = behaviour.fun;
  this->hygiene = behaviour.hygiene;
  this->oneShot = behaviour.oneShot;
  this->doNotDisturb = behaviour.doNotDisturb;
  this->allowFlip = behaviour.allowFlip;
  this->pitchLock = behaviour.pitchLock;
  this->frames = behaviour.frames;
  this->labels = behaviour.labels;
  this->defines = behaviour.defines;
}
