/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            about.cpp
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
#include "about.h"
#include "settings.h"

#include "SFML/Audio.hpp"

#include <stdio.h>
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QScrollArea>
#include <QFile>
#include <QIntValidator>
#include <QHostInfo>
#include <QSettings>
#include <QFileInfo>

extern Settings settings;

About::About(QWidget *parent) : QDialog(parent)
{
  setFixedSize(900, 520);
  move((settings.desktopWidth / 2) - (width() / 2), 256);
  setWindowIcon(QIcon(":icon.png"));
  setWindowTitle("Boris v" VERSION);

  // About tab
  QWidget *aboutWidget = new QWidget;
  QLabel *aboutText = new QLabel(tr("<strong>Boris bids you welcome!</strong><br/>He is here to keep you company during all seasons of the year. He is a bit of a prankster, so keep an eye on him!<br/><br/>"
                                    "<strong>Functions:</strong><ul><li>You can throw Boris around your desktop by left-clicking him. Don't throw him too much though, or you'll bruise him...</li><li>Right-clicking Boris brings up a menu where you can select what you want Boris to do. Each activity is added to a queue and performed in the order you select them.</li><li>Right-click the tray icon to either quit, cause an earthquake, teleport or open this config/about box.</li></ul><strong>Vitality description:</strong><ul><li>The 'Zz' icon signifies Boris' energy level. Tell him to relax to give him more energy.</li><li>The 'knife and fork' icon shows how hungry Boris is. Tell him to eat to remove his hunger.</li><li>The 'toilet' icon is an indicator of Boris' need to go to the toilet. Tell him to weewee to make him feel better.</li><li>The 'speech bubble' icon shows Boris' need for social interaction. Move you mouse over him to interact with him. Boris will also socialize with other Boris clones when he meets them.</li><li>The 'smiley' icon indicates if Boris is in a good mood. If he's sad, tell him to do something fun.</li><li>If Boris is getting dirty, tell him to take a shower.</li></ul><br/>If your friends / colleagues become envious and would like a Boris of their own, simply ask them to visit http://www.muldjord.com/boris and download it. You can also download the full source code (GPLv3) in there if you like."));
  aboutText->setWordWrap(true);
  aboutText->setMaximumWidth(400);

  QVBoxLayout *aboutLayout = new QVBoxLayout();
  aboutLayout->addWidget(aboutText);
  aboutWidget->setLayout(aboutLayout);

  QScrollArea *aboutScroll = new QScrollArea();
  aboutScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  aboutScroll->setWidget(aboutWidget);

  // Author tab
  QWidget *authorWidget = new QWidget;
  QLabel *authorText = new QLabel(tr("<strong>Programming:</strong> Lars Muldjord<br/><strong>Graphics:</strong> Lars Muldjord<br/><strong>Sound:</strong> Lars Muldjord<br/><br/>Boris was programmed using the Qt framework (http://www.qt.io) and C++.<br/><br/>Bug reports, suggestions and / or comments can be emailed to me at: muldjordlars@gmail.com.<br/><br/>If your friends / colleagues would like to get their own Boris, tell them to visit:<br/><br/>http://www.muldjord.com/boris<br/><br/>Copyright 2019 Lars Muldjord. This software is distributed under the terms of the GNU General Public License. Be sure to read the license in the 'License' tab or check out the web page http://www.gnu.org/licenses/gpl-3.0.html."));
  authorText->setWordWrap(true);
  authorText->setMaximumWidth(400);

  QVBoxLayout *authorLayout = new QVBoxLayout();
  authorLayout->addWidget(authorText);
  authorWidget->setLayout(authorLayout);

  QScrollArea *authorScroll = new QScrollArea();
  authorScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  authorScroll->setWidget(authorWidget);

  // License tab
  // Read COPYING data from file
  QFile file("COPYING.TXT");
  QByteArray gplText;
  if(file.open(QIODevice::ReadOnly)) {
    gplText = file.readAll();
    file.close();
  } else {
    printf("ERROR: Couldn't find COPYING file at the designated location.\n");
    gplText = "ERROR: File not found... This means that someone has been fiddling with the files of this software, and someone might be violating the terms of the GPL. Go to the following location to read the license: http://www.gnu.org/licenses/gpl-3.0.html";
  }

  QWidget *licenseWidget = new QWidget;
  QLabel *licenseText = new QLabel(gplText);
  licenseText->setWordWrap(true);
  licenseText->setMaximumWidth(400);

  QVBoxLayout *licenseLayout = new QVBoxLayout();
  licenseLayout->addWidget(licenseText);
  licenseWidget->setLayout(licenseLayout);

  QScrollArea *licenseScroll = new QScrollArea();
  licenseScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  licenseScroll->setWidget(licenseWidget);

  QTabWidget *tabWidget = new QTabWidget;
  tabWidget->addTab(aboutScroll, tr("About"));
  tabWidget->addTab(authorScroll, tr("Author"));
  tabWidget->addTab(licenseScroll, tr("License"));
  
  QPushButton *okButton = new QPushButton(tr("Close and save"));
  connect(okButton, &QPushButton::clicked, this, &About::saveAll);

  QLabel *sizeLabel = new QLabel(tr("Boris size in pixels (8-256 or 0 for random):"));
  sizeLineEdit = new QLineEdit();
  QIntValidator *sizeValidator = new QIntValidator(0, 256, this);
  sizeLineEdit->setValidator(sizeValidator);
  sizeLineEdit->setText(QString::number(settings.size));

  QLabel *clonesLabel = new QLabel(tr("Boris clones (1-100 or 0 for random):"));
  clonesLineEdit = new QLineEdit();
  QIntValidator *clonesValidator = new QIntValidator(0, 100, this);
  clonesLineEdit->setValidator(clonesValidator);
  clonesLineEdit->setText(QString::number(settings.clones));

  QLabel *weatherLabel = new QLabel(tr("Show weather for city (mouse over for help):"));
  weatherLineEdit = new QLineEdit();
  weatherLineEdit->setToolTip(tr("Try typing in a nearby city. If it doesn't work, go to openweathermap.org and search for a city until you find one that exists.<br/>Then type that in exactly as it is shown on their website."));
  weatherLineEdit->setText(settings.city);

  QLabel *weatherKeyLabel = new QLabel(tr("OpenWeatherMap key (mouse over for help):"));
  weatherKeyLineEdit = new QLineEdit();
  weatherKeyLineEdit->setToolTip(tr("The weather functionality needs an API key to function. The default one should work.<br/>In case it doesn't, get a new one for free at openweathermap.org/appid"));
  weatherKeyLineEdit->setText(settings.key);

  QLabel *feedUrlLabel = new QLabel(tr("RSS feed url:"));
  feedUrlLineEdit = new QLineEdit();
  feedUrlLineEdit->setToolTip(tr("Type in any RSS feed url. Boris will sometimes update you on a title from this feed"));
  feedUrlLineEdit->setText(settings.feedUrl);

  QLabel *statsLabel = new QLabel(tr("Vitality stats:"));
  statsComboBox = new QComboBox();
  statsComboBox->addItem(tr("Always show"), STATS_ALWAYS);
  statsComboBox->addItem(tr("Show on critical levels and mouse over"), STATS_CRITICAL);
  statsComboBox->addItem(tr("Show only on mouse over"), STATS_MOUSEOVER);
  statsComboBox->addItem(tr("Never show"), STATS_NEVER);
  statsComboBox->setCurrentIndex(statsComboBox->findData(settings.stats));

  QLabel *independenceLabel = new QLabel(tr("Independence:"));
  independenceSlider = new QSlider(Qt::Horizontal);
  independenceSlider->setMinimum(0);
  independenceSlider->setMaximum(100);
  independenceSlider->setValue(settings.independence);

  enableChatter = new QCheckBox(tr("Enable Boris speech bubbles"));
  if(settings.chatter) {
    enableChatter->setCheckState(Qt::Checked);
  }

  enableSound = new QCheckBox(tr("Enable sound"));
  if(settings.sound) {
    enableSound->setCheckState(Qt::Checked);
  }

  QLabel *volumeLabel = new QLabel(tr("Sound volume:"));
  volumeSlider = new QSlider(Qt::Horizontal);
  volumeSlider->setMinimum(0);
  volumeSlider->setMaximum(100);
  volumeSlider->setValue(settings.volume * 100);
  connect(volumeSlider, &QSlider::valueChanged, this, &About::volumeChanged);
  
  showWelcome = new QCheckBox(tr("Always show this dialog on startup"));
  if(settings.showWelcome) {
    showWelcome->setCheckState(Qt::Checked);
  }

  QVBoxLayout *configLayout = new QVBoxLayout();
  configLayout->addWidget(showWelcome);
  configLayout->addWidget(sizeLabel);
  configLayout->addWidget(sizeLineEdit);
  configLayout->addWidget(clonesLabel);
  configLayout->addWidget(clonesLineEdit);
  configLayout->addWidget(statsLabel);
  configLayout->addWidget(statsComboBox);
  configLayout->addWidget(independenceLabel);
  configLayout->addWidget(independenceSlider);
  configLayout->addWidget(enableSound);
  configLayout->addWidget(volumeLabel);
  configLayout->addWidget(volumeSlider);
  configLayout->addWidget(weatherLabel);
  configLayout->addWidget(weatherLineEdit);
  configLayout->addWidget(weatherKeyLabel);
  configLayout->addWidget(weatherKeyLineEdit);
  configLayout->addWidget(enableChatter);
  configLayout->addWidget(feedUrlLabel);
  configLayout->addWidget(feedUrlLineEdit);
  
  QVBoxLayout *infoLayout = new QVBoxLayout;
  infoLayout->addWidget(tabWidget);
  infoLayout->addWidget(okButton);

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addLayout(configLayout);
  layout->addLayout(infoLayout);
  
  setLayout(layout);
}

