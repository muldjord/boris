/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            scripthandler.cpp
 *
 *  Fri Oct 11 20:00:00 CEST 2019
 *  Copyright 2019 Lars Muldjord
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

#include "scripthandler.h"

#include <stdio.h>

extern QList<Behaviour> behaviours;
extern QMap<QChar, QImage> pfont;

ScriptHandler::ScriptHandler(QImage *image, bool *drawing,
                             Boris *boris, Settings *settings,
                             Stats *stats)
{
  this->boris = boris;
  this->image = image;
  this->drawing = drawing;
  this->settings = settings;
  this->stats = stats;
}

void ScriptHandler::runScript(int &stop, const Script &script)
{
  for(const auto &command: script.commands) {
    printf("CODE: '%s'\n", command.toStdString().c_str());
    QList<QString> parameters = command.split(" ", QString::KeepEmptyParts);
    runCommand(parameters, stop, script);
    if(stop) {
      return;
    }
  }
}

void ScriptHandler::runCommand(QList<QString> &parameters, int &stop, const Script &script)
{
  if(parameters.first() == "var") {
    handleVar(parameters);
  } else if(parameters.first() == "stat") {
    handleStat(parameters);
  } else if(parameters.first() == "if") {
    handleIf(parameters, stop, script);
  } else if(parameters.first() == "goto") {
    handleGoto(parameters, stop);
  } else if(parameters.first() == "print") {
    handlePrint(parameters);
  } else if(parameters.first() == "draw") {
    handleDraw(parameters);
  } else if(parameters.first() == "break") {
    handleBreak(stop);
  } else if(parameters.first() == "call") {
    handleCall(parameters, stop);
  } else if(parameters.first() == "sound") {
    handleSound(parameters);
  }
}

void ScriptHandler::handleIf(QList<QString> &parameters, int &stop, const Script &script)
{
  parameters.removeFirst(); // Remove 'if'

  bool isTrue = false;
  bool compare = true;
  condition(parameters, isTrue, compare);
  if(isTrue) {
    if(parameters.first() == "then") {
      parameters.removeFirst();
    }
    if(parameters.first().left(2) == "##") {
      printf("\n");
      runScript(stop, script.blocks[parameters.first()]);
    } else {
      runCommand(parameters, stop, script);
    }
  } else if(parameters.contains("else") &&
            parameters.first() != "then") {
    while(parameters.takeFirst() != "else") {
    }
    if(parameters.first().left(2) == "##") {
      printf("\n");
      runScript(stop, script.blocks[parameters.first()]);
    } else {
      runCommand(parameters, stop, script);
    }
  } else {
    printf("Condition not met\n");
  }
}

void ScriptHandler::condition(QList<QString> &parameters, bool &isTrue, bool &compare)
{
  int compareFrom = getValue(parameters);
  QString op = parameters.first();
  parameters.removeFirst(); // Remove operator
  int compareTo = getValue(parameters);
  
  if(compare) {
    isTrue = false;
    if(op == "<") {
      if(compareFrom < compareTo) {
        isTrue = true;
      }
    } else if(op == ">") {
      if(compareFrom > compareTo) {
        isTrue = true;
      }
    } else if(op == "<=") {
      if(compareFrom <= compareTo) {
        isTrue = true;
      }
    } else if(op == ">=") {
      if(compareFrom >= compareTo) {
        isTrue = true;
      }
    } else if(op == "=") {
      if(compareFrom == compareTo) {
        isTrue = true;
      }
    } else if(op == "==") {
      if(compareFrom == compareTo) {
        isTrue = true;
      }
    } else if(op == "!=") {
      if(compareFrom != compareTo) {
        isTrue = true;
      }
    }
  }

  printf("%d %s %d", compareFrom, op.toStdString().c_str(), compareTo);

  if(parameters.count() >= 1) {
    if(parameters.first() == "or") {
      printf(" or ");
      parameters.removeFirst();
      if(isTrue) {
        compare = false;
      }
      condition(parameters, isTrue, compare);
      return;
    } else if(parameters.first() == "and") {
      printf(" and ");
      parameters.removeFirst();
      if(!isTrue) {
        compare = false;
      }
      condition(parameters, isTrue, compare);
      return;
    } else {
      printf(", ");
    }
  }
}

void ScriptHandler::handleGoto(QList<QString> &parameters, int &stop)
{
  parameters.removeFirst(); // Remove 'goto'

  if(behaviours.at(boris->curBehav).labels.contains(parameters.first())) {
    printf("Going to label '%s' at frame %d\n", parameters.first().toStdString().c_str(), behaviours.at(boris->curBehav).labels[parameters.first()]);
    boris->curFrame = behaviours.at(boris->curBehav).labels[parameters.first()];
  } else {
    printf("Going to '%s', ERROR: Unknown label\n", parameters.first().toStdString().c_str());
  }
  stop = 1; // Will end the script execution for this frame
}

