/*
 * ArduinoControl.cpp
 *
 *  Created on: Feb 16, 2019
 *      Author: osvaldo
 */

#include "Arduino.h"
#include <steamBath.h>
#include <debug.h>
#include <ArduinoControl.h>
#include <LedRGB.h>

ArduinoControl::ArduinoControl() {
	uint8_t pin[3], upperBound[3], lowerBound[3], actualValue[3];

	initialized = false;
	Serial.println("ArduinoControl: registering controls");

	pin[RED] = 3;
	pin[GREEN] = 5;
	pin[BLUE] = 6;
	upperBound[RED] = 255;
	upperBound[GREEN] = 255;
	upperBound[BLUE] = 255;
	lowerBound[RED] = 0;
	lowerBound[GREEN] = 0;
	lowerBound[BLUE] = 0;
	actualValue[RED] = 0;
	actualValue[GREEN] = 0;
	actualValue[BLUE] = 0;
	lights.addCommand(CTRLID_RGBSTRIPE, (uint8_t) TYPE_LED_RGB, pin,
								  upperBound, lowerBound, actualValue, &timerManager, "CabinLights");

	pin[RED] = 9;
	pin[GREEN] = 10;
	pin[BLUE] = 11;
	upperBound[RED] = 255;
	upperBound[GREEN] = 255;
	upperBound[BLUE] = 255;
	lowerBound[RED] = 0;
	lowerBound[GREEN] = 0;
	lowerBound[BLUE] = 0;
	actualValue[RED] = 0;
	actualValue[GREEN] = 0;
	actualValue[BLUE] = 0;
	lights.addCommand(CTRLID_STARRYSKY, (uint8_t) TYPE_LED_RGB, pin,
								  upperBound, lowerBound, actualValue, &timerManager, "StarrySky");
}

ArduinoControl::~ArduinoControl() {
	// TODO Auto-generated destructor stub
}

char* ArduinoControl::toHex(uint8_t a)
{
	static char hex[6];
	hex[0] = '0';
	hex[1] = 'x';
	hex[2] = (char) (a / 16);
	if (hex[2] >= 10)
		hex[2] += 55;
	else
		hex[2] += 48;
	hex[3] = (char) (a % 16);
	if (hex[3] >= 10)
		hex[3] += 55;
	else
		hex[3] += 48;
	hex[4] = ' ';
	hex[5] = '\0';
	return(hex);
}

void ArduinoControl::setup(unsigned long now)
{
	cmdList *current = lights.getController();
	while((current = current->next) != nullptr)
	{
		current->cmd->setup(now);
	}
}

void ArduinoControl::loop(unsigned long now)
{
	timerManager.update(now);
	if (writeIdx != readIdx)
	{
		handleI2CCommand();
	}
	cmdList *current = lights.getController();
	while((current = current->next) != nullptr)
	{
		current->cmd->loop(now);
	}
}

Command* ArduinoControl::getCommand(unsigned char ctrlId)
{
	cmdList *current = lights.getController();
	while((current = current->next) != nullptr)
	{
		if (current->cmd->ctrlId == ctrlId)
			return(current->cmd);
	}
	return(nullptr);
}

int ArduinoControl::handleI2CCommand()
{
//	DebugARDCtrl(( 1, "Command in. idxs r %d - w %d", readIdx, writeIdx ));

	uint8_t cmdBuf[32];
	uint8_t cmdLen = readBuf[readIdx++];

	memset(msgBuf, '\0', sizeof(msgBuf));
	for(int i = 1; i < cmdLen; i++)
	{
		if (readIdx == LOCAL_BUF_SIZE - 1) readIdx = 0;
		cmdBuf[i - 1] = readBuf[readIdx++];
//		strcat(msgBuf, toHex(cmdBuf[i - 1]));
//		strcat(msgBuf, " ");
	}
//	DebugARDCtrl(( 1, "buffer '%s'", msgBuf ));

	Command* ctrl = getCommand(cmdBuf[0]);

	if (ctrl == nullptr)
	{
		writeBuf[0] = (uint8_t) 0x02;
		writeBuf[1] = (uint8_t) I2CCMD_NACK;
	}
	else
	{
		delay(80);
		ctrl->handleCommand(&cmdBuf[1], writeBuf);
	}
	writeBufLen = writeBuf[0];
	return writeBuf[0];
}

int ArduinoControl::initializeControls(const unsigned char * command, unsigned char * response)
{
	Command* ctrl;
	int offset = 0;
	unsigned long now = millis();
//	RGB Stripe
//		byte 2 set mode auto / manual (0x00 / 0x01)
//		bytes 3-5 set point (RGB)
//		bytes 6-8 upper limit (RGB)
//		bytes 9-11 lower limit (RGB)
//		bytes 12-14 speed (RGB)
//		bytes 15-17 starting point (RGB)
//		byte 18 swith off/on (0x00/0x01)
	ctrl = getCommand(CTRLID_RGBSTRIPE);

	((LedRGB *)ctrl)->setUpperBound((unsigned char *) &command[offset + 2]);
	((LedRGB *)ctrl)->setLowerBound((unsigned char *) &command[offset + 5]);
	((LedRGB *)ctrl)->setSpeed((unsigned char *) &command[offset + 8]);
	((LedRGB *)ctrl)->setActualValue((unsigned char *) &command[offset + 11]);
	(command[offset + 14] == 0 ? ((LedRGB *)ctrl)->turnManualOn() : ((LedRGB *)ctrl)->turnAutoOn());
	(command[offset + 15] == 0 ? ((LedRGB *)ctrl)->switchOff() : ((LedRGB *)ctrl)->switchOff());
	((LedRGB *)ctrl)->setup(now);

//	Starry sky
//		byte 2 set mode auto / manual (0x00 / 0x01)
//		bytes 3-5 set point (RGB)
//		bytes 6-8 upper limit (RGB)
//		bytes 9-12 lower limit (RGB)
//		bytes 13-14 speed (RGB)
//		bytes 15-17 starting point (RGB)
//		byte 18 swith off/on (0x00/0x01)

	response[1] = I2CCMD_ACK;
	response[0] = 2;
	initialized = true;
	return(2);
}

void ArduinoControl::setInitialized(bool flag)
{
	initialized = flag;
}

bool ArduinoControl::getInitialized()
{
	return initialized;
}

TimerManager ArduinoControl::getTimerManager()
{
	return(timerManager);
}
