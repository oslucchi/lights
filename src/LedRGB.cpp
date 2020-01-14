/*
 * LedStripeHandler.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */
#include <LedRGB.h>
#include "I2CComm.h"
#include <debug.h>
#include <steamBath.h>

unsigned long now = 0;

LedRGB::LedRGB()
{
	fadingTmr = NULL;
}

void LedRGB::setup(unsigned long now)
{
	leds[RED].upperBound = 255;
	leds[GREEN].upperBound = 255;
	leds[BLUE].upperBound = 255;
	leds[RED].lowerBound = 0;
	leds[GREEN].lowerBound = 0;
	leds[BLUE].lowerBound = 0;
	leds[RED].actualValue = 0;
	leds[GREEN].actualValue = 0;
	leds[BLUE].actualValue = 0;
	leds[RED].direction = 1;
	leds[GREEN].direction = 1;
	leds[BLUE].direction = 1;
	leds[RED].speed = 1;
	leds[GREEN].speed = 1;
	leds[BLUE].speed = 1;
	leds[RED].timer = timerManager->getNewTimer("RED");
	leds[RED].timer->setDuration(1000);
	leds[GREEN].timer = timerManager->getNewTimer("GREEN");
	leds[GREEN].timer->setDuration(1000);
	leds[BLUE].timer = timerManager->getNewTimer("BLUE");
	leds[BLUE].timer->setDuration(1000);
	fadingTmr = timerManager->getNewTimer("Fader");
}

void LedRGB::loop(unsigned long now)
{
	switch(mode)
	{
	case MODE_AUTO:
		for(int i = 0; i < 3; i++)
		{
			if (leds[i].timer->getIsExpired())
			{
				pwmBrightness(i);
				leds[i].timer->restart(now);
			}
		}
		break;

	case MODE_MANUAL:
		if ((fading != FADING_OFF) && fadingTmr->getIsExpired())
		{
			for(int i = 0; i < 3; i++)
			{
				if (fading == FADING_IN )
				{
					leds[i].actualValue = leds[i].lowerBound;
					leds[i].direction = 1;
					fading = FADING_OUT;
				}
				else
				{
					leds[i].actualValue = leds[i].upperBound;
					leds[i].direction = -1;
					fading = FADING_IN;
				}
			}
			turnAutoOn();
			fadingTmr->restart(now);
			Debug(( "RGB", "Manual part done" ));
		}
		break;

	default:
		Debug(( "RGB", "running in unknown mode" ));
	}
	for(int i = 0; i < 3; i++)
	{
		analogWrite(leds[i].pin, leds[i].actualValue * status);
	}
}

int LedRGB::getActualValues(unsigned char *response)
{
	response[RED] = leds[RED].actualValue;
	response[GREEN] = leds[GREEN].actualValue;
	response[BLUE] = leds[BLUE].actualValue;
	return(3);
}

void LedRGB::handleCommand(const unsigned char * command, unsigned char * response)
{
	bool switchOnFlag;

	switch(command[0])
	{
	case 0x01:
		Debug(( "RGB", "I2CCMD_GET" ));
		response[0] = 5;
		response[1] = I2CCMD_ACK;
		response[2] = leds[RED].actualValue;
		response[3] = leds[GREEN].actualValue;
		response[4] = leds[BLUE].actualValue;
		break;

	case 0x02:
		Debug(( "RGB", "I2CCMD_SET_MANUAL" ));
		setActualValue((uint8_t *)&command[1]);
		turnManualOn();
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x03:
		Debug(( "RGB", "I2CCMD_SET_AUTO" ));
		setUpperBound((uint8_t *)&command[1]);
		setLowerBound((uint8_t *)&command[4]);
		setSpeed((uint8_t *)&command[7]);
		setActualValue((uint8_t *)&command[10]);
		setTimers((uint8_t *) &command[13]);
		turnAutoOn();
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x04:
		Debug(( "RGB", "I2CCMD_SET_BRIGHTNESS" ));
		leds[RED].actualValue = command[1];
		leds[GREEN].actualValue = command[2];
		leds[BLUE].actualValue = command[3];
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x05:
		Debug(( "RGB", "I2CCMD_SWITCH_ON_OFF" ));
		switchOnFlag = (bool) command[1];
		if (switchOnFlag)
			switchOn();
		else
			switchOff();
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x06:
		Debug(( "RGB", "I2CCMD_SET_MANUAL" ));
		setSpeed((uint8_t *)&command[1]);
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x07:
		Debug(( "RGB", "I2CCMD_RESET_TIMERS" ));
		leds[RED].timer->restart(now);
		leds[GREEN].timer->restart(now);
		leds[BLUE].timer->restart(now);
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x08:
		Debug(( "RGB", "I2CCMD_SET_TIMERS" ));
		setTimers((uint8_t *) &command[1]);
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x09:
		Debug(( "RGB", "I2CCMD_FADE" ));
		if (command[1] == 1)
		{
			memcpy((void *)&fadingTimeIn, (void *) &command[3], sizeof(uint16_t));
			memcpy((void *)&fadingTimeout, (void *) &command[5], sizeof(uint16_t));
			fading = (command[2] == 0 ? FADING_OUT : FADING_IN);
			fadingTmr->setDuration(fading == FADING_OUT ? (uint8_t *)&fadingTimeout : (uint8_t *)&fadingTimeIn);
			fadingTmr->start(millis());
		}
		else
		{
			if (fading == FADING_OUT)
			{
				for(int i = 0; i < 3; i++)
				{
					leds[i].actualValue = leds[i].lowerBound;
					leds[i].direction = 1;
				}
			}
			else if (fading == FADING_IN)
			{
				for(int i = 0; i < 3; i++)
				{
					leds[i].actualValue = leds[i].upperBound;
					leds[i].direction = -1;
				}
			}
			fading = FADING_OFF;
			fadingTmr->reset();
		}
		turnAutoOn();
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;
	}
}