void ScriptHandler::handleVar(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'var'

  QString variable = parameters.first();
  parameters.removeFirst(); // Remove variable
  QString op = parameters.first();
  parameters.removeFirst(); // Remove operator
  int number = getValue(parameters);

  if(op == "=") {
    boris->scriptVars[variable] = number;
  } else if(op == "+=") {
    boris->scriptVars[variable] += number;
  } else if(op == "-=") {
    boris->scriptVars[variable] -= number;
  } else if(op == "*=") {
    boris->scriptVars[variable] *= number;
  } else if(op == "/=") {
    boris->scriptVars[variable] /= number;
  }

  printf("%s = %d\n", variable.toStdString().c_str(), boris->scriptVars[variable]);
}

void ScriptHandler::handleStat(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'stat'

  QString statType = parameters.first();
  parameters.removeFirst(); // Remove stat type
  QString op = parameters.first();
  parameters.removeFirst(); // Remove operator
  int number = getValue(parameters);

  int *stat = nullptr;
  if(statType == "hyper") {
    stat = &boris->hyperQueue;
  } else if(statType == "health") {
    stat = &boris->healthQueue;
  } else if(statType == "energy") {
    stat = &boris->energyQueue;
  } else if(statType == "hunger") {
    stat = &boris->hungerQueue;
  } else if(statType == "bladder") {
    stat = &boris->bladderQueue;
  } else if(statType == "social") {
    stat = &boris->socialQueue;
  } else if(statType == "fun") {
    stat = &boris->funQueue;
  } else if(statType == "hygiene") {
    stat = &boris->hygieneQueue;
  }
  if(stat != nullptr) {
    if(op == "+=") {
      *stat += number;
    } else if(op == "-=") {
      *stat -= number;
    }
    printf("%s %s %d\n", statType.toStdString().c_str(),
           op.toStdString().c_str(), number);
  } else {
    printf("%s, ERROR: Unknown stat\n", statType.toStdString().c_str());
  }
}

void ScriptHandler::handlePrint(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'print'

  if(boris->scriptVars.contains(parameters.first())) {
    printf("%s = %d\n", parameters.first().toStdString().c_str(), boris->scriptVars[parameters.first()]);
  } else {
    printf("%s, ERROR: Unknown variable\n", parameters.first().toStdString().c_str());
  }
}

void ScriptHandler::handleDraw(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'draw'

  if(parameters.count() >= 1) {
    if(parameters.first() == "begin") {
      parameters.removeFirst(); // Remove 'begin'
      *drawing = true;
      return;
    } else if(parameters.first() == "end") {
      parameters.removeFirst(); // Remove 'end'
      image->fill(Qt::transparent);
      *drawing = false;
      return;
    }
    QPainter painter;
    QString colorString = parameters.first();
    Qt::GlobalColor color = Qt::transparent;
    if(colorString == "black") {
      color = Qt::black;
    } else if(colorString == "white") {
      color = Qt::white;
    } else if(colorString == "cyan") {
      color = Qt::cyan;
    } else if(colorString == "darkcyan") {
      color = Qt::darkCyan;
    } else if(colorString == "red") {
      color = Qt::red;
    } else if(colorString == "darkred") {
      color = Qt::darkRed;
    } else if(colorString == "magenta") {
      color = Qt::magenta;
    } else if(colorString == "darkmagenta") {
      color = Qt::darkMagenta;
    } else if(colorString == "green") {
      color = Qt::green;
    } else if(colorString == "darkgreen") {
      color = Qt::darkGreen;
    } else if(colorString == "yellow") {
      color = Qt::yellow;
    } else if(colorString == "darkyellow") {
      color = Qt::darkYellow;
    } else if(colorString == "blue") {
      color = Qt::blue;
    } else if(colorString == "darkblue") {
      color = Qt::darkBlue;
    } else if(colorString == "gray") {
      color = Qt::gray;
    } else if(colorString == "grey") {
      color = Qt::gray;
    } else if(colorString == "darkgray") {
      color = Qt::darkGray;
    } else if(colorString == "darkgrey") {
      color = Qt::darkGray;
    } else if(colorString == "lightgray") {
      color = Qt::lightGray;
    } else if(colorString == "lightgrey") {
      color = Qt::lightGray;
    } else {
      printf("Color '%s' doesn't exist\n", colorString.toStdString().c_str());
      return;
    }
    painter.begin(image);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(QColor(color)));
    
    parameters.removeFirst(); // Remove drawing color string
    
    if(parameters.count() >= 1) {
      if(parameters.first() == "line") {
        parameters.removeFirst(); // Remove 'line'
        if(parameters.count() >= 4) {
          int x1 = getValue(parameters);
          int y1 = getValue(parameters);
          int x2 = getValue(parameters);
          int y2 = getValue(parameters);
          printf("Drawing %s line from %d,%d to %d,%d\n", colorString.toStdString().c_str(),
                 x1, y1, x2, y2);
          painter.drawLine(x1, y1, x2, y2);
        }        
      } else if(parameters.first() == "pixel") {
        parameters.removeFirst(); // Remove 'pixel'
        if(parameters.count() >= 2) {
          int x = getValue(parameters);
          int y = getValue(parameters);
          printf("Drawing %s pixel at %d,%d\n", colorString.toStdString().c_str(), x, y);
          painter.drawPoint(x, y);
        }
      } else if(parameters.first() == "ellipse") {
        parameters.removeFirst(); // Remove 'ellipse'
        if(parameters.count() >= 4) {
          painter.setBrush(QBrush(color, Qt::SolidPattern));
          int x = getValue(parameters);
          int y = getValue(parameters);
          int w = getValue(parameters);
          int h = getValue(parameters);
          printf("Drawing %s ellipse at %d,%d with a width of %d and a height of %d\n", colorString.toStdString().c_str(), x, y, w, h);
          painter.drawEllipse(x, y, w, h);
        }
      } else if(parameters.first() == "rectangle") {
        parameters.removeFirst(); // Remove 'rectangle'
        if(parameters.count() >= 4) {
          painter.setBrush(QBrush(color, Qt::SolidPattern));
          int x = getValue(parameters);
          int y = getValue(parameters);
          int w = getValue(parameters);
          int h = getValue(parameters);
          printf("Drawing %s rectangle at %d,%d with a width of %d and a height of %d\n", colorString.toStdString().c_str(), x, y, w, h);
          painter.drawRect(x, y, w, h);
        }
      } else if(parameters.first() == "text") {
        parameters.removeFirst(); // Remove 'text'
        if(parameters.count() >= 3) {
          int x = getValue(parameters);
          int y = getValue(parameters);
          QString text = parameters.first();
          parameters.removeFirst(); // Remove string
          printf("Drawing %s text '%s' at %d,%d\n", colorString.toStdString().c_str(),
                 text.toStdString().c_str(), x, y);
          drawText(painter, color, x, y, text);
        }
      } else if(parameters.first() == "value") {
        parameters.removeFirst(); // Remove 'value'
        if(parameters.count() >= 3) {
          int x = getValue(parameters);
          int y = getValue(parameters);
          int value = getValue(parameters);
          printf("Drawing %s value %d at %d,%d\n", colorString.toStdString().c_str(),
                 value, x, y);
          drawText(painter, color, x, y, QString::number(value));
        }
      }
    }
    painter.end();
  }
}

