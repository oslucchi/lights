/*
 * I2CComm.h
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */

#ifndef I2CCOMM_H_
#define I2CCOMM_H_
#include "Arduino.h"

#include <Wire.h>
#include <ArduinoControl.h>

#define I2C_BUS_ADDRESS	8

class I2CComm {
private:
//	void sendEvent();
//	void receiveEvent(int);
//	void clearChannel();
//	char* toHex(unsigned char);

public:
//	I2CComm(void (*)(int), void (*)());
	I2CComm(ArduinoControl *);
	void connectToBus();
	void receiveEventOnSerial();
	void setCallback();
	virtual ~I2CComm();
};

#endif /* I2CCOMM_H_ */
