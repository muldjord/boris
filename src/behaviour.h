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
#ifndef _BEHAVIOUR_H
#define _BEHAVIOUR_H

#include <SDL2/SDL_mixer.h>
#include <QObject>
#include <QPixmap>

class Frame: public QObject
{
  Q_OBJECT;

public:
  Frame(const Frame &frame);
  void operator=(const Frame &frame);
  Frame();
  QPixmap sprite;
  int time;
  int dx;
  int dy;
  bool hide;
  bool show;
  Mix_Chunk *soundFx = nullptr;
};

class Behaviour: public QObject
{
  Q_OBJECT;

public:
  Behaviour(const Behaviour &behaviour);
  void operator=(const Behaviour &behaviour);
  Behaviour();
  QString file;
  QString title;
  QString category;
  int hyper;
  int health;
  int energy;
  int hunger;
  int bladder;
  int social;
  int fun;
  int hygiene;
  bool oneShot;
  bool doNotDisturb;
  bool allowFlip;
  QList<Frame> frames;
};

#endif // _BEHAVIOUR_H
