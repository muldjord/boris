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

#include "stats.h"

#include <stdio.h>
#include <math.h>

#include <QPainter>
#include <QRandomGenerator>
#include <QTime>
#include <QUuid>

constexpr int STATTIMER = 200;

Stats::Stats(Settings &settings,
	     const int &energy,
	     const int &social,
	     const int &fun,
	     const int &hunger,
	     const int &toilet,
	     const int &anxiety,
	     const int &health,
	     const int &hygiene,
	     const int &hyper,
	     const int &independence) :
  settings(settings),
  energy(energy),
  social(social),
  fun(fun),
  hunger(hunger),
  toilet(toilet),
  anxiety(anxiety),
  health(health),
  hygiene(hygiene),
  hyper(hyper),
  independence(independence)
{
  flashIcon = settings.getPixmap("flash_icon.png");
  statsPixmap = settings.getPixmap("stats.png");

  statProgressTimer.setInterval(30000);
  connect(&statProgressTimer, &QTimer::timeout, this, &Stats::statProgress);
  statProgressTimer.start();

  statQueueTimer.setInterval(50);
  statQueueTimer.setSingleShot(true);
  connect(&statQueueTimer, &QTimer::timeout, this, &Stats::statQueueProgress);
  statQueueTimer.start();
}

Stats::~Stats()
{
}

void Stats::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->drawPixmap(0, 0, rendered);
}

QRectF Stats::boundingRect() const
{
  return QRectF(0, 0, rendered.width(), rendered.height());
}

QColor Stats::getBarColor(const int &value, const bool &inverse)
{
  if(inverse) {
    return QColor((5.1 * value > 255 ? 255 : round(5.1 * value)), (5.1 * (value - 100) * -1 > 255 ? 255 : round(5.1 * (value - 100) * -1)), 0);
  }
  return QColor((5.1 * (value - 100) * -1 > 255 ? 255 : round(5.1 * (value - 100) * -1)), (5.1 * value > 255 ? 255 : round(5.1 * value)), 0);
}

int Stats::getEnergy()
{
  return energy;
}

int Stats::getSocial()
{
  return social;
}

int Stats::getFun()
{
  return fun;
}

int Stats::getHunger()
{
  return hunger;
}

int Stats::getToilet()
{
  return toilet;
}

int Stats::getHyper()
{
  return hyper;
}

int Stats::getHealth()
{
  return health;
}

int Stats::getHygiene()
{
  return hygiene;
}

int Stats::getAnxiety()
{
  return anxiety;
}

int Stats::getIndependence()
{
  return independence;
}

void Stats::setStat(const StatType &stat, const int &value)
{
  switch(stat) {
  case ENERGY:
    energy = std::clamp(value, 0, 100);
    break;
  case SOCIAL:
    social = std::clamp(value, 0, 100);
    break;
  case FUN:
    fun = std::clamp(value, 0, 100);
    break;
  case HUNGER:
    hunger = std::clamp(value, 0, 100);
    break;
  case TOILET:
    toilet = std::clamp(value, 0, 100);
    break;
  case ANXIETY:
    anxiety = std::clamp(value, 0, 100);
    break;
  case HEALTH:
    health = std::clamp(value, 0, 100);
    break;
  case HYGIENE:
    hygiene = std::clamp(value, 0, 100);
    break;
  case HYPER:
    hyper = std::clamp(value, 0, 100);
    break;
  default:
    qWarning("deltaStat: Unknown stat type: %d\n", stat);
    break;
  };
}

void Stats::deltaStat(const StatType &stat, const int &value)
{
  switch(stat) {
  case ENERGY:
    energy = std::clamp(energy + value, 0, 100);
    break;
  case SOCIAL:
    social = std::clamp(social + value, 0, 100);
    break;
  case FUN:
    fun = std::clamp(fun + value, 0, 100);
    break;
  case HUNGER:
    hunger = std::clamp(hunger + value, 0, 100);
    break;
  case TOILET:
    toilet = std::clamp(toilet + value, 0, 100);
    break;
  case ANXIETY:
    anxiety = std::clamp(anxiety + value, 0, 100);
    break;
  case HEALTH:
    health = std::clamp(health + value, 0, 100);
    break;
  case HYGIENE:
    hygiene = std::clamp(hygiene + value, 0, 100);
    break;
  case HYPER:
    hyper = std::clamp(hyper + value, 0, 100);
    break;
  default:
    qWarning("deltaStat: Unknown stat type: %d\n", stat);
    break;
  };
}

