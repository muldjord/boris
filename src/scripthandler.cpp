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
#include "soundmixer.h"
#include "item.h"
#include "sprite.h"
#include "tinyexpr.h"

#include <stdio.h>

#include <QRandomGenerator>
#include <QRegularExpression>

extern SoundMixer soundMixer;

ScriptHandler::ScriptHandler(QImage *image,
                             bool *drawing,
                             Settings &settings,
                             Bubble *bubble,
                             const QMap<QString, int> &labels,
                             const QMap<QString, Script> &defines,
                             QMap<QString, int> &scriptVars,
                             const QPoint parentPos,
                             const int &size)
  : settings(settings), labels(labels), defines(defines), scriptVars(scriptVars), parentPos(parentPos), size(size)
{
  this->image = image;
  this->drawing = drawing;
  this->bubble = bubble;
}

void ScriptHandler::runScript(int &stop, const Script &script)
{
  for(const auto &command: script.commands) {
    if(settings.scriptOutput) {
      printf("CODE: '%s'\n", command.toStdString().c_str());
    }
#if QT_VERSION >= 0x050e00
    QList<QString> parameters = command.split(" ", Qt::KeepEmptyParts);
#else
    QList<QString> parameters = command.split(" ", QString::KeepEmptyParts);
#endif
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
  } else if(parameters.first() == "spawn") {
    handleSpawn(parameters);
  } else if(parameters.first() == "draw") {
    handleDraw(parameters);
  } else if(parameters.first() == "break") {
    handleBreak(stop);
  } else if(parameters.first() == "stop") {
    handleStop(stop);
  } else if(parameters.first() == "behav") {
    handleBehav(parameters, stop);
  } else if(parameters.first() == "call") {
    handleCall(parameters, stop);
  } else if(parameters.first() == "sound") {
    handleSound(parameters);
  } else if(parameters.first() == "say") {
    handleSay(parameters);
  } else if(parameters.first() == "think") {
    handleThink(parameters);
  } else if(parameters.first() == "throw") {
    handleThrow(parameters);
  }
  // IMPORTANT!!! When adding new commands, REMEMBER TO ALSO ADD THEM TO 'getValue()'!!!
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
      if(settings.scriptOutput) {
        printf("\n");
      }
      runScript(stop, script.blocks[parameters.first()]);
    } else {
      runCommand(parameters, stop, script);
    }
  } else if(parameters.contains("else") &&
            parameters.first() != "then") {
    while(parameters.takeFirst() != "else") {
    }
    if(parameters.first().left(2) == "##") {
      if(settings.scriptOutput) {
        printf("\n");
      }
      runScript(stop, script.blocks[parameters.first()]);
    } else {
      runCommand(parameters, stop, script);
    }
  } else {
    if(settings.scriptOutput) {
      printf("Condition not met\n");
    }
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

  if(settings.scriptOutput) {
    printf("%d %s %d", compareFrom, op.toStdString().c_str(), compareTo);
  }

  if(parameters.count() >= 1) {
    if(parameters.first() == "or") {
      if(settings.scriptOutput) {
        printf(" or ");
      }
      parameters.removeFirst();
      if(isTrue) {
        compare = false;
      }
      condition(parameters, isTrue, compare);
      return;
    } else if(parameters.first() == "and") {
      if(settings.scriptOutput) {
        printf(" and ");
      }
      parameters.removeFirst();
      if(!isTrue) {
        compare = false;
      }
      condition(parameters, isTrue, compare);
      return;
    } else {
      if(settings.scriptOutput) {
        printf(", ");
      }
    }
  }
}

void ScriptHandler::handleGoto(QList<QString> &parameters, int &stop)
{
  parameters.removeFirst(); // Remove 'goto'

  if(labels.contains(parameters.first())) {
    if(settings.scriptOutput) {
      printf("Going to label '%s' at frame %d\n", parameters.first().toStdString().c_str(), labels[parameters.first()]);
    }
    emit setCurFrame(labels[parameters.first()]);
  } else {
    if(settings.scriptOutput) {
      printf("Going to '%s', ERROR: Unknown label\n", parameters.first().toStdString().c_str());
    }
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
    scriptVars[variable] = number;
  } else if(op == "+=") {
    scriptVars[variable] += number;
  } else if(op == "-=") {
    scriptVars[variable] -= number;
  } else if(op == "*=") {
    scriptVars[variable] *= number;
  } else if(op == "/=") {
    scriptVars[variable] /= number;
  }

  if(settings.scriptOutput) {
    printf("%s = %d\n", variable.toStdString().c_str(), scriptVars[variable]);
  }
}

