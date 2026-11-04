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
  : QSlider(Qt::Horizontal, parent)
{
  this->allowedValues = allowedValues;
  if(allowedValues.length() == 2) {
    this->allowedValues.clear();
    for(int a = allowedValues.first(); a <= allowedValues.last(); ++a) {
      this->allowedValues.append(a);
    }
  }

  setMinimum(this->allowedValues.first());
  setMaximum(this->allowedValues.last());

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

void InternalSlider::wheelEvent(QWheelEvent *event)
{
  event->accept();
}

void InternalSlider::keyPressEvent(QKeyEvent *event)
{
  event->accept();
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
  emit valueChanged(value);
  label->setText(QString::number(value));
}

int StepSlider::value()
{
  return slider->value();
}
