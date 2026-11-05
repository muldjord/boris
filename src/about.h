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

#include "stepslider.h"

#include <QDialog>

class Settings;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QSlider;
class QSettings;

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
  QSettings *iniSettings = nullptr;

  StepSlider *clonesSlider = nullptr;
  StepSlider *sizeSlider = nullptr;
  StepSlider *itemSpawnIntervalSlider = nullptr;
  StepSlider *itemTimeoutSlider = nullptr;
  QLineEdit *weatherLineEdit = nullptr;
  QLineEdit *weatherKeyLineEdit = nullptr;
  QLineEdit *feedUrlLineEdit = nullptr;
  QComboBox *statsComboBox = nullptr;
  QCheckBox *makeInvincible = nullptr;
  QCheckBox *enableSound = nullptr;
  QCheckBox *enableItems = nullptr;
  QCheckBox *enableBubbles = nullptr;
  QCheckBox *showWelcome = nullptr;
  StepSlider *independenceSlider = nullptr;
  StepSlider *volumeSlider = nullptr;
};
