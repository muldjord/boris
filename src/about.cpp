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

#include "about.h"

#include "settings.h"
#include "soundmixer.h"
#include "version.h"

#include <stdio.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QSlider>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

#include <SFML/Audio/Sound.hpp>

extern SoundMixer *soundMixer;

About::About(QSettings *iniSettings, Settings &settings) : settings(settings), iniSettings(iniSettings)
{
  setMinimumWidth(1024);
  setWindowIcon(QIcon(":icon.png"));
  setWindowTitle(QApplication::applicationName() + " v" +
                 QString("%1.%2.%3")
                 .arg(PROJECT_VERSION_MAJOR)
                 .arg(PROJECT_VERSION_MINOR)
                 .arg(PROJECT_VERSION_PATCH));

  QString aboutStyle = R"(
/* ==========================================
   MAIN DIALOG AND WIDGETS
   ========================================== */

QDialog {
    background-color: #FFF0D0;
    color: #14581D;
    font-family: "sans-serif";
    font-size: 14pt;
}

QWidget {
    color: #14581D;
}

/* ==========================================
   COMBO BOX
   ========================================== */

QComboBox {
    background-color: white;
    color: #14581D;
    border: 3px solid #14581D;
    border-radius: 10px;
    padding: 3px 5px;
    min-height: 24px;
}

QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 30px;
    border-left: 3px solid #14581D;
    border-top-right-radius: 5px;
    border-bottom-right-radius: 5px;
    background-color: #4A8710;
}

QComboBox::down-arrow {
    image: url(:downarrow.png);
    width: 20px;
    height: 20px;
}

QComboBox QAbstractItemView {
    background-color: transparent;
    color: #14581D;
    border: 3px solid #14581D;
    border-radius: 0px;
    selection-background-color: #14581D;
    selection-color: white;
    padding: 0px;
}

/* ==========================================
   LABELS
   ========================================== */

QLabel {
    background-color: transparent;
    color: #14581D;
    padding: 0px;
    font-weight: bold;
}

/* ==========================================
   PUSH BUTTONS
   ========================================== */

QPushButton {
    background-color: #D53825;
    color: white;
    border: 3px solid #AA1010;
    border-radius: 12px;
    padding: 5px 10px;
    font-weight: bold;
    min-height: 22px;
}

QPushButton:hover {
    background-color: #FFA27F;
    border: 3px solid #D56659;
}

QPushButton:pressed {
    background-color: #AA1010;
    padding-top: 10px;
    padding-bottom: 6px;
}

/* ==========================================
   CHECK BOXES
   ========================================== */

QCheckBox {
    spacing: 9px;
    color: #14581D;
    padding: 4px;
}

QCheckBox::indicator {
    width: 20px;
    height: 20px;
    border: 3px solid #14581D;
    border-radius: 6px;
    background-color: white;
}

QCheckBox::indicator:checked {
    background-color: #4A8710;
    border: 3px solid #14581D;
    image: url(:checkmark.png);
    width: 20;
    height: 20;
}

QCheckBox::indicator:unchecked {
    background-color: white;
}

/* ==========================================
   LINE EDITS
   ========================================== */

QLineEdit {
    background-color: white;
    color: #14581D;
    border: 3px solid #14581D;
    border-radius: 10px;
    padding: 3px 5px;
    min-height: 24px;
}

/* ==========================================
   SCROLL AREAS
   ========================================== */

QScrollArea {
    background-color: #FFFDF8;
    border: 3px solid #14581D;
    border-radius: 14px;
}

QScrollArea QWidget {
    background-color: transparent;
}

QScrollBar:vertical {
    background-color: #F0E8D9;
    width: 14px;
    margin: 4px;
    border-radius: 7px;
}

QScrollBar::handle:vertical {
    background-color: #D6915D;
    min-height: 30px;
    border-radius: 6px;
}

QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0px;
}

QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
    background: transparent;
}

/* ==========================================
   SLIDERS
   ========================================== */

QSlider {
    min-height: 24px;
}

QSlider::groove:horizontal {
    height: 9px;
    background-color: #4A8710;
    border-radius: 4px;
}

QSlider::sub-page:horizontal {
    background-color: #D53825;
    border-radius: 4px;
}

QSlider::add-page:horizontal {
    background-color: #4A8710;
    border-radius: 4px;
}

QSlider::handle:horizontal {
    background-color: #D6915D;
    border: 3px solid #B67B4F;
    width: 18px;
    height: 18px;
    margin: -7px 0;
    border-radius: 11px;
}

/* ==========================================
   TAB WIDGET
   ========================================== */

QTabWidget {
    background: transparent;
    border: none;
}