void LedRGB::setMode(uint8_t mode)
{
	this->mode = mode;
}

void LedRGB::switchOn()
{
	status = POWER_ON;
}

void LedRGB::switchOff()
{
	status = POWER_OFF;
}

void LedRGB::turnManualOn()
{
	mode = MODE_MANUAL;
}

void LedRGB::turnAutoOn()
{
	mode = MODE_AUTO;
	for(int i = 0; i < 3; i++)
	{
		leds[i].direction = 1;
		leds[i].timer->start(now);
	}
}

void LedRGB::setUpperBound(uint8_t* ub)
{
	leds[RED].upperBound = ub[0];
	leds[GREEN].upperBound = ub[1];
	leds[BLUE].upperBound = ub[2];
}

void LedRGB::setLowerBound(uint8_t* lb)
{
	leds[RED].lowerBound = lb[0];
	leds[GREEN].lowerBound = lb[1];
	leds[BLUE].lowerBound = lb[2];
}

void LedRGB::setSpeed(uint8_t* s)
{
	leds[RED].speed = s[0];
	leds[GREEN].speed = s[1];
	leds[BLUE].speed = s[2];
}

void LedRGB::setTimers(uint8_t* s)
{
	unsigned long now = millis();
	leds[RED].timer->setDuration(&s[0]);
	leds[GREEN].timer->setDuration(&s[2]);
	leds[BLUE].timer->setDuration(&s[4]);
	leds[RED].timer->restart(now);
	leds[GREEN].timer->restart(now);
	leds[BLUE].timer->restart(now);
}

void LedRGB::setActualValue(uint8_t* sv)
{
	leds[RED].actualValue = sv[0];
	leds[GREEN].actualValue = sv[1];
	leds[BLUE].actualValue = sv[2];
}

void LedRGB::setPin(uint8_t * pin)
{
	leds[RED].pin = pin[0];
	leds[GREEN].pin = pin[1];
	leds[BLUE].pin = pin[2];

}

void LedRGB::fade()
{
	fading = true;
}

boolean LedRGB::handleBrightnessNormal(uint8_t led)
{
	static boolean changeDirection;
	changeDirection = false;
	short value;
	value = leds[led].actualValue + leds[led].direction * leds[led].speed;
	if (value >= leds[led].upperBound && leds[led].direction == 1)
	{
		value = leds[led].upperBound;
		changeDirection = true;
	}
	else if (value <= leds[led].lowerBound && leds[led].direction == -1)
	{
		value = leds[led].lowerBound;
		changeDirection = true;
	}
	leds[led].actualValue = value;
	return(changeDirection);
}

void LedRGB::handleBrightnessFading(uint8_t led)
{
	if (fadingTmr->getIsExpired())
	{
		if (fadingTmr->getIsFront())
		{
			Debug(( "RGB", "%s fading time expired", this->controlName ));
			for(int i = 0; i < 3; i++)
			{
				leds[i].direction = (fading == FADING_IN ? 1 : -1);
			}
		}

		boolean fadingIsOver = true;
		// fading is only an Auto feature. At the end of the action, the
		// command needs to get back to Manual
		for(int i = 0; i < 3; i++)
		{
			if (((fading == FADING_IN) && (leds[i].actualValue < leds[i].upperBound)) ||
				((fading == FADING_OUT) && (leds[i].actualValue > leds[i].lowerBound)))
			{
				fadingIsOver = false;
			}
		}
		if (fadingIsOver)
		{
			fadingTmr->setDuration((fading == FADING_IN ? (uint8_t*) &fadingTimeIn : (uint8_t*) &fadingTimeout));
			fading = (fading == FADING_IN ? FADING_OUT : FADING_IN);
			Debug(( "RGB", "%s moving to manual", this->controlName ));
			turnManualOn();
			fadingTmr->restart(millis());
		}
		else
			handleBrightnessNormal(led);
		return;
	}
	else
	{
		if (handleBrightnessNormal(led))
		{
			Debug(( "RGB", "handleBrightnessFading: change direction for automode" ));
			leds[led].direction *= -1;
		}
	}
}

void LedRGB::pwmBrightness(uint8_t led)
{
	if (fading != FADING_OFF)
		handleBrightnessFading(led);
	else
	{
		if (handleBrightnessNormal(led))
		{
			Debug(( "RGB", "pwmBrigthenss: change direction for automode" ));
			leds[led].direction *= -1;
		}
	}
}