void ScriptHandler::handleStat(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'stat'

  QString statType = parameters.takeFirst();
  QString op = parameters.takeFirst();
  int number = getValue(parameters);

  if(op == "-=") {
    number *= -1;
  }
  if(statType == "hyper" || statType == "health" || statType == "energy" || statType == "hunger" || statType == "toilet" || statType == "social" || statType == "fun" || statType == "hygiene") {
    if(settings.scriptOutput) {
      printf("%s %s %d\n", statType.toStdString().c_str(),
             op.toStdString().c_str(), number);
    }
    emit statChange(statType, number);
  } else {
    if(settings.scriptOutput) {
      printf("%s, ERROR: Unknown stat\n", statType.toStdString().c_str());
    }
  }
}

void ScriptHandler::handlePrint(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'print'
  if(settings.scriptOutput) {
    printf("%d\n", getValue(parameters));
  }
}

void ScriptHandler::handleSpawn(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'spawn'
  QString itemName = parameters.takeFirst();
  int iX = getValue(parameters);
  int iY = getValue(parameters);
  if(settings.items) {
    if(settings.scriptOutput) {
      printf("Spawning item '%s' at %d,%d\n", itemName.toStdString().c_str(), iX, iY);
    }
    settings.itemList.append(new Item(parentPos.x() + (iX * (size / 32)),
                                      parentPos.y() + (size / 2) + (iY * (size / 32)),
                                      size, itemName, settings));
  } else {
    if(settings.scriptOutput) {
      printf("Items disabled, ignoring spawn\n");
    }
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
      //image->fill(Qt::transparent);
      *drawing = false;
      return;
    }
    QPainter painter;
    painter.begin(image);
    painter.setRenderHint(QPainter::Antialiasing, false);
    if(parameters.first() == "sprite") {
      parameters.removeFirst(); // Remove 'sprite'
      if(parameters.count() >= 4) {
        QString sprite = parameters.first();
        parameters.removeFirst(); // Remove sprite name
        int f = getValue(parameters); // Sprite frame
        int x = getValue(parameters);
        int y = getValue(parameters);
        if(settings.scriptOutput) {
          printf("Drawing frame %d from sprite '%s' at %d,%d\n", f, sprite.toStdString().c_str(), x, y);
        }
        painter.drawImage(x, y, settings.sprites[sprite].at(f));
      }        
      return;
    }
    QString colorString = parameters.first();
    Qt::GlobalColor color = Qt::transparent;
    if(colorString == "black") {
      painter.setPen(QPen(QColor(Qt::black)));
    } else if(colorString == "white") {
      painter.setPen(QPen(QColor(Qt::white)));
    } else if(colorString == "cyan") {
      painter.setPen(QPen(QColor(Qt::cyan)));
    } else if(colorString == "darkcyan") {
      painter.setPen(QPen(QColor(Qt::darkCyan)));
    } else if(colorString == "red") {
      painter.setPen(QPen(QColor(Qt::red)));
    } else if(colorString == "darkred") {
      painter.setPen(QPen(QColor(Qt::darkRed)));
    } else if(colorString == "magenta") {
      painter.setPen(QPen(QColor(Qt::magenta)));
    } else if(colorString == "darkmagenta") {
      painter.setPen(QPen(QColor(Qt::darkMagenta)));
    } else if(colorString == "green") {
      painter.setPen(QPen(QColor(Qt::green)));
    } else if(colorString == "darkgreen") {
      painter.setPen(QPen(QColor(Qt::darkGreen)));
    } else if(colorString == "yellow") {
      painter.setPen(QPen(QColor(Qt::yellow)));
    } else if(colorString == "darkyellow") {
      painter.setPen(QPen(QColor(Qt::darkYellow)));
    } else if(colorString == "blue") {
      painter.setPen(QPen(QColor(Qt::blue)));
    } else if(colorString == "darkblue") {
      painter.setPen(QPen(QColor(Qt::darkBlue)));
    } else if(colorString == "gray") {
      painter.setPen(QPen(QColor(Qt::gray)));
    } else if(colorString == "grey") {
      painter.setPen(QPen(QColor(Qt::gray)));
    } else if(colorString == "darkgray") {
      painter.setPen(QPen(QColor(Qt::darkGray)));
    } else if(colorString == "darkgrey") {
      painter.setPen(QPen(QColor(Qt::darkGray)));
    } else if(colorString == "lightgray") {
      painter.setPen(QPen(QColor(Qt::lightGray)));
    } else if(colorString == "lightgrey") {
      painter.setPen(QPen(QColor(Qt::lightGray)));
    } else if(colorString.length() == 7 && colorString.left(1) == "#") {
      int redVal = colorString.mid(1, 2).toInt(nullptr, 16);
      int blueVal = colorString.mid(3, 2).toInt(nullptr, 16);
      int greenVal = colorString.mid(5, 2).toInt(nullptr, 16);
      painter.setPen(QPen(QColor(redVal, greenVal, blueVal)));
    } else {
      if(settings.scriptOutput) {
        printf("Color '%s' is malformed, falling back to black\n",
               colorString.toStdString().c_str());
      }
      painter.setPen(QPen(QColor(Qt::black)));
    }
    
    parameters.removeFirst(); // Remove drawing color string
    
    if(parameters.count() >= 1) {
      if(parameters.first() == "line") {
        parameters.removeFirst(); // Remove 'line'
        if(parameters.count() >= 4) {
          int x1 = getValue(parameters);
          int y1 = getValue(parameters);
          int x2 = getValue(parameters);
          int y2 = getValue(parameters);
          if(settings.scriptOutput) {
            printf("Drawing %s line from %d,%d to %d,%d\n", colorString.toStdString().c_str(),
                   x1, y1, x2, y2);
          }
          painter.drawLine(x1, y1, x2, y2);
        }        
      } else if(parameters.first() == "pixel") {
        parameters.removeFirst(); // Remove 'pixel'
        if(parameters.count() >= 2) {
          int x = getValue(parameters);
          int y = getValue(parameters);
          if(settings.scriptOutput) {
            printf("Drawing %s pixel at %d,%d\n", colorString.toStdString().c_str(), x, y);
          }
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
          if(settings.scriptOutput) {
            printf("Drawing %s ellipse at %d,%d with a width of %d and a height of %d\n", colorString.toStdString().c_str(), x, y, w, h);
          }
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
          if(settings.scriptOutput) {
            printf("Drawing %s rectangle at %d,%d with a width of %d and a height of %d\n", colorString.toStdString().c_str(), x, y, w, h);
          }
          painter.drawRect(x, y, w, h);
        }
      } else if(parameters.first() == "text") {
        parameters.removeFirst(); // Remove 'text'
        if(parameters.count() >= 3) {
          int x = getValue(parameters);
          int y = getValue(parameters);
          QString text = parameters.first();
          parameters.removeFirst(); // Remove string
          if(settings.scriptOutput) {
            printf("Drawing %s text '%s' at %d,%d\n", colorString.toStdString().c_str(),
                   text.toStdString().c_str(), x, y);
          }
          drawText(painter, x, y, text);
        }
      } else if(parameters.first() == "value") {
        parameters.removeFirst(); // Remove 'value'
        if(parameters.count() >= 3) {
          int x = getValue(parameters);
          int y = getValue(parameters);
          int value = getValue(parameters);
          if(settings.scriptOutput) {
            printf("Drawing %s value %d at %d,%d\n", colorString.toStdString().c_str(),
                   value, x, y);
          }
          drawText(painter, x, y, QString::number(value));
        }
      }
    }
    painter.end();
  }
}

