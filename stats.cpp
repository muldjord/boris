/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            stats.cpp
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

#include "stats.h"
#include "math.h"

#include <stdio.h>
#include <QSettings>
#include <QPainter>

//#define DEBUG

extern QSettings *settings;

Stats::Stats(QWidget *parent) : QGraphicsView(parent)
{
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background:transparent");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFixedSize(80, 80);

  QGraphicsScene *scene = new QGraphicsScene();
  setScene(scene);
  scene->addPixmap(QPixmap(":stats.png"));
  energyBar = scene->addRect(17, 7, 60, 2);
  hungerBar = scene->addRect(17, 23, 60, 2);
  bladderBar = scene->addRect(17, 39, 60, 2);
  socialBar = scene->addRect(17, 55, 60, 2);
  funBar = scene->addRect(17, 71, 60, 2);
  flashIcon = scene->addPixmap(QPixmap(":flash_icon.png"));
  flashIcon->setVisible(false);
  
  flashTimer.setInterval(150);
  connect(&flashTimer, SIGNAL(timeout()), this, SLOT(showHide()));
}

Stats::~Stats()
{
}

void Stats::updateStats(int energy, int hunger, int bladder, int social, int fun)
{
  QColor energyColor((5.1 * (energy - 100) * -1 > 255 ? 255 : round(5.1 * (energy - 100) * -1)),
                     (5.1 * energy > 255 ? 255 : round(5.1 * energy)), 0);
  QColor hungerColor((5.1 * hunger > 255 ? 255 : round(5.1 * hunger)),
                     (5.1 * (hunger - 100) * -1 > 255 ? 255 : round(5.1 * (hunger - 100) * -1)), 0);
  QColor bladderColor((5.1 * bladder > 255 ? 255 : round(5.1 * bladder)),
                     (5.1 * (bladder - 100) * -1 > 255 ? 255 : round(5.1 * (bladder - 100) * -1)), 0);
  QColor socialColor((5.1 * (social - 100) * -1 > 255 ? 255 : round(5.1 * (social - 100) * -1)),
                     (5.1 * social > 255 ? 255 : round(5.1 * social)), 0);
  QColor funColor((5.1 * (fun - 100) * -1 > 255 ? 255 : round(5.1 * (fun - 100) * -1)),
                     (5.1 * fun > 255 ? 255 : round(5.1 * fun)), 0);

  energyBar->setPen(energyColor);
  energyBar->setBrush(QBrush(energyColor));
  hungerBar->setPen(hungerColor);
  hungerBar->setBrush(QBrush(hungerColor));
  bladderBar->setPen(bladderColor);
  bladderBar->setBrush(QBrush(bladderColor));
  socialBar->setPen(socialColor);
  socialBar->setBrush(QBrush(socialColor));
  funBar->setPen(funColor);
  funBar->setBrush(QBrush(funColor));

  energyBar->setRect(17, 7, 0.6 * energy, 1);
  hungerBar->setRect(17, 23, 0.6 * hunger, 1);
  bladderBar->setRect(17, 39, 0.6 * bladder, 1);
  socialBar->setRect(17, 55, 0.6 * social, 1);
  funBar->setRect(17, 71, 0.6 * fun, 1);
}

void Stats::flashStat(QString stat)
{
  if(stat == "none") {
    setVisible(settings->value("stats", "false").toBool());
    flashIcon->setVisible(false);
    flashTimer.stop();
  } else {
    if(stat == "energy") {
      flashIcon->setPos(7, 0);
    } else if(stat == "hunger") {
      flashIcon->setPos(7, 16);
    } else if(stat == "bladder") {
      flashIcon->setPos(7, 32);
    } else if(stat == "social") {
      flashIcon->setPos(7, 48);
    } else if(stat == "fun") {
      flashIcon->setPos(7, 64);
    }
    setVisible(true);
    flashTimer.start();
  }
}

void Stats::showHide()
{
  flashIcon->setVisible(!flashIcon->isVisible());
}