QTabBar::tab {
    background-color: #4A8710;
    color: #14581D;

    border: 3px solid #14581D;
    border-bottom: none;

    border-top-left-radius: 11px;
    border-top-right-radius: 11px;

    padding: 8px 18px;
    min-height: 24px;

    margin-right: 4px;
    margin-top: 4px;

    font-weight: bold;
}

QTabBar::tab:selected {
    background-color: #D6915D;
    color: white;
    border-color: #B67B4F;
    margin-top: 0px;
}

QTabWidget::pane {
    background-color: #FFFDF8;
    border: 3px solid #14581D;

    border-top-left-radius: 0px;
    border-top-right-radius: 0px;
    border-bottom-left-radius: 12px;
    border-bottom-right-radius: 12px;
}

/* ==========================================
   TEXTBROWSER
   ========================================== */

QTextBrowser {
    background-color: white;
    color: #14581D;
    border-radius: 7px;
    padding: 8px 12px;
}

/* ==========================================
   TOOLTIP
   ========================================== */

QToolTip {
    background-color: #FFFDF8;
    color: #14581D;
    border: 2px solid #14581D;
    border-radius: 0px;
    padding: 2px 2px;
}

)";

  setStyleSheet(aboutStyle);

  // About tab
  QTextBrowser *aboutWidget = new QTextBrowser(this);
  aboutWidget->setReadOnly(true);
  aboutWidget->setOpenExternalLinks(true);
  aboutWidget->setFrameShape(QFrame::NoFrame);
  aboutWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  aboutWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QString aboutText = R"(

<h3>Boris bids you welcome!</h3>
<p>He is here to keep you company during all seasons of the year. He is a bit of a prankster, so keep an eye on him!</p>
<h4>Good to know:</h4>
<ul>
<li>You can pick Boris up if he is in your way by holding down the left mouse button while the pointer is on top of him.</li>
<li>Right-clicking any Boris brings up a menu where you can select from a plethora of Boris behaviours from categories that correspond to his current needs.</li>
<li>Using the mouse scroll-wheel on Boris will tickle him if he's not busy.</li>
<li>Right-clicking the Boris system tray icon brings up a menu that gives you access to information and important functions that will help you configure and take care of Boris. This is also where you can unlock items that Boris can interact with.</li>
</ul>
<h4>Health stats:</h4>
<ul>
<li>The 'Zz' icon signifies Boris' energy level.</li>
<li>The 'Speech bubble' icon shows Boris' need for social interaction.</li>
<li>The 'Smiley' icon indicates whether Boris is in a good mood and having fun.</li>
<li>The 'Knife and fork' icon shows how hungry Boris currently is.</li>
<li>The 'Toilet' icon is an indicator of Boris' need to go to the bathroom.</li>
</ul>
<h4>Other stats:</h4>
<ul>
<li>Boris can get dirty if he eats messy food or goes to the toilet without washing his hands.</li>
<li>He can also get bruised if you thrown him around too much.</li>
</ul>
<h4>How to get coins:</h4>
<p>It's just for fun! No real money involved. You can earn Boris coins by right-clicking Boris and choosing a behaviour from a category that corresponds to his current needs. The more needy Boris is for the particular category you select, the more coins you receive.</p>
<p>You can then spend the coins by right-clicking the Boris system tray icon, selecting the 'Items' submenu and choose the item you want to unlock. The item then becomes unlocked and can be selected any time you want.</p>
<h4>Where to get Boris:</h4>
<p>If your friends / colleagues become envious and would like a Boris of their own, simply ask them to visit <a href="https://github.com/muldjord/boris/releases">https://github.com/muldjord/boris/releases</a> and download it. He is completely free and open source. You can also download the full source code (GPLv3) in there if you like.</p>

)";
  aboutWidget->setHtml(aboutText);

  // Author tab
  QTextBrowser *authorWidget = new QTextBrowser(this);
  authorWidget->setReadOnly(true);
  authorWidget->setOpenExternalLinks(true);
  authorWidget->setFrameShape(QFrame::NoFrame);
  authorWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  authorWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QString authorText = R"(

<h3>Programming</h3>
<p>Lars Muldjord</p>

<h3>Graphics</h3>
<p>Lars Muldjord</p>

<h3>Sound</h3>
<p>Lars Muldjord</p>

