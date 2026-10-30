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

#include "bubble.h"

#include "settings.h"

#include <QDesktopServices>
#include <QGraphicsSceneMouseEvent>
#include <QImage>
#include <QPainter>
#include <QRectF>
#include <QString>

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

  bubbleThought = settings.getPixmap("bubble_thought.png");
  bubbleSpeech = settings.getPixmap("bubble_speech.png");

  rendered.fill(Qt::transparent);

  hideTimer.setSingleShot(true);
  connect(&hideTimer, &QTimer::timeout, [this]() { hide(); });
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

  prepareGeometryChange(); // Needed to make sure the parent scene has its geometry index updated or something... Otherwise boundingRect might report faulty size.
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
    hideTimer.setInterval(duration);
    hideTimer.start();
  }
}

void Bubble::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if(event->button() == Qt::LeftButton && rssUrl.isValid()) {
    QDesktopServices::openUrl(rssUrl);
  }
  event->ignore();
}