void Stats::queueStatChange(const StatType &stat, const int &value)
{
  switch(stat) {
  case ENERGY:
    energyQueue = std::clamp(energyQueue + value, -100, 100);
    break;
  case SOCIAL:
    socialQueue = std::clamp(socialQueue + value, -100, 100);
    break;
  case FUN:
    funQueue = std::clamp(funQueue + value, -100, 100);
    break;
  case HUNGER:
    hungerQueue = std::clamp(hungerQueue + value, -100, 100);
    break;
  case TOILET:
    toiletQueue = std::clamp(toiletQueue + value, -100, 100);
    break;
  case ANXIETY:
    anxietyQueue = std::clamp(anxietyQueue + value, -100, 100);
    break;
  case HEALTH:
    healthQueue = std::clamp(healthQueue + value, -100, 100);
    break;
  case HYGIENE:
    hygieneQueue = std::clamp(hygieneQueue + value, -100, 100);
    break;
  case HYPER:
    hyperQueue = std::clamp(hyperQueue + value, -100, 100);
    break;
  default:
    qWarning("queueStatChange: Unknown stat type: %d\n", stat);
    break;
  };
}

void Stats::setIndependence(const int &value)
{
  independence = value;
}

void Stats::updateStats()
{
  energyColor = getBarColor(energy);
  socialColor = getBarColor(social);
  funColor = getBarColor(fun);
  hungerColor = getBarColor(hunger, true);
  toiletColor = getBarColor(toilet, true);

  rendered = statsPixmap;
  QPainter painter(&rendered);

  painter.setPen(energyColor);
  painter.drawRect(17, 7, 0.6 * energy, 1);

  painter.setPen(socialColor);
  painter.drawRect(17, 23, 0.6 * social, 1);

  painter.setPen(funColor);
  painter.drawRect(17, 39, 0.6 * fun, 1);

  painter.setPen(hungerColor);
  painter.drawRect(3, 55, 0.6 * hunger, 1);

  painter.setPen(toiletColor);
  painter.drawRect(3, 71, 0.6 * toilet, 1);

  if(flashes > 0) {
    if(flashes % 5 == 0) { // Only blink every 5 updates
      showFlashIcon = !showFlashIcon;
    }
    flashes--;
    if(showFlashIcon) {
      painter.drawPixmap(flashIconPos, flashIcon);
    }
  } else {
    showFlashIcon = false;
    if(settings.stats == STATS_CRITICAL && !underMouse) {
      hide();
    }
  }
  painter.end();

  update();
}

void Stats::flashStat(QString stat)
{
  if(stat == "energy") {
    flashIconPos = QPoint(7, 0);
  } else if(stat == "social") {
    flashIconPos = QPoint(7, 16);
  } else if(stat == "fun") {
    flashIconPos = QPoint(7, 32);
  } else if(stat == "hunger") {
    flashIconPos = QPoint(57, 48);
  } else if(stat == "toilet") {
    flashIconPos = QPoint(57, 64);
  }
  flashes = 150;
  showFlashIcon = true;
  if(settings.stats == STATS_CRITICAL) {
    show();
  }
}

