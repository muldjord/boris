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
#include <QHBoxLayout>
#include <QTimer>
#include <QFile>
#include <QDesktopServices>
#include <QPainter>

extern QMap<QChar, QImage> pfont;

Chatter::Chatter(Settings *settings, QWidget *parent) : QLabel(parent)
{
  this->settings = settings;

  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background:transparent");
}

Chatter::~Chatter()
{
}

QPair<QString, int> Chatter::initChatter(const int x, const int y, const int &borisSize)
{
  QString chatType = "_complain";
  QString chatText = "I'm speechless...";
  if(!settings->chatLines.isEmpty()) {
    currentLine = qrand() % settings->chatLines.count();
    chatType = settings->chatLines.at(currentLine).type;
    chatText = settings->chatLines.at(currentLine).text;
  }

  int textWidth = 0;
  int textHeight = 0;
  for(const auto &ch: chatText) {
    if(pfont.contains(ch)) {
      textWidth += pfont[ch].width();
      if(pfont[ch].height() > textHeight) {
        textHeight = pfont[ch].height();
      }
    }
  }
  QImage bubbleText(textWidth, textHeight, QImage::Format_ARGB32_Premultiplied);
  bubbleText.fill(Qt::white);
  QPainter painter;
  painter.begin(&bubbleText);
  int idx = 0;
  for(const auto &ch: chatText) {
    QImage charImage;
    if(pfont.contains(ch)) {
      charImage = pfont[ch];
    } else {
      continue;
    }
    painter.drawImage(idx, 0, charImage);
    idx += charImage.width();
  }
  painter.end();

  QImage bubbleAtlas(":bubble.png");
  QImage bubbleTip(":bubble_tip.png");
  QImage bubbleImage(bubbleText.width() + bubbleAtlas.width() - 1 - 2,
                     bubbleText.height() + bubbleAtlas.height() - 1 + bubbleTip.height() - 2,
                     QImage::Format_ARGB32_Premultiplied);
  bubbleImage.fill(Qt::transparent);
  painter.begin(&bubbleImage);
  painter.drawImage(0, 0, bubbleAtlas.copy(0, 0, 6, 6));
  painter.drawImage(6, 0, bubbleAtlas.copy(6, 0, 1, 6).scaled(bubbleText.width() - 2, 6));
  painter.drawImage(6 + bubbleText.width() - 2, 0, bubbleAtlas.copy(7, 0, 6, 6));
  painter.drawImage(0, 6, bubbleAtlas.copy(0, 6, 6, 1).scaled(6, bubbleText.height() - 2));
  painter.drawImage(6, 6, bubbleText);
  painter.drawImage(6 + bubbleText.width() - 2, 6, bubbleAtlas.copy(7, 6, 6, 1).scaled(6, bubbleText.height() - 2));
  painter.drawImage(0, 6 + bubbleText.height() - 2, bubbleAtlas.copy(0, 7, 6, 6));
  painter.drawImage(6, 6 + bubbleText.height() - 2, bubbleAtlas.copy(6, 7, 1, 6).scaled(bubbleText.width() - 2, 6));
  painter.drawImage(6 + bubbleText.width() - 2, 6 + bubbleText.height() - 2, bubbleAtlas.copy(7, 7, 6, 6));
  painter.drawImage(bubbleImage.width() / 2, 6 + 6 + bubbleText.height() - 4, bubbleTip);
  painter.end();
  
  bubbleImage = bubbleImage.scaledToWidth(bubbleImage.width() * 2);
  setPixmap(QPixmap::fromImage(bubbleImage));
  int duration = 2000 + (chatText.length() * 120);

  if(settings->chatter) {
    show();
    move((x + (borisSize / 8 * 7)) - (width() / 2), y + (borisSize / 10 * 9) - height());
    QTimer::singleShot(duration, this, &Chatter::hide);
  }

  return QPair<QString, int>(chatType, duration);
}

void Chatter::moveChatter(const int x, const int y, const int &borisSize)
{
  move((x + (borisSize / 8 * 7)) - (width() / 2), y + (borisSize / 10 * 9) - height());
}

void Chatter::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && settings->chatLines.at(currentLine).url.isValid()) {
    QDesktopServices::openUrl(settings->chatLines.at(currentLine).url);
  }
  event->ignore();
}