void ScriptHandler::drawText(QPainter &painter, const Qt::GlobalColor &color,
                             const int &x, const int &y, const QString &text)
{
  int idx = x;
  for(const auto &textChar: text) {
    QImage charImage;
    if(pfont.contains(textChar)) {
      charImage = pfont[textChar];
    } else {
      charImage = QImage(5, 4, QImage::Format_ARGB32_Premultiplied);
      charImage.fill(Qt::red);
    }
    charImage.setColor(1, QColor(color).rgb());

    painter.drawImage(idx, y, charImage);
    idx += charImage.width();
  }
}

void ScriptHandler::handleBreak(int &stop)
{
  printf("Changing behaviour\n");
  stop = 2; // Will tell the Boris class to change behaviour
}

void ScriptHandler::handleCall(QList<QString> &parameters, int &stop)
{
  parameters.removeFirst(); // Remove 'call'
  if(behaviours.at(boris->curBehav).defines.contains(parameters.first())) {
    printf("Calling define '%s'\n", parameters.first().toStdString().c_str());
    runScript(stop, behaviours.at(boris->curBehav).defines[parameters.first()]);
  } else {
    printf("Calling define '%s', ERROR: Unknown define\n", parameters.first().toStdString().c_str());
  }
  parameters.removeFirst(); // Remove define name
}

void ScriptHandler::handleSound(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'sound'
  if(parameters.count() >= 1) {
    printf("Playing sound '%s'\n", parameters.first().toStdString().c_str());
    emit boris->playSoundFile(parameters.first(),
                              (float)boris->pos().x() / (float)settings->desktopWidth * 2.0 - 1.0,
                              (stats->getHyper() / 60.0) + 1);
  }
  parameters.removeFirst(); // Remove sound file name
}

int ScriptHandler::getValue(QList<QString> &parameters)
{
  bool isInt = false;
  int result = parameters.first().toInt(&isInt);
  if(!isInt) {
    if(parameters.first().left(1) == "@") {
      result = (qrand() % parameters.first().right(parameters.first().length() - 1).toInt()) + 1;
    } else {
      result = boris->scriptVars[parameters.first()];
    }
  }

  parameters.removeFirst(); // Remove value

  if(parameters.count() >= 2) {
    if(parameters.first() == "+") {
      parameters.removeFirst(); // Remove '+'
      return result + getValue(parameters);
    } else if(parameters.first() == "-") {
      parameters.removeFirst(); // Remove '-'
      return result - getValue(parameters);
    } else if(parameters.first() == "*") {
      parameters.removeFirst(); // Remove '*'
      return result * getValue(parameters);
    } else if(parameters.first() == "/") {
      parameters.removeFirst(); // Remove '/'
      return result / getValue(parameters);
    } else if(parameters.first() == "%") {
      parameters.removeFirst(); // Remove '%'
      return result % getValue(parameters);
    }
  }

  return result;
}
