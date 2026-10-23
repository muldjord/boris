/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            bubble.cpp
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

#include "bubble.h"

#include <stdio.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QFile>
#include <QDesktopServices>
#include <QPainter>

Bubble::Bubble(Settings &settings) : settings(settings)
{
  bubbleTopLeft = settings.getPixmap("bubble.png").copy(0, 0, 6, 6);
  bubbleTop = settings.getPixmap("bubble.png").copy(6, 0, 1, 6);
  bubbleTopRight = settings.getPixmap("bubble.png").copy(7, 0, 6, 6);
  bubbleLeft = settings.getPixmap("bubble.png").copy(0, 6, 6, 1);
  bubbleRight = settings.getPixmap("bubble.png").copy(7, 6, 6, 1);
  bubbleBottomLeft = settings.getPixmap("bubble.png").copy(0, 7, 6, 6);
  bubbleBottom = settings.getPixmap("bubble.png").copy(6, 7, 1, 6);
  bubbleBottomRight = settings.getPixmap("bubble.png").copy(7, 7, 6, 6);

  bubbleSpeech = settings.getPixmap("bubble_speech.png");
  bubbleThought = settings.getPixmap("bubble_thought.png");
}

Bubble::~Bubble()
{
}

void Bubble::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->drawPixmap(0, 0, rendered);
}

QRectF Bubble::boundingRect() const
{
  return QRectF(0, 0, rendered.width(), rendered.height());
}


void Bubble::initBubble(const int &hyper,
                        const QString &sentence,
                        const QString &type,
                        const QUrl &rssUrl)
{
  if(rssUrl.isValid()) {
    this->rssUrl = rssUrl;
  } else {
    this->rssUrl.clear();
  }
  int sentenceWidth = 0;
  int sentenceHeight = 0;
  for(const auto &ch: sentence.split("")) {
    if(settings.pixelFont.contains(ch)) {
      sentenceWidth += settings.pixelFont[ch].width();
      if(settings.pixelFont[ch].height() > sentenceHeight) {
        sentenceHeight = settings.pixelFont[ch].height();
      }
    }
  }
  QPixmap sentencePixmap(sentenceWidth, sentenceHeight);
  sentencePixmap.fill(Qt::white);
  QPainter painter(&sentencePixmap);
  int idx = 0;
  for(const auto &ch: sentence.split("")) {
    QImage charImage;
    if(settings.pixelFont.contains(ch)) {
      charImage = settings.pixelFont[ch];
    } else {
      continue;
    }
    painter.drawImage(idx, 0, charImage);
    idx += charImage.width();
  }
  painter.end();

  int width = 6 + sentencePixmap.width() + 6;
  int height = 6 + sentencePixmap.height() + 6 + 6;

  rendered = QPixmap(width, height);
  rendered.fill(Qt::transparent);
  painter.begin(&rendered);

  painter.drawPixmap(0, 0, bubbleTopLeft);

  bubbleTop = bubbleTop.scaled(sentencePixmap.width(), bubbleTop.height());
  painter.drawPixmap(6, 0, bubbleTop);

  painter.drawPixmap(sentencePixmap.width() + 6, 0, bubbleTopRight);

  bubbleLeft = bubbleLeft.scaled(bubbleLeft.width(), sentencePixmap.height());
  painter.drawPixmap(0, 6, bubbleLeft);

  painter.drawPixmap(6, 6, sentencePixmap);

  bubbleRight = bubbleRight.scaled(bubbleRight.width(), sentencePixmap.height());
  painter.drawPixmap(sentencePixmap.width() + 6, 6, bubbleRight);
 
  painter.drawPixmap(0, sentencePixmap.height() + 6, bubbleBottomLeft);

  bubbleBottom = bubbleBottom.scaled(sentencePixmap.width(), bubbleBottom.height());
  painter.drawPixmap(6, sentencePixmap.height() + 6, bubbleBottom);

  painter.drawPixmap(sentencePixmap.width() + 6, sentencePixmap.height() + 6, bubbleBottomRight);

  if(type == "_thought") {
    painter.drawPixmap(width / 2, height - 8, bubbleThought);
  } else {
    painter.drawPixmap(width / 2, height - 8, bubbleSpeech);
  }

  painter.end();

  int duration = 1000 + (sentence.length() * 100);
  duration = duration - (duration / 100.0 * hyper);

  setPos(0 - (width / 2) + 24, -16);
  update();

  if(settings.bubbles) {
    show();
    QTimer::singleShot(duration, this, &Bubble::hideBubble);
  }
}

void Bubble::hideBubble()
{
  hide();
}

/*
void Bubble::moveBubble(const int &x, const int &y, const int &borisSize)
{
  move((x + (borisSize * 0.7)) - (width() * 0.5), y);
}
*/
/*
void Bubble::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && rssUrl.isValid()) {
    QDesktopServices::openUrl(rssUrl);
  }
  event->ignore();
}
*/
