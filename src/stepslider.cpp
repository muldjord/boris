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

#include "stepslider.h"

#include <cmath>

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QSlider>
#include <QStyle>
#include <QStyleOptionSlider>

InternalSlider::InternalSlider(const QList<int> &allowedValues, QWidget *parent)
  : allowedValues(allowedValues), QSlider(Qt::Horizontal, parent)
{
  setMinimum(allowedValues.first());
  setMaximum(allowedValues.last());

  setSingleStep(1);
  setPageStep(1);
}

int InternalSlider::nearestValue(int value) const
{
  int nearest = allowedValues.first();
  int minDistance = abs(value - nearest);

  for(int v : allowedValues) {
    int distance = abs(value - v);
    
    if(distance < minDistance) {
      minDistance = distance;
      nearest = v;
    }
  }
  
  return nearest;
}

void InternalSlider::mousePressEvent(QMouseEvent *event)
{
  QSlider::mousePressEvent(event);
  setValue(nearestValue(value()));
}

void InternalSlider::mouseMoveEvent(QMouseEvent *event)
{
  QSlider::mouseMoveEvent(event);
  setValue(nearestValue(value()));
}

void InternalSlider::keyPressEvent(QKeyEvent *event)
{
  int index = allowedValues.indexOf(value());

  if(event->key() == Qt::Key_Right ||
     event->key() == Qt::Key_Up) {
    index = qMin(index + 1, allowedValues.size() - 1);
    setValue(allowedValues[index]);
    
    return;
  }
  
  if(event->key() == Qt::Key_Left ||
     event->key() == Qt::Key_Down) {
    index = qMax(index - 1, 0);
    setValue(allowedValues[index]);

    return;
  }
  
  QSlider::keyPressEvent(event);
}

StepSlider::StepSlider(const QList<int> &allowedValues, QWidget *parent)
  : QWidget(parent)
{
  setContentsMargins(0, 0, 0, 0);
  slider = new InternalSlider(allowedValues, parent);
  connect(slider, &QSlider::valueChanged, this, &StepSlider::setValue);
  
  label = new QLineEdit;
  label->setReadOnly(true);
  label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(slider);
  layout->addWidget(label);

  setLayout(layout);
}

void StepSlider::setValue(const int &value)
{
  slider->setValue(value);
  label->setText(QString::number(value));
}

int StepSlider::value()
{
  return slider->value();
}
