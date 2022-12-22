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

#include <stdio.h>
#include <math.h>
#include <QUuid>

Stats::Stats(Settings &settings,
             const int &hyper,
             const int &health,
             const int &energy,
             const int &hunger,
             const int &toilet,
             const int &social,
             const int &fun,
             const int &hygiene,
             const int &anxiety,
             QWidget *parent) :
  QGraphicsView(parent),
  settings(settings),
  hyper(hyper),
  health(health),
  energy(energy),
  hunger(hunger),
  toilet(toilet),
  social(social),
  fun(fun),
  hygiene(hygiene),
  anxiety(anxiety)
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
  scene->addPixmap(settings.getPixmap("stats.png"));
  energyBar = scene->addRect(17, 7, 60, 2);
  socialBar = scene->addRect(17, 23, 60, 2);
  funBar = scene->addRect(17, 39, 60, 2);
  hungerBar = scene->addRect(3, 55, 60, 2);
  toiletBar = scene->addRect(3, 71, 60, 2);
  flashIcon = scene->addPixmap(settings.getPixmap("flash_icon.png"));
  flashIcon->setVisible(false);
  
  flashes = 0;
  underMouse = false;
  
  if(settings.statLogging) {
    statLog.setFileName("stats_" + QUuid::createUuid().toString().replace("{", "").replace("}", "") + ".csv");
    if(statLog.open(QIODevice::WriteOnly)) {
      statLog.write("anxiety;energy;fun;health;hunger;hygiene;hyper;social;toilet\n");
      statTimer.setInterval(10000);
      statTimer.setSingleShot(false);
      connect(&statTimer, &QTimer::timeout, this, &Stats::logStats);
      statTimer.start();
    }
  }
}

Stats::~Stats()
{
  if(statLog.isOpen()) {
    statLog.close();
  }
}

void Stats::logStats()
{
  qInfo("Logging stats\n");
  statLog.write(
                QByteArray::number(anxiety) + ";" +
                QByteArray::number(energy) + ";" +
                QByteArray::number(fun) + ";" +
                QByteArray::number(health) + ";" +
                QByteArray::number(hunger) + ";" +
                QByteArray::number(hygiene) + ";" +
                QByteArray::number(hyper) + ";" +
                QByteArray::number(social) + ";" +
                QByteArray::number(toilet) + "\n");
}

int Stats::getHyper()
{
  return hyper;
}

int Stats::getHealth()
{
  return health;
}

int Stats::getEnergy()
{
  return energy;
}

int Stats::getHunger()
{
  return hunger;
}

int Stats::getToilet()
{
  return toilet;
}

int Stats::getSocial()
{
  return social;
}

int Stats::getFun()
{
  return fun;
}

int Stats::getHygiene()
{
  return hygiene;
}

int Stats::getAnxiety()
{
  return anxiety;
}

void Stats::deltaHyper(int value)
{
  if(hyper + value > 100) {
    hyper = 100;
  } else if(hyper + value < 0) {
    hyper = 0;
  } else {
    hyper += value;
  }
}

void Stats::deltaHealth(int value)
{
  if(health + value > 100) {
    health = 100;
  } else if(health + value < 0) {
    health = 0;
  } else {
    health += value;
  }
}

void Stats::deltaEnergy(int value)
{
  if(energy + value > 100) {
    energy = 100;
  } else if(energy + value < 0) {
    energy = 0;
  } else {
    energy += value;
  }
}

void Stats::deltaHunger(int value)
{
  if(hunger + value > 100) {
    hunger = 100;
  } else if(hunger + value < 0) {
    hunger = 0;
  } else {
    hunger += value;
  }
}

void Stats::deltaToilet(int value)
{
  if(toilet + value > 100) {
    toilet = 100;
  } else if(toilet + value < 0) {
    toilet = 0;
  } else {
    toilet += value;
  }
}

void Stats::deltaSocial(int value)
{
  if(social + value > 100) {
    social = 100;
  } else if(social + value < 0) {
    social = 0;
  } else {
    social += value;
  }
}

