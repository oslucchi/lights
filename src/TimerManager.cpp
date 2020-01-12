/*
 * TimerManager.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */
#include <TimerManager.h>
#include "Arduino.h"

TimerManager::TimerManager() {
	for(int i = 0; i < HOW_MANY; i++)
	{
		timers[i].timer = nullptr;
		timers[i].allocated = false;
	}
}
void TimerManager::update(unsigned long now)
{
	for(int i =0; i < HOW_MANY; i++)
	{
		if (timers[i].timer != nullptr)
		{
			timers[i].timer->update(now);
		}
	}
}
void TimerManager::dropTimer(Timer* timer)
{
	for(int i =0; i < HOW_MANY; i++)
	{
		if (timers[i].timer == timer)
		{
			timers[i].timer->reset();
			free(timers[i].timer);
			timers[i].allocated = false;
			break;
		}
	}
}

Timer* TimerManager::getNewTimer(const char *n)
{
	for(int i = 0; i < HOW_MANY; i++)
	{
		if (!timers[i].allocated)
		{
			timers[i].allocated = true;
			timers[i].timer = new Timer();
			timers[i].timer->setName(n);
			Serial.println(n);
			return(timers[i].timer);
		}
	}
	return(nullptr);
}

void TimerManager::dump()
{
	for(int i = 0; i < HOW_MANY; i++)
	{
		if (timers[i].allocated)
		{
			Serial.print("Timer ");
			Serial.print(timers[i].timer->getName());
			Serial.print(", duration ");
			Serial.print(timers[i].timer->getDuration());
			Serial.print(", elapsed ");
			Serial.print(timers[i].timer->getElapsed());
			Serial.print(", status run ");
			Serial.print(timers[i].timer->getIsRunning() ? "yes " : "no ");
			Serial.print("exp ");
			Serial.println(timers[i].timer->getIsExpired() ? "yes " : "no ");
		}
	}
}