void ScriptHandler::drawText(QPainter &painter, const int &x, const int &y, const QString &text)
{
  int idx = x;
  for(const auto &textChar: text) {
    QImage charImage;
    if(settings.pixelFont.contains(textChar)) {
      charImage = settings.pixelFont[textChar];
    } else {
      charImage = QImage(5, 4, QImage::Format_ARGB32_Premultiplied);
      charImage.fill(Qt::red);
    }
    charImage.setColor(1, painter.pen().color().rgb());

    painter.drawImage(idx, y, charImage);
    idx += charImage.width();
  }
}

void ScriptHandler::handleBreak(int &stop)
{
  if(settings.scriptOutput) {
    printf("Changing behaviour\n");
  }
  stop = 2; // Will tell the Boris class to change behaviour
}

void ScriptHandler::handleStop(int &stop)
{
  if(settings.scriptOutput) {
    printf("Stopping frame progression\n");
  }
  stop = 3; // Will stop the animation and behaviour timers
}

void ScriptHandler::handleCall(QList<QString> &parameters, int &stop)
{
  parameters.removeFirst(); // Remove 'call'
  if(defines.contains(parameters.first())) {
    if(settings.scriptOutput) {
      printf("Calling define '%s'\n", parameters.first().toStdString().c_str());
    }
    runScript(stop, defines[parameters.first()]);
  } else {
    if(settings.scriptOutput) {
      printf("Calling define '%s', ERROR: Unknown define\n", parameters.first().toStdString().c_str());
    }
  }
  parameters.removeFirst(); // Remove define name
}

void ScriptHandler::handleBehav(QList<QString> &parameters, int &stop)
{
  parameters.removeFirst(); // Remove 'behav'
  emit behavFromFile(parameters.first());
  if(settings.scriptOutput) {
    printf("Changing behaviour to '%s'\n", parameters.first().toStdString().c_str());
  }
  stop = 1; // Will tell the Boris class to exit the script processing
  parameters.removeFirst(); // Remove behaviour filename
}

