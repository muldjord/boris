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
#include <QFileInfo>

About::About(QSettings *iniSettings, Settings &settings) : settings(settings), iniSettings(iniSettings)
{
  setFixedSize(900, 700);
  move((settings.desktopWidth / 2) - (width() / 2), 256);
  setWindowIcon(QIcon(":icon.png"));
  setWindowTitle("Boris v" VERSION);
  
  // About tab
  QWidget *aboutWidget = new QWidget;
  QLabel *aboutText = new QLabel(tr("<strong>Boris bids you welcome!</strong><br/>He is here to keep you company during all seasons of the year. He is a bit of a prankster, so keep an eye on him!<br/><br/>"
                                    "<strong>Good to know:</strong>"
                                    "<ul>"
                                    "<li>You can pick Boris up if he is in your way by holding down the left mouse button while the pointer is on top of him.</li>"
                                    "<li>Right-clicking Boris brings up a menu where you can select any of the behaviours you have unlocked using coins. Each behaviour is added to a queue and performed in the order you select them.</li>"
                                    "<li>Using the mouse scroll-wheel on Boris will tickle him if he's not busy.</li>"
                                    "<li>Right-clicking the Boris system tray icon brings up a menu that gives you access to information and important functions that will help you configure and take care of Boris.</li>"
                                    "</ul>"
                                    "<strong>Health stats:</strong>"
                                    "<ul>"
                                    "<li>The 'Zz' icon signifies Boris' energy level.</li>"
                                    "<li>The 'Knife and fork' icon shows how hungry Boris currently is.</li>"
                                    "<li>The 'Toilet' icon is an indicator of Boris' need to go to the bathroom.</li>"
                                    "<li>The 'Speech bubble' icon shows Boris' need for social interaction.</li>"
                                    "<li>The 'Smiley' icon indicates whether Boris is in a good mood and having fun.</li>"
                                    "</ul>"
                                    "<strong>Other stats:</strong>"
                                    "<ul>"
                                    "<li>Boris can get dirty if he eats messy food or goes to the toilet without washing his hands.</li>"
                                    "<li>He can also get bruised if you thrown him around too much.</li>"
                                    "</ul>"
                                    "<strong>How to get coins:</strong>"
                                    "<br/>It's just for fun! No real money involved. You can earn Boris coins by right-clicking the Boris system tray icon, selecting the 'Items' submenu, and choosing an item that corresponds to the current needs of Boris. The more needy Boris is for the particular item you select, the more coins you receive. Drag the item on top of him to give it to him or wait for him to find it on his own.<br/>"
                                    "You can then spend the coins by selecting the 'Behaviours' submenu and choosing the behaviour you want to buy. The behaviour then becomes unlocked and can be selected anytime you want from the Boris right-click menu or Boris system tray icon 'Behaviours' submenu.<br/>"
                                    "<br/>"
                                    "<strong>Where to get Boris:</strong><br/>"
                                    "If your friends / colleagues become envious and would like a Boris of their own, simply ask them to visit https://github.com/muldjord/boris/releases and download it. He is completely free and open source. You can also download the full source code (GPLv3) in there if you like."));
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
  QLabel *authorText = new QLabel(tr("<strong>Programming:</strong> Lars Muldjord<br/><strong>Graphics:</strong> Lars Muldjord<br/><strong>Sound:</strong> Lars Muldjord<br/><br/>Boris was programmed using the Qt framework (http://www.qt.io) and C++.<br/><br/>Bug reports, suggestions and / or comments can be emailed to me at: muldjordlars@gmail.com.<br/><br/>If your friends / colleagues would like to get their own Boris, tell them to visit:<br/><br/>https://github.com/muldjord/boris/releases<br/><br/>Copyright 2021 Lars Muldjord. This software is distributed under the terms of the GNU General Public License. Be sure to read the license in the 'License' tab or check out the web page http://www.gnu.org/licenses/gpl-3.0.html."));
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
    qWarning("ERROR: Couldn't find COPYING file at the designated location.\n");
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

  enableItems = new QCheckBox(tr("Allow items"));
  enableItems->setToolTip(tr("Boris will sometimes place various items around the desktop. Items will also be randomly spawned or created using the tray icon menu."));
  if(settings.items) {
    enableItems->setCheckState(Qt::Checked);
  }

  QLabel *itemSpawnIntervalLabel = new QLabel(tr("Seconds between random item spawn (5-3600 or 0 for never):"));
  itemSpawnIntervalLineEdit = new QLineEdit();
  itemSpawnIntervalLineEdit->setToolTip(tr("If items are enabled random items will be spawned around the desktop at this interval. If set to 0 no items will spawn randomly."));
  QIntValidator *itemSpawnIntervalValidator = new QIntValidator(5, 3600, this);
  itemSpawnIntervalLineEdit->setValidator(itemSpawnIntervalValidator);
  itemSpawnIntervalLineEdit->setText(QString::number(settings.itemSpawnInterval));

  QLabel *itemTimeoutLabel = new QLabel(tr("Item timeout in seconds (10-3600 or 0 for never):"));
  itemTimeoutLineEdit = new QLineEdit();
  itemTimeoutLineEdit->setToolTip(tr("Items will disappear after this amount of time. Setting this to 0 will make them stay indefinitely."));
  QIntValidator *itemValidator = new QIntValidator(10, 3600, this);
  itemTimeoutLineEdit->setValidator(itemValidator);
  itemTimeoutLineEdit->setText(QString::number(settings.itemTimeout));

  QLabel *weatherLabel = new QLabel(tr("Show weather for city (mouse over for help):"));
  weatherLineEdit = new QLineEdit();
  weatherLineEdit->setToolTip(tr("Type in a nearby city name or the openweathermap city id. Using a city id is the most stable method. To find the id for your city go to openweathermap.org and do a city lookup. The city id will be the last part of the website address."));
  weatherLineEdit->setText(settings.city);

  QLabel *weatherKeyLabel = new QLabel(tr("OpenWeatherMap key (mouse over for help):"));
  weatherKeyLineEdit = new QLineEdit();
  weatherKeyLineEdit->setToolTip(tr("The weather functionality needs an API key to function. The default one should work.<br/>In case it doesn't, get a new one for free at openweathermap.org/appid."));
  weatherKeyLineEdit->setText(settings.key);

  QLabel *feedUrlLabel = new QLabel(tr("RSS feed url (mouse over for help):"));
  feedUrlLineEdit = new QLineEdit();
  feedUrlLineEdit->setToolTip(tr("Type in any RSS feed url. Boris will sometimes update you on a title from this feed. You can click it to open it in the default browser."));
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

  enableBubble = new QCheckBox(tr("Enable Boris speech bubbles"));
  if(settings.bubbles) {
    enableBubble->setCheckState(Qt::Checked);
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
  configLayout->addWidget(enableItems);
  configLayout->addWidget(itemSpawnIntervalLabel);
  configLayout->addWidget(itemSpawnIntervalLineEdit);
  configLayout->addWidget(itemTimeoutLabel);
  configLayout->addWidget(itemTimeoutLineEdit);
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
  configLayout->addWidget(enableBubble);
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
  if(itemSpawnIntervalLineEdit->text().toInt() != 0) {
    if(itemSpawnIntervalLineEdit->text().toInt() < 5) {
      itemSpawnIntervalLineEdit->setText("5");
    }
    if(itemSpawnIntervalLineEdit->text().toInt() > 3600) {
      itemSpawnIntervalLineEdit->setText("3600");
    }
    settings.itemSpawnInterval = itemSpawnIntervalLineEdit->text().toInt();
  } else {
    settings.itemSpawnInterval = 0;
  }
  if(itemTimeoutLineEdit->text().toInt() != 0) {
    if(itemTimeoutLineEdit->text().toInt() < 10) {
      itemTimeoutLineEdit->setText("10");
    }
    if(itemTimeoutLineEdit->text().toInt() > 3600) {
      itemTimeoutLineEdit->setText("3600");
    }
    settings.itemTimeout = itemTimeoutLineEdit->text().toInt();
  } else {
    settings.itemTimeout = 0;
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
  if(enableBubble->isChecked()) {
    settings.bubbles = true;
  } else {
    settings.bubbles = false;
  }
  if(enableItems->isChecked()) {
    settings.items = true;
  } else {
    settings.items = false;
  }
  
  iniSettings->setValue("show_welcome", settings.showWelcome);
  iniSettings->setValue("boris_x", settings.borisX);
  iniSettings->setValue("boris_y", settings.borisY);
  iniSettings->setValue("clones", settings.clones);
  iniSettings->setValue("size", settings.size);
  iniSettings->setValue("items", settings.items);
  iniSettings->setValue("item_timeout", settings.itemTimeout);
  iniSettings->setValue("item_spawn_timer", settings.itemSpawnInterval);
  iniSettings->setValue("independence", settings.independence);
  iniSettings->setValue("bubble", settings.bubbles);
  iniSettings->setValue("sound", settings.sound);
  iniSettings->setValue("volume", settings.volume * 100);
  iniSettings->setValue("feed_url", settings.feedUrl);
  iniSettings->setValue("weather_city", settings.city);
  iniSettings->setValue("weather_key", settings.key);
  if(settings.stats == STATS_ALWAYS) {
    iniSettings->setValue("stats", "always");
  } else if(settings.stats == STATS_CRITICAL) {
    iniSettings->setValue("stats", "critical");
  } else if(settings.stats == STATS_MOUSEOVER) {
    iniSettings->setValue("stats", "mouseover");
  } else if(settings.stats == STATS_NEVER) {
    iniSettings->setValue("stats", "never");
  }

  accept();
}
