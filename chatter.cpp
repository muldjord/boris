/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            chatter.cpp
 *
 *  Thu Oct 27 18:47:00 CEST 2016
 *  Copyright 2016 Lars Muldjord
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

#include "chatter.h"

#include <stdio.h>
#include <QSettings>
#include <QHBoxLayout>
#include <QTimer>
#include <QFile>
#include <QDesktopServices>

//#define DEBUG

extern QSettings *settings;

Chatter::Chatter(QList<ChatLine> *chatLines, QWidget *parent) : QWidget(parent)
{
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setStyleSheet("border-image: url(:bubble.png) 12 12 24 12;"
                "border-top: 12px transparent;"
                "border-bottom: 24px transparent;"
                "border-right: 12px transparent;"
                "border-left: 12px transparent;");

  this->chatLines = chatLines;
  
  chatterLabel = new QLabel;
  bubbleTip = new QLabel(this);
  bubbleTip->setPixmap(QPixmap(":bubble_tip.png"));
  bubbleTip->setStyleSheet("QLabel { border-top: 0px transparent;"
                           "border-right: 0px transparent;"
                           "border-bottom: 0px transparent;"
                           "border-left: 0px transparent; }");
  bubbleTip->setFixedSize(18, 16);
  bubbleTip->setAttribute(Qt::WA_TranslucentBackground);
  
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(chatterLabel);
  setLayout(layout);
}

Chatter::~Chatter()
{
}

QPair<QString, int> Chatter::initChatter(int x, int y, int borisSize)
{
  currentLine = qrand() % chatLines->size();
  QString chatType = "_complain";
  if(!chatLines->isEmpty()) {
    chatType = chatLines->at(currentLine).type;
    chatterLabel->setText(chatLines->at(currentLine).text);
  } else {
    chatterLabel->setText("I have nothing to say...");
  }
  int duration = 2000 + (chatterLabel->text().length() * 120);

  if(settings->value("chatter", "true").toBool() == true) {
    show();
    move((x + (borisSize / 8 * 7)) - (width() / 2), y + (borisSize / 10 * 9) - height());
    bubbleTip->move(width() / 2, height() - 27);
    bubbleTip->raise();
    QTimer::singleShot(duration, this, SLOT(hide()));
  }

  return QPair<QString, int>(chatType, duration);
}

void Chatter::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && chatLines->at(currentLine).url.isValid()) {
    QDesktopServices::openUrl(chatLines->at(currentLine).url);
  }
  event->ignore();
}
