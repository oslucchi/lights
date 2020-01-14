/*
 * Command.cpp
 *
 *  Created on: Feb 15, 2019
 *      Author: osvaldo
 */

#include <Command.h>

Command::Command() {
	timer = nullptr;
	pin = nullptr;
	ctrlId = 0;
	actualValue = nullptr;
	upperBound = nullptr;
	lowerBound = nullptr;
	ctrlType = 0;
	timerManager = nullptr;
}

Command::~Command() {
	// TODO Auto-generated destructor stub
}

Timer* Command::getTimer(int idx)
{
	return(timer[idx]);
}

void Command::setTimerManager(TimerManager* tm)
{
	timerManager = tm;
}

void Command::setControlName(const char *name)
{
	memset(this->controlName, '\0', sizeof(this->controlName));
	strncpy(this->controlName, name, 11);
}
