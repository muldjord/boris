/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            about.h
 *
 *  Sat Nov 24 14:02:00 CEST 2012
 *  Copyright 2012 Lars Muldjord
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
#ifndef _ABOUT_H
#define _ABOUT_H

#include "settings.h"

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>

class About : public QDialog
{
  Q_OBJECT;
public:
  About(Settings *settings);
  ~About();

private slots:
  void volumeChanged(int value);
  void saveAll();

private:
  Settings *settings;
  
  QLineEdit *clonesLineEdit;
  QLineEdit *sizeLineEdit;
  QLineEdit *itemLineEdit;
  QLineEdit *weatherLineEdit;
  QLineEdit *weatherKeyLineEdit;
  QLineEdit *feedUrlLineEdit;
  QComboBox *statsComboBox;
  //QCheckBox *showStats;
  QCheckBox *enableSound;
  QCheckBox *enableItems;
  QCheckBox *enableChatter;
  QCheckBox *showWelcome;
  QSlider *independenceSlider;
  QSlider *volumeSlider;
};

#endif // _ABOUT_H
