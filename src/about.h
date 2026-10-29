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

#include "settings.h"

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QSettings>

class About : public QDialog
{
  Q_OBJECT;
public:
  About(QSettings *iniSettings, Settings &settings);
  ~About();

private slots:
  void volumeChanged(int value);
  void saveAll();

private:
  Settings &settings;
  QSettings *iniSettings;

  QLineEdit *clonesLineEdit;
  QLineEdit *sizeLineEdit;
  QLineEdit *itemSpawnIntervalLineEdit;
  QLineEdit *itemTimeoutLineEdit;
  QLineEdit *weatherLineEdit;
  QLineEdit *weatherKeyLineEdit;
  QLineEdit *feedUrlLineEdit;
  QComboBox *statsComboBox;
  QCheckBox *makeInvincible;
  QCheckBox *enableSound;
  QCheckBox *enableItems;
  QCheckBox *enableBubbles;
  QCheckBox *showWelcome;
  QSlider *independenceSlider;
  QSlider *volumeSlider;
};
