/*
 * ArduinoControl.h
 *
 *  Created on: Feb 16, 2019
 *      Author: osvaldo
 */

#ifndef ARDUINOCONTROL_H_
#define ARDUINOCONTROL_H_
#include "Arduino.h"
#include <Timer.h>
#include <CommandHandler.h>

#define	TYPE_LED_PWM			1
#define	TYPE_DIGITAL_OUT		2
#define	TYPE_VALVE_TRISTATE		3
#define	TYPE_DIGITAL_IN			4
#define	TYPE_LED_RGB			5

#define CTRLID_RGBSTRIPE		(uint8_t) 1
#define CTRLID_STARRYSKY		(uint8_t) 2
#define CTRLID_STEAM_POWER		(uint8_t) 3
#define CTRLID_STEAM_WATER_IN	(uint8_t) 4
#define CTRLID_STEAM_WATER_OUT	(uint8_t) 5
#define CTRLID_HUMIDITY_SENSOR	(uint8_t) 6

#define	LOCAL_BUF_SIZE			64

class ArduinoControl {
	CommandHandler lights;
	bool initialized;
	TimerManager timerManager;

public:
	uint8_t readBuf[LOCAL_BUF_SIZE];
	uint8_t writeBuf[LOCAL_BUF_SIZE];
	uint8_t writeIdx = 0;
	uint8_t	readIdx = 0;
	uint8_t writeBufLen = 0;

	ArduinoControl();
	virtual ~ArduinoControl();
	void setPin(int pin[]);
	void setTimer(Timer *t[]);
	Timer *getTimer(int idx);
	TimerManager getTimerManager();
	void setup(unsigned long now);
	void loop(unsigned long now);
	int handleI2CCommand();
	int initializeControls(const uint8_t * command, uint8_t * response);
	Command* getCommand(uint8_t id);
	void setInitialized(bool flag);
	bool getInitialized();
	char *toHex(uint8_t);
};

#endif /* ARDUINOCONTROL_H_ */