void ScriptHandler::handleSound(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'sound'
  if(parameters.count() >= 1) {
    if(settings.scriptOutput) {
      printf("Playing sound '%s'\n", parameters.first().toStdString().c_str());
    }
    soundMixer.playSoundFile(parameters.first(),
                             (float)parentPos.x() / (float)settings.desktopWidth * 2.0 - 1.0,
                             (scriptVars["hyper"] / 60.0) + 1);
  }
  parameters.removeFirst(); // Remove sound file name
}

void ScriptHandler::handleSay(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'say'
  if(parameters.count() >= 1) {
    QString bubbleText = "";
    if(parameters.first() == "rss") {
      parameters.removeFirst(); // Remove 'rss'
      if(!settings.rssLines.isEmpty()) {
        int rssLine = QRandomGenerator::global()->bounded(settings.rssLines.count());
        bubbleText = settings.rssLines.at(rssLine).text;
        bubble->initBubble(parentPos.x(), parentPos.y(), size, scriptVars["hyper"],
                           bubbleText,
                           "_chat",
                           settings.rssLines.at(rssLine).url);
      }
    } else {
      int quotes = 0;
      while(quotes < 2) {
        QString word = parameters.first();
        parameters.removeFirst();
        if(word.count('\"') > 0) {
          quotes += word.count('\"');
          word.replace('\"', "");
        }
        bubbleText.append(word + " ");
      }
      bubbleText = bubbleText.trimmed();
      for(const auto &variable: scriptVars.keys()) {
        bubbleText.replace("$" + variable, QString::number(scriptVars[variable]));
      }
      bubble->initBubble(parentPos.x(), parentPos.y(), size, scriptVars["hyper"], bubbleText);
    }
    if(settings.scriptOutput) {
      printf("Saying '%s'\n", bubbleText.toStdString().c_str());
    }
  }
}

void ScriptHandler::handleThink(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'think'
  if(parameters.count() >= 1) {
    QString bubbleText = "";
    if(parameters.first() == "rss") {
      parameters.removeFirst(); // Remove 'rss'
      if(!settings.rssLines.isEmpty()) {
        int rssLine = QRandomGenerator::global()->bounded(settings.rssLines.count());
        bubbleText = settings.rssLines.at(rssLine).text;
        bubble->initBubble(parentPos.x(), parentPos.y(), size, scriptVars["hyper"],
                           bubbleText,
                           "_thought",
                           settings.rssLines.at(rssLine).url);
      }
    } else {
      int quotes = 0;
      while(quotes < 2) {
        QString word = parameters.first();
        parameters.removeFirst();
        if(word.count('\"') > 0) {
          quotes += word.count('\"');
          word.replace('\"', "");
        }
        bubbleText.append(word + " ");
      }
      bubbleText = bubbleText.trimmed();
      for(const auto &variable: scriptVars.keys()) {
        bubbleText.replace("$" + variable, QString::number(scriptVars[variable]));
      }
      bubble->initBubble(parentPos.x(), parentPos.y(), size, scriptVars["hyper"], bubbleText, "_thought");
    }
    if(settings.scriptOutput) {
      printf("Thinking '%s'\n", bubbleText.toStdString().c_str());
    }
  }
}

void ScriptHandler::handleThrow(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'throw'
  scriptVars["xvel"] = getValue(parameters);
  scriptVars["yvel"] = getValue(parameters) * -1; // Reverse it
}

int ScriptHandler::getValue(QList<QString> &parameters)
{
  QList<QString> code;
  while(!parameters.isEmpty()) {
    QString startPars = "";
    QString endPars = "";
    while(parameters.first().left(1) == "(") {
      startPars.append("(");
      parameters.first().remove(0, 1);
    }
    while(parameters.first().right(1) == ")") {
      endPars.append(")");
      parameters.first().remove(parameters.first().length() - 1, 1);
    }
    bool isInt = false;
    parameters.first().toInt(&isInt);
    if(isInt ||
       parameters.first().left(1) == "@" ||
       scriptVars.contains(parameters.first())) {
      if(parameters.first().left(1) == "@") {
        parameters.first() = QString::number(QRandomGenerator::global()->bounded(parameters.first().mid(1).toInt()) + 1);
      } else if(scriptVars.contains(parameters.first())) {
        parameters.first() = QString::number(scriptVars[parameters.first()]);
      }
      code.append(startPars + parameters.first() + endPars);
      parameters.removeFirst();
      if(parameters.count() >= 1) {
        if(parameters.first() != "+" &&
           parameters.first() != "-" &&
           parameters.first() != "*" &&
           parameters.first() != "/" &&
           parameters.first() != "(" &&
           parameters.first() != ")" &&
           parameters.first() != "%") {
          break;
        }
      }
    } else {
      code.append(startPars + parameters.first() + endPars);
      parameters.removeFirst();
    }
  }

  QString codeConcat = "";
  for(const auto &token: code) {
    codeConcat.append(token);
  }
  return te_interp(codeConcat.toStdString().c_str(), 0);
}
