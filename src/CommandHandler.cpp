/*
 * CommandHandler.cpp
 *
 *  Created on: Feb 15, 2019
 *      Author: osvaldo
 */

#include "Arduino.h"

#include <CommandHandler.h>
#include <ArduinoControl.h>
#include <LedRGB.h>


void CommandHandler::addCommand(uint8_t ctrlId, byte ctrlType, uint8_t* pin, uint8_t* upperBound,
								uint8_t *lowerBound, uint8_t* actualValue, TimerManager* timerManager, const char *ctrlName)
{
	unsigned long now = millis();

	current->next = (cmdList *) malloc(sizeof(cmdList));
	current = current->next;
	switch(ctrlType)
	{
//	case TYPE_LED_PWM:
//		current->cmd = new LedPwm();
//		break;
//	case TYPE_DIGITAL_OUT:
//		current->cmd = new DigitalOutput();
//		break;
//	case TYPE_DIGITAL_IN:
//		current->cmd = new DigitalInput();
//		break;
//	case TYPE_VALVE_TRISTATE:
//		current->cmd = new ValveTriState();
//		break;
	case TYPE_LED_RGB:
		current->cmd = new LedRGB();
		((LedRGB *)(current->cmd))->setMode(MODE_MANUAL);
		((LedRGB *)(current->cmd))->switchOff();
	}
	current->cmd->ctrlId = ctrlId;
	current->cmd->ctrlType = ctrlType;
	current->cmd->setUpperBound(upperBound);
	current->cmd->setLowerBound(lowerBound);
	current->cmd->setActualValue(actualValue);
	current->cmd->setPin(pin);
	current->cmd->setTimerManager(timerManager);
	current->cmd->setup(now);
	current->cmd->setControlName(ctrlName);
	current->next = nullptr;

	return;
}

CommandHandler::CommandHandler()
{
	controllerRoot.next = nullptr;
	current = &controllerRoot;
	timerManager = nullptr;
}

CommandHandler::~CommandHandler()
{
	cmdList *next;
	if(controllerRoot.next == nullptr)
		return;
	current = controllerRoot.next;
	{
		next = current->next;
		free(current);
		current = next;
	} while(current != nullptr);

	controllerRoot.next = nullptr;
}

cmdList* CommandHandler::getController()
{
	return &controllerRoot;
}
