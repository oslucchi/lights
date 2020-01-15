/*
 * LedStripeHandler.h
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */

#ifndef LEDRGB_H_
#define LEDRGB_H_

#include "Arduino.h"
#include <Command.h>
// #include <LedPwm.h>
#include <TimerManager.h>

#define	POWER_OFF	0
#define	POWER_ON	1
#define	MODE_MANUAL		0
#define	MODE_AUTO		1

#define RED_PIN		5
#define GREEN_PIN	6
#define BLUE_PIN	9
#define	RED			0
#define	GREEN		1
#define	BLUE		2

#define FADING_OFF	0
#define FADING_IN	1
#define FADING_OUT	2

typedef struct _Led {
	uint8_t 	pin = -1;
	uint8_t 	actualValue = 0;
	uint8_t		upperBound = 255;
	uint8_t		lowerBound = 0;
	int8_t 		direction = 1;
	uint8_t		speed = 1;
	Timer*		timer = nullptr;
} Led;

class LedRGB : public Command {
private:
	Led		leds[3];
	uint8_t faderLowerBound[3];
	uint8_t autoLowerBound[3];
	uint8_t	mode = 0; // 0 - manual / 1 - auto
	uint8_t	status = 0; // 0 - off / 1 - on
	uint8_t	fading = FADING_OFF;
	uint16_t fadingTmrAuto = 0, fadingTmrManual = 0;
	Timer *fadingTmr;
	void handleBrightnessFading();
	boolean handleBrightnessNormal(uint8_t led);
public:
	LedRGB();
	int getActualValues(unsigned char *);
	void setup(unsigned long mills);
	void loop(unsigned long mills);
	void handleCommand(const unsigned char *inBuf, unsigned char * outBuf);
	void switchOn();
	void switchOff();
	void turnManualOn();
	void turnAutoOn();
	void fade();
	void setUpperBound(uint8_t *);
	void setLowerBound(uint8_t *);
	void setSpeed(uint8_t *);
	void setActualValue(uint8_t *);
	void setPin(uint8_t*);
	void setMode(uint8_t mode);
	void setTimers(uint8_t* s);
};


#endif /* LEDRGB_H_ */
