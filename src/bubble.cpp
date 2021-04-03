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
#include <QHBoxLayout>
#include <QTimer>
#include <QFile>
#include <QDesktopServices>
#include <QPainter>

extern QMap<QString, QImage> pfont;

Bubble::Bubble(Settings *settings) : settings(settings)
{
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background:transparent");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QGraphicsScene *scene = new QGraphicsScene();
  setScene(scene);

  bubbleSprite = this->scene()->addPixmap(QPixmap());
  bubbleSprite->setPos(0, 0);
}

Bubble::~Bubble()
{
}

int Bubble::initBubble(const int x, const int y,
                         const int &borisSize,
                         const QString &bubbleText,
                         const QString &bubbleType,
                         const QUrl &rssUrl)
{
  if(rssUrl.isValid()) {
    this->rssUrl = rssUrl;
  } else {
    this->rssUrl.clear();
  }
  int textWidth = 0;
  int textHeight = 0;
  for(const auto &ch: bubbleText) {
    if(pfont.contains(ch)) {
      textWidth += pfont[ch].width();
      if(pfont[ch].height() > textHeight) {
        textHeight = pfont[ch].height();
      }
    }
  }
  QImage textImage(textWidth, textHeight, QImage::Format_ARGB32_Premultiplied);
  textImage.fill(Qt::white);
  QPainter painter;
  painter.begin(&textImage);
  int idx = 0;
  for(const auto &ch: bubbleText.split("")) {
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
  QImage bubbleSpeech(":bubble_speech.png");
  QImage bubbleThought(":bubble_thought.png");
  QImage bubbleImage(textImage.width() + bubbleAtlas.width() - 1 - 2,
                     textImage.height() + bubbleAtlas.height() - 1 + bubbleSpeech.height() - 2,
                     QImage::Format_ARGB32_Premultiplied);
  bubbleImage.fill(Qt::transparent);
  painter.begin(&bubbleImage);
  painter.drawImage(0, 0, bubbleAtlas.copy(0, 0, 6, 6));
  painter.drawImage(6, 0, bubbleAtlas.copy(6, 0, 1, 6).scaled(textImage.width() - 2, 6));
  painter.drawImage(6 + textImage.width() - 2, 0, bubbleAtlas.copy(7, 0, 6, 6));
  painter.drawImage(0, 6, bubbleAtlas.copy(0, 6, 6, 1).scaled(6, textImage.height() - 2));
  painter.drawImage(6 + textImage.width() - 2, 6, bubbleAtlas.copy(7, 6, 6, 1).scaled(6, textImage.height() - 2));
  painter.drawImage(0, 6 + textImage.height() - 2, bubbleAtlas.copy(0, 7, 6, 6));
  painter.drawImage(6, 6 + textImage.height() - 2, bubbleAtlas.copy(6, 7, 1, 6).scaled(textImage.width() - 2, 6));
  painter.drawImage(6 + textImage.width() - 2, 6 + textImage.height() - 2, bubbleAtlas.copy(7, 7, 6, 6));
  if(bubbleType == "_thought") {
    painter.drawImage(bubbleImage.width() / 2, 6 + 6 + textImage.height() - 4, bubbleThought);
  } else {
    painter.drawImage(bubbleImage.width() / 2, 6 + 6 + textImage.height() - 4, bubbleSpeech);
  }
  // Draw actual text last, to make sure borders don't overlap it
  painter.drawImage(6, 6, textImage);
  painter.end();

  bubbleSprite->setPixmap(QPixmap::fromImage(bubbleImage));
  scene()->setSceneRect(0.0, 0.0, bubbleImage.width(), bubbleImage.height());
  setFixedSize(bubbleImage.width() * borisSize / 32.0, bubbleImage.height() * borisSize / 32.0);
  resetTransform();
  scale(borisSize / 32.0, borisSize / 32.0);
  int duration = 2000 + (bubbleText.length() * 175);

  if(settings->bubbles) {
    show();
    moveBubble(x, y, borisSize);
    QTimer::singleShot(duration, this, &Bubble::hide);
  }

  return duration;
}

void Bubble::moveBubble(const int &x, const int &y, const int &borisSize)
{
  move((x + (borisSize * 0.7)) - (width() * 0.5), y);
}

void Bubble::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && rssUrl.isValid()) {
    QDesktopServices::openUrl(rssUrl);
  }
  event->ignore();
}
