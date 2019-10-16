/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            scripthandler.cpp
 *
 *  Tue Nov 26 16:56:00 CEST 2013
 *  Copyright 2013 Lars Muldjord
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

ScriptHandler::ScriptHandler(Boris *boris)
{
  this->boris = boris;
}

bool ScriptHandler::runScript(QList<QString> script)
{
  for(const auto &instruction: script) {
    printf("CODE: '%s'\n", instruction.toStdString().c_str());
    QList<QString> parameters = instruction.split(" ", QString::KeepEmptyParts);
    if(parameters.at(0) == "var") {
      handleVar(parameters.mid(0));
    } else if(parameters.at(0) == "stat") {
      handleStat(parameters.mid(0));
    } else if(parameters.at(0) == "if") {
      if(handleIf(parameters.mid(0))) {
        return true;
      }
    } else if(parameters.at(0) == "goto") {
      if(handleGoto(parameters.mid(0))) {
        return true;
      }
    } else if(parameters.at(0) == "print") {
      handlePrint(parameters.mid(0));
    }
  }
  return false;
}

bool ScriptHandler::handleIf(QList<QString> parameters)
{
  int idx = 1;
  if(handleCondition(parameters.mid(idx), idx)) {
    if(parameters.at(idx) == "goto") {
      if(handleGoto(parameters.mid(idx))) {
        return true;
      }
    } else if(parameters.at(idx) == "break") {
      handleBreak();
      return true;
    }
  } else {
    if(parameters.count() >= idx + 2 && parameters.at(idx + 1) == "else") {
      if(parameters.at(idx + 2) == "goto") {
        if(handleGoto(parameters.mid(idx + 2))) {
          return true;
        }
      } else if(parameters.at(idx + 2) == "if") {
        if(handleIf(parameters.mid(idx + 2))) {
          return true;
        }
      }
    } else {
      printf("Condition(s) not met\n");
    }
  }
  return false;
}

bool ScriptHandler::handleGoto(QList<QString> parameters)
{
  if(behaviours.at(boris->curBehav).labels.contains(parameters.at(1))) {
    printf("Going to label '%s' at frame %d\n", parameters.at(1).toStdString().c_str(), behaviours.at(boris->curBehav).labels[parameters.at(1)]);
    boris->curFrame = behaviours.at(boris->curBehav).labels[parameters.at(1)];
    return true;
  } else {
    printf("Going to '%s', ERROR: Unknown label\n", parameters.at(1).toStdString().c_str());
  }
  return false;
}

void ScriptHandler::handleVar(QList<QString> parameters)
{
  bool isInt = false;
  int number = parameters.at(3).toInt(&isInt);;
  if(!isInt) {
    if(parameters.at(3).left(1) == "@") {
      number = (qrand() % parameters.at(3).right(parameters.at(3).length() - 1).toInt()) + 1;
    } else {
      number = boris->scriptVars[parameters.at(3)];
    }
  }
  if(parameters.at(2) == "=") {
    boris->scriptVars[parameters.at(1)] = number;
  } else if(parameters.at(2) == "+=") {
    boris->scriptVars[parameters.at(1)] += number;
  } else if(parameters.at(2) == "-=") {
    boris->scriptVars[parameters.at(1)] -= number;
  }
  printf("%s = %d\n", parameters.at(1).toStdString().c_str(), boris->scriptVars[parameters.at(1)]);
}

void ScriptHandler::handleStat(QList<QString> parameters)
{
  bool isInt = false;
  int number = parameters.at(3).toInt(&isInt);;
  if(!isInt) {
    if(parameters.at(3).left(1) == "@") {
      number = (qrand() % parameters.at(3).right(parameters.at(3).length() - 1).toInt()) + 1;
    } else {
      number = boris->scriptVars[parameters.at(3)];
    }
  }
  int *stat = nullptr;
  if(parameters.at(1) == "hyper") {
    stat = &boris->hyperQueue;
  } else if(parameters.at(1) == "health") {
    stat = &boris->healthQueue;
  } else if(parameters.at(1) == "energy") {
    stat = &boris->energyQueue;
  } else if(parameters.at(1) == "hunger") {
    stat = &boris->hungerQueue;
  } else if(parameters.at(1) == "bladder") {
    stat = &boris->bladderQueue;
  } else if(parameters.at(1) == "social") {
    stat = &boris->socialQueue;
  } else if(parameters.at(1) == "fun") {
    stat = &boris->funQueue;
  } else if(parameters.at(1) == "hygiene") {
    stat = &boris->hygieneQueue;
  }
  if(stat != nullptr) {
    if(parameters.at(2) == "+=") {
      *stat += number;
    } else if(parameters.at(2) == "-=") {
      *stat -= number;
    }
    printf("%s %s %s\n", parameters.at(1).toStdString().c_str(),
           parameters.at(2).toStdString().c_str(),
           parameters.at(3).toStdString().c_str());
  } else {
    printf("%s, ERROR: Unknown stat\n", parameters.at(1).toStdString().c_str());
  }
}

void ScriptHandler::handleBreak()
{
  printf("Changing behaviour\n");
  boris->behavTimer.stop();
  boris->nextBehaviour();
}

void ScriptHandler::handlePrint(QList<QString> parameters)
{
  if(boris->scriptVars.contains(parameters.at(1))) {
    printf("%s = %d\n", parameters.at(1).toStdString().c_str(), boris->scriptVars[parameters.at(1)]);
  } else {
    printf("%s, ERROR: Unknown variable\n", parameters.at(1).toStdString().c_str());
  }
}

bool ScriptHandler::handleCondition(QList<QString> parameters, int &idx)
{
  idx += 3; // Move index past this condition
  bool isInt = false;
  int compareFrom = parameters.at(0).toInt(&isInt);
  if(!isInt) {
    if(parameters.at(0).left(1) == "@") {
      compareFrom = (qrand() % parameters.at(0).right(parameters.at(0).length() - 1).toInt()) + 1;
    } else {
      compareFrom = boris->scriptVars[parameters.at(0)];
    }
  }
  isInt = false;
  int compareTo = parameters.at(2).toInt(&isInt);
  if(!isInt) {
    if(parameters.at(2).left(1) == "@") {
      compareTo = (qrand() % parameters.at(2).right(parameters.at(2).length() - 1).toInt()) + 1;
    } else {
      compareTo = boris->scriptVars[parameters.at(2)];
    }
  }
  printf("%d %s %d ", compareFrom, parameters.at(1).toStdString().c_str(), compareTo);

  bool cond = false;
  if(parameters.at(1) == "<") {
    if(compareFrom < compareTo) {
      cond = true;
    }
  } else if(parameters.at(1) == ">") {
    if(compareFrom > compareTo) {
      cond = true;
    }
  } else if(parameters.at(1) == "<=") {
    if(compareFrom <= compareTo) {
      cond = true;
    }
  } else if(parameters.at(1) == ">=") {
    if(compareFrom >= compareTo) {
      cond = true;
    }
  } else if(parameters.at(1) == "=") {
    if(compareFrom == compareTo) {
      cond = true;
    }
  } else if(parameters.at(1) == "==") {
    if(compareFrom == compareTo) {
      cond = true;
    }
  }
  if(parameters.count() >= 5) {
    if(parameters.at(3) == "or") {
      printf("or ");
      idx++;
      if(handleCondition(parameters.mid(4), idx)) {
        cond = true;
      } else {
        cond = false;
      }
    } else if(parameters.at(3) == "and" && cond) {
      printf("and ");
      idx++;
      if(!handleCondition(parameters.mid(4), idx)) {
        cond = false;
      }
    }
  }
  return cond;
}