void Stats::statProgress()
{
  // If it's late decrease energy to make Boris tired
  // Other than that energy decreases are mainly controlled by walking behaviours
  if(QTime::currentTime().hour() >= 21 && QTime::currentTime().hour() < 23) {
    queueStatChange(StatType::ENERGY, -1);
  } else if(QTime::currentTime().hour() >= 23) {
    queueStatChange(StatType::ENERGY, -3);
  } else if(QTime::currentTime().hour() >= 0 && QTime::currentTime().hour() < 3) {
    queueStatChange(StatType::ENERGY, -4);
  } else if(QTime::currentTime().hour() >= 3 && QTime::currentTime().hour() < 6) {
    queueStatChange(StatType::ENERGY, -5);
  } else if(QTime::currentTime().hour() >= 6 && QTime::currentTime().hour() < 8) {
    queueStatChange(StatType::ENERGY, -1);
  }
  queueStatChange(StatType::SOCIAL, -QRandomGenerator::global()->bounded(2));
  queueStatChange(StatType::FUN, -QRandomGenerator::global()->bounded(3));
  queueStatChange(StatType::HUNGER, QRandomGenerator::global()->bounded(2)); // This one is opposite in the stats overview
  queueStatChange(StatType::ANXIETY, -QRandomGenerator::global()->bounded(8) + 8);
  queueStatChange(StatType::HYGIENE, -QRandomGenerator::global()->bounded(1));
  queueStatChange(StatType::HYPER, -QRandomGenerator::global()->bounded(7));
  // Nothing needed for 'health' and 'toilet'
}

void Stats::statQueueProgress()
{
  energyQueue = std::clamp(energyQueue, -100, 100);
  socialQueue = std::clamp(socialQueue, -100, 100);
  funQueue = std::clamp(funQueue, -100, 100);
  hungerQueue = std::clamp(hungerQueue, -100, 100);
  toiletQueue = std::clamp(toiletQueue, -100, 100);

  anxietyQueue = std::clamp(anxietyQueue, -100, 100);
  healthQueue = std::clamp(healthQueue, -100, 100);
  hygieneQueue = std::clamp(hygieneQueue, -100, 100);
  hyperQueue = std::clamp(hyperQueue, -100, 100);

  if(energyQueue > 0) {
    energyQueue--;
    deltaStat(StatType::ENERGY, 1);
  }
  if(energyQueue < 0) {
    energyQueue++;
    deltaStat(StatType::ENERGY, -1);
  }
  if(socialQueue > 0) {
    socialQueue--;
    deltaStat(StatType::SOCIAL, 1);
  }
  if(socialQueue < 0) {
    socialQueue += 2;
    deltaStat(StatType::SOCIAL, -2);
  }
  if(funQueue > 0) {
    funQueue--;
    deltaStat(StatType::FUN, 1);
  }
  if(funQueue < 0) {
    funQueue++;
    deltaStat(StatType::FUN, -1);
  }
  if(hungerQueue > 0) {
    hungerQueue--;
    deltaStat(StatType::HUNGER, 1);
  }
  if(hungerQueue < 0) {
    hungerQueue++;
    deltaStat(StatType::HUNGER, -1);
  }
  if(toiletQueue > 0) {
    toiletQueue--;
    deltaStat(StatType::TOILET, 1);
  }
  if(toiletQueue < 0) {
    toiletQueue += 2;
    deltaStat(StatType::TOILET, -2);
  }
  if(anxietyQueue > 0) {
    anxietyQueue -= 2;
    deltaStat(StatType::ANXIETY, 2);
  }
  if(anxietyQueue < 0) {
    anxietyQueue++;
    deltaStat(StatType::ANXIETY, -1);
  }
  if(healthQueue > 0) {
    healthQueue -= 2;
    deltaStat(StatType::HEALTH, 2);
  }
  if(healthQueue < 0) {
    healthQueue++;
    deltaStat(StatType::HEALTH, -1);
  }
  if(hygieneQueue > 0) {
    hygieneQueue -= 2;
    deltaStat(StatType::HYGIENE, 2);
  }
  if(hygieneQueue < 0) {
    hygieneQueue++;
    deltaStat(StatType::HYGIENE, -1);
  }
  if(hyperQueue > 0) {
    hyperQueue--;
    deltaStat(StatType::HYPER, 1);
  }
  if(hyperQueue < 0) {
    hyperQueue++;
    deltaStat(StatType::HYPER, -1);
  }

  updateStats();

  int interval = STATTIMER;
  statQueueTimer.setInterval(interval - (interval / 100.0 * hyper));
  if(statQueueTimer.interval() < 5)
    statQueueTimer.setInterval(5);
  statQueueTimer.start();
}

void Stats::stopTimers()
{
  statProgressTimer.stop();
  statQueueTimer.stop();
}