<h4>Other notes</h4>
<p>Boris was programmed using the Qt framework (<a href="http://www.qt.io">http://www.qt.io</a>), SFML framework (<a href="https://www.sfml-dev.org">https://www.sfml-dev.org</a>) and C++.</p>
<p>Bug reports, suggestions and / or comments can be emailed to me at: muldjordlars@gmail.com.</p>
<p>If your friends / colleagues would like to get their own Boris, tell them to visit <a href="https://github.com/muldjord/boris/releases">https://github.com/muldjord/boris/releases</a>.</p>
<p>Copyright 2026 Lars Muldjord. This software is distributed under the terms of the GNU General Public License. Be sure to read the license in the 'License' tab or check out <a href="http://www.gnu.org/licenses/gpl-3.0.html">http://www.gnu.org/licenses/gpl-3.0.html</a>.</p>

)";

  authorWidget->setHtml(authorText);			   

  // License tab
  QTextBrowser *licenseWidget = new QTextBrowser(this);
  licenseWidget->setReadOnly(true);
  licenseWidget->setOpenExternalLinks(true);
  licenseWidget->setFrameShape(QFrame::NoFrame);
  licenseWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  licenseWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Read LICENSE data from file
  QFile file("LICENSE");
  QByteArray licenseText;
  if(file.open(QIODevice::ReadOnly)) {
    licenseText = file.readAll();
    file.close();
  } else {
    qWarning("ERROR: Couldn't find LICENSE file at the designated location.\n");
    licenseText = "ERROR: File not found... This means that someone has been fiddling with the files of this software, and someone might be violating the terms of the GPL. Go to the following location to read the license: http://www.gnu.org/licenses/gpl-3.0.html";
  }
  licenseWidget->setHtml(licenseText);

  QTabWidget *tabWidget = new QTabWidget;
  tabWidget->addTab(aboutWidget, "About");
  tabWidget->addTab(authorWidget, "Author");
  tabWidget->addTab(licenseWidget, "License");

  QPushButton *okButton = new QPushButton("Close and save");
  okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  okButton->setMinimumWidth(500);
  connect(okButton, &QPushButton::clicked, this, &About::saveAll);

  QLabel *sizeLabel = new QLabel("Boris size in pixels (32, 64, 128, 256 or 512):");
  sizeSlider = new StepSlider({32, 64, 128, 256, 512}, this);
  sizeSlider->setValue(settings.size);

  QLabel *clonesLabel = new QLabel("Boris clones (1-20 or 0 for random):");
  clonesSlider = new StepSlider({0, 20}, this);
  clonesSlider->setValue(settings.clones);

  enableItems = new QCheckBox("Allow items");
  enableItems->setToolTip("Boris will sometimes place various items around the desktop. Items will also be randomly spawned or created using the tray icon menu.");
  if(settings.items) {
    enableItems->setCheckState(Qt::Checked);
  }

  QLabel *itemSpawnIntervalLabel = new QLabel("Seconds between random item spawn (5-3600):");
  itemSpawnIntervalLineEdit = new QLineEdit();
  itemSpawnIntervalLineEdit->setToolTip("If items are enabled random items will be spawned around the desktop at this interval.");
  QIntValidator *itemSpawnIntervalValidator = new QIntValidator(5, 3600, this);
  itemSpawnIntervalLineEdit->setValidator(itemSpawnIntervalValidator);
  itemSpawnIntervalLineEdit->setText(QString::number(settings.itemSpawnInterval));

  QLabel *itemTimeoutLabel = new QLabel("Item timeout in seconds (10-3600):");
  itemTimeoutLineEdit = new QLineEdit();
  itemTimeoutLineEdit->setToolTip("Items will disappear after this amount of time.");
  QIntValidator *itemValidator = new QIntValidator(10, 3600, this);
  itemTimeoutLineEdit->setValidator(itemValidator);
  itemTimeoutLineEdit->setText(QString::number(settings.itemTimeout));

  QLabel *weatherLabel = new QLabel("Show weather for city (mouse over for help):");
  weatherLineEdit = new QLineEdit();
  weatherLineEdit->setToolTip("Type in a nearby city name or the openweathermap city id. Using a city id is the most stable method. To find the id for your city go to openweathermap.org and do a city lookup. The city id will be the last part of the website address.");
  weatherLineEdit->setText(settings.city);

  QLabel *weatherKeyLabel = new QLabel("OpenWeatherMap key (mouse over for help):");
  weatherKeyLineEdit = new QLineEdit();
  weatherKeyLineEdit->setToolTip("The weather functionality needs an API key to function. The default one should work.<br/>In case it doesn't, get a new one for free at openweathermap.org/appid.");
  weatherKeyLineEdit->setText(settings.key);

  QLabel *feedUrlLabel = new QLabel("RSS feed url (mouse over for help):");
  feedUrlLineEdit = new QLineEdit();
  feedUrlLineEdit->setToolTip("Type in any RSS feed url. Boris will sometimes update you on a title from this feed. You can click it to open it in the default browser.");
  feedUrlLineEdit->setText(settings.feedUrl);

  QLabel *statsLabel = new QLabel("Vitality stats:");
  statsComboBox = new QComboBox();
  statsComboBox->addItem("Always show", STATS_ALWAYS);
  statsComboBox->addItem("Show on critical levels and mouse over", STATS_CRITICAL);
  statsComboBox->addItem("Show only on mouse over", STATS_MOUSEOVER);
  statsComboBox->addItem("Never show", STATS_NEVER);
  statsComboBox->setCurrentIndex(statsComboBox->findData(settings.stats));

  QLabel *independenceLabel = new QLabel("Independence:");
  independenceSlider = new StepSlider({0, 100}, this);
  independenceSlider->setValue(settings.independence);

  makeInvincible = new QCheckBox("Make Boris invincible");
  if(settings.isInvincible) {
    makeInvincible->setCheckState(Qt::Checked);
  }

  enableSound = new QCheckBox("Enable sound (applied when dialog closes)");
  if(settings.sound) {
    enableSound->setCheckState(Qt::Checked);
  }

  enableBubbles = new QCheckBox("Enable Boris speech bubbles");
  if(settings.bubbles) {
    enableBubbles->setCheckState(Qt::Checked);
  }

  QLabel *volumeLabel = new QLabel("Sound volume:");
  volumeSlider = new StepSlider({0, 100}, this);
  volumeSlider->setValue(settings.volume);
  connect(volumeSlider, &StepSlider::valueChanged, this, &About::volumeChanged);

  showWelcome = new QCheckBox("Always show this dialog on startup");
  if(settings.showWelcome) {
    showWelcome->setCheckState(Qt::Checked);
  }

  QVBoxLayout *configLayout = new QVBoxLayout();
  configLayout->addWidget(sizeLabel);
  configLayout->addWidget(sizeSlider);
  configLayout->addWidget(clonesLabel);
  configLayout->addWidget(clonesSlider);
  configLayout->addWidget(enableItems);
  configLayout->addWidget(itemSpawnIntervalLabel);
  configLayout->addWidget(itemSpawnIntervalLineEdit);
  configLayout->addWidget(itemTimeoutLabel);
  configLayout->addWidget(itemTimeoutLineEdit);
  configLayout->addWidget(statsLabel);
  configLayout->addWidget(statsComboBox);
  configLayout->addWidget(independenceLabel);
  configLayout->addWidget(independenceSlider);
  configLayout->addWidget(makeInvincible);
  configLayout->addWidget(enableSound);
  configLayout->addWidget(volumeLabel);
  configLayout->addWidget(volumeSlider);
  configLayout->addWidget(weatherLabel);
  configLayout->addWidget(weatherLineEdit);
  configLayout->addWidget(weatherKeyLabel);
  configLayout->addWidget(weatherKeyLineEdit);
  configLayout->addWidget(enableBubbles);
  configLayout->addWidget(feedUrlLabel);
  configLayout->addWidget(feedUrlLineEdit);

  QHBoxLayout *hLayout1 = new QHBoxLayout;
  hLayout1->addLayout(configLayout);
  hLayout1->addWidget(tabWidget);

  QHBoxLayout *hLayout2 = new QHBoxLayout;
  hLayout2->addStretch(2);
  hLayout2->addWidget(showWelcome, 0);
  hLayout2->addWidget(okButton, 0);
  hLayout2->addStretch(2);

  QVBoxLayout *vLayout = new QVBoxLayout;
  vLayout->addLayout(hLayout1);
  vLayout->addLayout(hLayout2);
  
  setLayout(vLayout);
}

