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

#include "shadow.h"

#include "settings.h"

#include <algorithm>

#include <QImage>
#include <QPainter>
#include <QRectF>
#include <QRgb>

Shadow::Shadow(Settings &settings)
  : settings(settings)
{
  setFlag(QGraphicsItem::ItemStacksBehindParent);

  origShadow = settings.getPixmap("shadow.png");
}

Shadow::~Shadow()
{
}

void Shadow::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->drawPixmap(0, 0, rendered);
}

QRectF Shadow::boundingRect() const
{
  return QRectF(0, 0, rendered.width(), rendered.height());
}

void Shadow::updateShadow(const QPixmap &sprite, const int &altitude, const bool &falling)
{
  QImage image = sprite.toImage();
  int bottom = image.height() - 1;
  const QRgb *pixelBits = reinterpret_cast<const QRgb*>(image.constBits());
  for(int pixel = (image.width() * image.height()) - 1; pixel >= 0; --pixel) {
    if(qAlpha(pixelBits[pixel]) != 0) {
      bottom = (pixel / sprite.width());
      break;
    }
  }

  int firstX = image.width();
  int lastX = 0;
  int shadowYDelta = 8; // From bottom and then how many pixels upwards should be used to determine the shadow width
  for(int y = std::clamp(bottom - shadowYDelta + 1, 0, 31); y <= bottom; ++y) {
    const QRgb *yBits = reinterpret_cast<const QRgb*>(image.constScanLine(y));
    int rowLastX = 0;
    for(int x = 0; x < sprite.width(); ++x) {
      if(qAlpha(yBits[x]) != 0) {
        if(x < firstX) {
          firstX = x;
        }
        rowLastX = x;
      }
    }
    if(rowLastX > lastX) {
      lastX = rowLastX;
    }
  }

  // In case no non-transparent pixels where found at all
  QImage shadow(32, 32, QImage::Format_ARGB32);
  shadow.fill(Qt::transparent);
  if(firstX < lastX) {
    double altitudeFactor = (settings.desktopHeight - altitude) + 1 / settings.desktopHeight; // +1 to avoid division by zero
    int shadowWidth = lastX - firstX + 3;
    QPainter painter(&shadow);
    painter.setOpacity(0.3);
    if(falling) {
      painter.drawImage(firstX - 1 + ((shadowWidth - (shadowWidth * altitudeFactor)) / 2), 0, origShadow.toImage().scaled(shadowWidth * altitudeFactor, origShadow.height()));
    } else {
      painter.drawImage(firstX - 1, bottom - image.height() + 1, origShadow.toImage().scaled(shadowWidth, origShadow.height()));
    }
    painter.end();
  }

  rendered = QPixmap::fromImage(shadow);

  update();
}
