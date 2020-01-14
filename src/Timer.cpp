/*
 * Timer.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */
#include "Arduino.h"
#include "Timer.h"

Timer::Timer() {
	duration = 0;
	startTime = 0;
	elapsed = 0;
	isRunning = false;
	isExpired = false;
	strcpy(name, "");
	isFront = false;
}

void Timer::setName(const char* n)
{
	strncpy(name, n, sizeof(name) - 1);
}

void Timer::setDuration(unsigned long d)
{
	duration = d;
}

unsigned long Timer::getTimeToExpiry()
{
	return((int)(duration - elapsed));
}

unsigned long Timer::getTimeElapsed()
{
	return(elapsed);
}

void Timer::start(unsigned long mills)
{
	startTime = mills;
	isRunning = true;
	isExpired = false;
}

void Timer::stop()
{
	isRunning = false;
}

void Timer::reset()
{
	isRunning = false;
	isExpired = false;
	isFront = false;
	startTime = 0;
	elapsed = 0;
}

void Timer::resume()
{
	isRunning = true;
	isFront = false;
	startTime = millis();
}

void Timer::update(unsigned long  mills)
{
	if (isRunning)
	{
		elapsed = mills - startTime;
	}
	if ((elapsed >= duration) && isRunning)
	{
		isRunning = false;
		isExpired = true;
		isFront = true;
	}
}

unsigned long	Timer::getDuration(){
	return(duration);
}

char* Timer::getName()
{
	return(name);
}

bool Timer::getIsRunning()
{
	return(isRunning);
}

bool Timer::getIsExpired()
{
	return(isExpired);
}

unsigned long Timer::getElapsed()
{
	return(elapsed);
}

void Timer::restart(unsigned long mills)
{
	reset();
	start(mills);
}

bool Timer::getIsFront()
{
	if (isFront)
	{
		isFront = false;
		return true;
	}
	return false;
}

void Timer::setDuration(unsigned char* t)
{
	unsigned long unitsMultilpier, units;
	float decimalMultiplier, decimals;

	switch((*t & 0b11000000)>>6)
	{
	case 0x00: // seconds
		unitsMultilpier = 1000;
		decimalMultiplier = 10;
		break;
	case 0x01: // minutes
		unitsMultilpier = (unsigned long) 60 * 1000;
		decimalMultiplier = 1000;
		break;
	case 0x02: // hours
		unitsMultilpier = (unsigned long) 60 * 60 * 1000;
		decimalMultiplier = (unsigned long) 60 * 1000;
		break;
	case 0x03: // days
		unitsMultilpier = (unsigned long) 24 * 60 * 60 * 1000;
		decimalMultiplier = (unsigned long) 60 * 60 * 1000;
		break;
	}
	units = (unsigned long)(((*t & 0b00111111)<<1)+((*(t+1) & 0b10000000)>>7));
	decimals = (unsigned long)(*(t+1) & 0b01111111);

	duration = units * unitsMultilpier + decimals * decimalMultiplier;
}