About::~About()
{
}

void About::volumeChanged(int value)
{
  sf::Listener::setGlobalVolume(value);
}

void About::saveAll()
{
  if(showWelcome->isChecked()) {
    settings.showWelcome = true;
  } else {
    settings.showWelcome = false;
  }
  if(sizeLineEdit->text().toInt() != 0) {
    if(sizeLineEdit->text().toInt() < 8) {
      sizeLineEdit->setText("8");
    }
    if(sizeLineEdit->text().toInt() > 256) {
      sizeLineEdit->setText("256");
    }
    settings.size = sizeLineEdit->text().toInt();
  } else {
    settings.size = 0;
  }
  if(clonesLineEdit->text().toInt() != 0) {
    if(clonesLineEdit->text().toInt() < 1) {
      clonesLineEdit->setText("1");
    }
    if(clonesLineEdit->text().toInt() > 100) {
      clonesLineEdit->setText("100");
    }
    settings.clones = clonesLineEdit->text().toInt();
  } else {
    settings.clones = 0;
  }
  settings.city = weatherLineEdit->text();
  settings.key = weatherKeyLineEdit->text();
  settings.feedUrl = feedUrlLineEdit->text();
  settings.stats = statsComboBox->currentData().toInt();
  settings.independence = independenceSlider->value();
  if(enableSound->isChecked()) {
    settings.sound = true;
  } else {
    settings.sound = false;
  }
  settings.volume = volumeSlider->value() / 100.0;
  if(enableChatter->isChecked()) {
    settings.chatter = true;
  } else {
    settings.chatter = false;
  }
  
  // Save relevants config to ini also
  QString iniFile = "config.ini";
  if(QFileInfo::exists("config_" + QHostInfo::localHostName().toLower() + ".ini")) {
    iniFile = "config_" + QHostInfo::localHostName().toLower() + ".ini";
  }
  QSettings iniSettings(iniFile, QSettings::IniFormat);
  iniSettings.setValue("show_welcome", settings.showWelcome);
  iniSettings.setValue("boris_x", settings.borisX);
  iniSettings.setValue("boris_y", settings.borisY);
  iniSettings.setValue("clones", settings.clones);
  iniSettings.setValue("size", settings.size);
  iniSettings.setValue("independence", settings.independence);
  iniSettings.setValue("chatter", settings.chatter);
  iniSettings.setValue("sound", settings.sound);
  iniSettings.setValue("volume", settings.volume * 100);
  iniSettings.setValue("feed_url", settings.feedUrl);
  iniSettings.setValue("weather_city", settings.city);
  iniSettings.setValue("weather_key", settings.key);
  if(settings.stats == STATS_ALWAYS) {
    iniSettings.setValue("stats", "always");
  } else if(settings.stats == STATS_CRITICAL) {
    iniSettings.setValue("stats", "critical");
  } else if(settings.stats == STATS_MOUSEOVER) {
    iniSettings.setValue("stats", "mouseover");
  } else if(settings.stats == STATS_NEVER) {
    iniSettings.setValue("stats", "never");
  }

  accept();
}