About::~About()
{
}

void About::volumeChanged(int value)
{
  soundMixer->setGlobalVolume(value);
}

void About::saveAll()
{
  if(showWelcome->isChecked()) {
    settings.showWelcome = true;
  } else {
    settings.showWelcome = false;
  }
  settings.size = sizeSlider->value();
  settings.sizeFactor = settings.size / 32.0;

  settings.clones = clonesSlider->value();

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
  if(makeInvincible->isChecked()) {
    settings.isInvincible = true;
  } else {
    settings.isInvincible = false;
  }
  if(enableSound->isChecked()) {
    settings.sound = true;
  } else {
    settings.sound = false;
  }
  settings.volume = volumeSlider->value();
  if(enableBubbles->isChecked()) {
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
  iniSettings->setValue("clones", settings.clones);
  iniSettings->setValue("size", settings.size);
  iniSettings->setValue("items", settings.items);
  iniSettings->setValue("item_timeout", settings.itemTimeout);
  iniSettings->setValue("item_spawn_timer", settings.itemSpawnInterval);
  iniSettings->setValue("independence", settings.independence);
  iniSettings->setValue("makeInvincible", settings.isInvincible);
  iniSettings->setValue("bubbles", settings.bubbles);
  iniSettings->setValue("sound", settings.sound);
  iniSettings->setValue("volume", settings.volume);
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
