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

ScriptHandler::ScriptHandler(Boris *boris)
{
  this->boris = boris;
}

void ScriptHandler::runScript(const QList<QString> &script, int &stop)
{
  for(const auto &instruction: script) {
    printf("CODE: '%s'\n", instruction.toStdString().c_str());
    QList<QString> parameters = instruction.split(" ", QString::KeepEmptyParts);
    runCommand(parameters, stop);
    if(stop) {
      break;
    }
  }
}

void ScriptHandler::runCommand(QList<QString> &parameters, int &stop)
{
  if(stop) {
    return;
  }
  if(parameters.first() == "var") {
    handleVar(parameters);
  } else if(parameters.first() == "stat") {
    handleStat(parameters);
  } else if(parameters.first() == "if") {
    handleIf(parameters, stop);
  } else if(parameters.first() == "goto") {
    handleGoto(parameters, stop);
  } else if(parameters.first() == "print") {
    handlePrint(parameters);
  } else if(parameters.first() == "break") {
    handleBreak(stop);
  }
}

void ScriptHandler::handleIf(QList<QString> &parameters, int &stop)
{
  parameters.removeFirst(); // Remove 'if'

  bool isTrue = false;
  bool compare = true;
  handleCondition(parameters, isTrue, compare);
  if(isTrue) {
    if(parameters.first() == "then") {
      parameters.removeFirst();
    }
    runCommand(parameters, stop);
  } else if(parameters.contains("else") &&
            parameters.first() != "then") {
    while(parameters.takeFirst() != "else") {
    }
    runCommand(parameters, stop);
  } else {
    printf("Condition not met\n");
  }
}

void ScriptHandler::handleCondition(QList<QString> &parameters, bool &isTrue, bool &compare)
{
  bool isInt = false;
  int compareFrom = parameters.first().toInt(&isInt);
  if(!isInt) {
    if(parameters.first().left(1) == "@") {
      compareFrom = (qrand() % parameters.first().right(parameters.first().length() - 1).toInt()) + 1;
    } else {
      compareFrom = boris->scriptVars[parameters.first()];
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
  
  if(compare) {
    isTrue = false;
    if(parameters.at(1) == "<") {
      if(compareFrom < compareTo) {
        isTrue = true;
      }
    } else if(parameters.at(1) == ">") {
      if(compareFrom > compareTo) {
        isTrue = true;
      }
    } else if(parameters.at(1) == "<=") {
      if(compareFrom <= compareTo) {
        isTrue = true;
      }
    } else if(parameters.at(1) == ">=") {
      if(compareFrom >= compareTo) {
        isTrue = true;
      }
    } else if(parameters.at(1) == "=") {
      if(compareFrom == compareTo) {
        isTrue = true;
      }
    } else if(parameters.at(1) == "==") {
      if(compareFrom == compareTo) {
        isTrue = true;
      }
    }
  }

  printf("%d %s %d", compareFrom, parameters.at(1).toStdString().c_str(), compareTo);

  parameters.removeFirst();
  parameters.removeFirst();
  parameters.removeFirst();

  if(parameters.count() >= 1) {
    if(parameters.first() == "or") {
      printf(" or ");
      parameters.removeFirst();
      if(isTrue) {
        compare = false;
      }
      handleCondition(parameters, isTrue, compare);
      return;
    } else if(parameters.first() == "and") {
      printf(" and ");
      parameters.removeFirst();
      if(!isTrue) {
        compare = false;
      }
      handleCondition(parameters, isTrue, compare);
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

  bool isInt = false;
  int number = parameters.at(2).toInt(&isInt);;
  if(!isInt) {
    if(parameters.at(2).left(1) == "@") {
      number = (qrand() % parameters.at(2).right(parameters.at(2).length() - 1).toInt()) + 1;
    } else {
      number = boris->scriptVars[parameters.at(2)];
    }
  }
  if(parameters.at(1) == "=") {
    boris->scriptVars[parameters.first()] = number;
  } else if(parameters.at(1) == "+=") {
    boris->scriptVars[parameters.first()] += number;
  } else if(parameters.at(1) == "-=") {
    boris->scriptVars[parameters.first()] -= number;
  } else if(parameters.at(1) == "*=") {
    boris->scriptVars[parameters.first()] *= number;
  } else if(parameters.at(1) == "/=") {
    boris->scriptVars[parameters.first()] /= number;
  }

  printf("%s = %d\n", parameters.first().toStdString().c_str(), boris->scriptVars[parameters.first()]);
  parameters.removeFirst();
  parameters.removeFirst();
  parameters.removeFirst();
}

void ScriptHandler::handleStat(QList<QString> &parameters)
{
  parameters.removeFirst(); // Remove 'stat'

  bool isInt = false;
  int number = parameters.at(2).toInt(&isInt);;
  if(!isInt) {
    if(parameters.at(2).left(1) == "@") {
      number = (qrand() % parameters.at(2).right(parameters.at(2).length() - 1).toInt()) + 1;
    } else {
      number = boris->scriptVars[parameters.at(2)];
    }
  }
  int *stat = nullptr;
  if(parameters.first() == "hyper") {
    stat = &boris->hyperQueue;
  } else if(parameters.first() == "health") {
    stat = &boris->healthQueue;
  } else if(parameters.first() == "energy") {
    stat = &boris->energyQueue;
  } else if(parameters.first() == "hunger") {
    stat = &boris->hungerQueue;
  } else if(parameters.first() == "bladder") {
    stat = &boris->bladderQueue;
  } else if(parameters.first() == "social") {
    stat = &boris->socialQueue;
  } else if(parameters.first() == "fun") {
    stat = &boris->funQueue;
  } else if(parameters.first() == "hygiene") {
    stat = &boris->hygieneQueue;
  }
  if(stat != nullptr) {
    if(parameters.at(1) == "+=") {
      *stat += number;
    } else if(parameters.at(1) == "-=") {
      *stat -= number;
    }
    printf("%s %s %s\n", parameters.first().toStdString().c_str(),
           parameters.at(1).toStdString().c_str(),
           parameters.at(2).toStdString().c_str());
  } else {
    printf("%s, ERROR: Unknown stat\n", parameters.first().toStdString().c_str());
  }

  parameters.removeFirst();
  parameters.removeFirst();
  parameters.removeFirst();
}

void ScriptHandler::handleBreak(int &stop)
{
  printf("Changing behaviour\n");
  stop = 2; // Will tell the Boris class to change behaviour
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