void Stats::deltaFun(int value)
{
  if(fun + value > 100) {
    fun = 100;
  } else if(fun + value < 0) {
    fun = 0;
  } else {
    fun += value;
  }
}

void Stats::deltaHygiene(int value)
{
  if(hygiene + value > 100) {
    hygiene = 100;
  } else if(hygiene + value < 0) {
    hygiene = 0;
  } else {
    hygiene += value;
  }
}

void Stats::deltaAnxiety(int value)
{
  if(anxiety + value > 100) {
    anxiety = 100;
  } else if(anxiety + value < 0) {
    anxiety = 0;
  } else {
    anxiety += value;
  }
}

void Stats::updateStats()
{
  QColor energyColor((5.1 * (energy - 100) * -1 > 255 ? 255 : round(5.1 * (energy - 100) * -1)),
                     (5.1 * energy > 255 ? 255 : round(5.1 * energy)), 0);
  /*
  QColor hungerColor((5.1 * (hunger - 100) * -1 > 255 ? 255 : round(5.1 * (hunger - 100) * -1)),
                     (5.1 * hunger > 255 ? 255 : round(5.1 * hunger)), 0);
  */
  QColor hungerColor((5.1 * hunger > 255 ? 255 : round(5.1 * hunger)),
                     (5.1 * (hunger - 100) * -1 > 255 ? 255 : round(5.1 * (hunger - 100) * -1)), 0);
  /*
  QColor toiletColor((5.1 * (toilet - 100) * -1 > 255 ? 255 : round(5.1 * (toilet - 100) * -1)),
                     (5.1 * toilet > 255 ? 255 : round(5.1 * toilet)), 0);
  */
  QColor toiletColor((5.1 * toilet > 255 ? 255 : round(5.1 * toilet)),
                     (5.1 * (toilet - 100) * -1 > 255 ? 255 : round(5.1 * (toilet - 100) * -1)), 0);
  QColor socialColor((5.1 * (social - 100) * -1 > 255 ? 255 : round(5.1 * (social - 100) * -1)),
                     (5.1 * social > 255 ? 255 : round(5.1 * social)), 0);
  QColor funColor((5.1 * (fun - 100) * -1 > 255 ? 255 : round(5.1 * (fun - 100) * -1)),
                     (5.1 * fun > 255 ? 255 : round(5.1 * fun)), 0);

  energyBar->setPen(energyColor);
  energyBar->setBrush(QBrush(energyColor));
  hungerBar->setPen(hungerColor);
  hungerBar->setBrush(QBrush(hungerColor));
  toiletBar->setPen(toiletColor);
  toiletBar->setBrush(QBrush(toiletColor));
  socialBar->setPen(socialColor);
  socialBar->setBrush(QBrush(socialColor));
  funBar->setPen(funColor);
  funBar->setBrush(QBrush(funColor));

  energyBar->setRect(17, 7, 0.6 * energy, 1);
  socialBar->setRect(17, 23, 0.6 * social, 1);
  funBar->setRect(17, 39, 0.6 * fun, 1);
  hungerBar->setRect(3, 55, 0.6 * hunger, 1);
  toiletBar->setRect(3, 71, 0.6 * toilet, 1);

  if(flashes > 0) {
    flashIcon->setVisible(!flashIcon->isVisible());
    flashes--;
  } else {
    flashIcon->setVisible(false);
    if(settings.stats == STATS_CRITICAL && !underMouse) {
      this->hide();
    }
  }
}

void Stats::flashStat(QString stat)
{
  if(stat == "energy") {
    flashIcon->setPos(7, 0);
  } else if(stat == "social") {
    flashIcon->setPos(7, 16);
  } else if(stat == "fun") {
    flashIcon->setPos(7, 32);
  } else if(stat == "hunger") {
    flashIcon->setPos(57, 48);
  } else if(stat == "toilet") {
    flashIcon->setPos(57, 64);
  }
  flashes = 15;
  flashIcon->setVisible(true);
  if(settings.stats == STATS_CRITICAL) {
    this->show();
  }
}
