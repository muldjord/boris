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
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::ToolTip);
  setFrameShape(QFrame::NoFrame);
  setStyleSheet("background:transparent");
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setTransformationAnchor(QGraphicsView::NoAnchor);
  setResizeAnchor(QGraphicsView::NoAnchor);
  
  QGraphicsScene *scene = new QGraphicsScene();
  setScene(scene);

  bubbleText = this->scene()->addPixmap(QPixmap());
  bubbleText->setPos(6, 6);

  bubbleTopLeft = this->scene()->addPixmap(settings.getPixmap("bubble.png").copy(0, 0, 6, 6));
  bubbleTopLeft->setPos(0, 0);
  bubbleTop = this->scene()->addPixmap(settings.getPixmap("bubble.png").copy(6, 0, 1, 6));
  bubbleTop->setPos(6, 0);
  bubbleTopRight = this->scene()->addPixmap(settings.getPixmap("bubble.png").copy(7, 0, 6, 6));
  bubbleLeft = this->scene()->addPixmap(settings.getPixmap("bubble.png").copy(0, 6, 6, 1));
  bubbleLeft->setPos(0, 6);
  bubbleRight = this->scene()->addPixmap(settings.getPixmap("bubble.png").copy(7, 6, 6, 1));
  bubbleBottomLeft = this->scene()->addPixmap(settings.getPixmap("bubble.png").copy(0, 7, 6, 6));
  bubbleBottom = this->scene()->addPixmap(settings.getPixmap("bubble.png").copy(6, 7, 1, 6));
  bubbleBottomRight = this->scene()->addPixmap(settings.getPixmap("bubble.png").copy(7, 7, 6, 6));

  bubbleSpeech = this->scene()->addPixmap(settings.getPixmap("bubble_speech.png"));
  bubbleSpeech->setZValue(1.0);
  bubbleThought = this->scene()->addPixmap(settings.getPixmap("bubble_thought.png"));
  bubbleThought->setZValue(1.0);
  
  hideTimer.setSingleShot(true);
  connect(&hideTimer, &QTimer::timeout, this, &Bubble::hide);
}

Bubble::~Bubble()
{
}

int Bubble::initBubble(const int x, const int y,
                       const int &borisSize,
                       const int &hyper,
                       const QString &text,
                       const QString &type,
                       const QUrl &rssUrl)
{
  if(rssUrl.isValid()) {
    this->rssUrl = rssUrl;
  } else {
    this->rssUrl.clear();
  }
  int textWidth = 0;
  int textHeight = 0;
  for(const auto &ch: text.split("")) {
    if(settings.pixelFont.contains(ch)) {
      textWidth += settings.pixelFont[ch].width();
      if(settings.pixelFont[ch].height() > textHeight) {
        textHeight = settings.pixelFont[ch].height();
      }
    }
  }
  QImage textImage(textWidth, textHeight, QImage::Format_ARGB32_Premultiplied);
  textImage.fill(Qt::white);
  QPainter painter;
  painter.begin(&textImage);
  int idx = 0;
  for(const auto &ch: text.split("")) {
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

  bubbleText->setPixmap(QPixmap::fromImage(textImage));

  int width = 6 + textImage.width() + 6;
  int height = 6 + textImage.height() + 6 + 6;
  
  if(type == "_thought") {
    bubbleSpeech->hide();
    bubbleThought->setPos(width / 2, height - 8);
    bubbleThought->show();
  } else {
    bubbleThought->hide();
    bubbleSpeech->setPos(width / 2, height - 8);
    bubbleSpeech->show();
  }

  QTransform scaleHorizontal(textImage.width(), 0.0, 0.0, 1.0, 0.0, 0.0);
  QTransform scaleVertical(1.0, 0.0, 0.0, textImage.height(), 0.0, 0.0);
  bubbleTop->setTransform(scaleHorizontal);
  bubbleBottom->setTransform(scaleHorizontal);
  bubbleLeft->setTransform(scaleVertical);
  bubbleRight->setTransform(scaleVertical);

  bubbleTopRight->setPos(textImage.width() + 6, 0);
  bubbleRight->setPos(textImage.width() + 6, 6);
  bubbleBottomLeft->setPos(0, textImage.height() + 6);
  bubbleBottom->setPos(6, textImage.height() + 6);
  bubbleBottomRight->setPos(textImage.width() + 6, textImage.height() + 6);

  double scaleFactor = settings.sizeFactor;
  resetTransform();
  scale(scaleFactor, scaleFactor);
  scene()->setSceneRect(0, 0, width * scaleFactor, height * scaleFactor);
  setFixedSize(width * scaleFactor, height * scaleFactor);
  int duration = 1000 + (text.length() * 100);
  duration = duration - (duration / 100.0 * hyper);
  if(settings.bubbles) {
    show();
    moveBubble(x, y, borisSize);
    hideTimer.setInterval(duration);
    hideTimer.start();
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
