/*
 * I2CComm.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */

#include <I2CComm.h>
#define	LOCAL_BUF_SIZE	128

ArduinoControl * arCtrl;


void clearChannel()
{
	while( Wire.available())
	{
		byte b = Wire.read();
	}
}

void fillBuffer(uint8_t *localBuf, uint8_t bytesRead)
{
	char msgBuf[64];
	sprintf((char *) msgBuf, "fillBuffer() init readIdx %d - writeIdx %d", arCtrl->readIdx, arCtrl->writeIdx);
	Serial.println(msgBuf);

	uint8_t writeIdxBck = arCtrl->writeIdx;
	for(int i = 0; i < bytesRead; i++)
	{
		if ((arCtrl->writeIdx == arCtrl->readIdx - 1) ||
			((arCtrl->writeIdx == LOCAL_BUF_SIZE - 1) && (arCtrl->readIdx == 0)))
		{
			arCtrl->writeIdx = writeIdxBck;
			arCtrl->writeBuf[0] = 0x02;
			arCtrl->writeBuf[1] = I2CCMD_NACK;
			arCtrl->writeBufLen = 2;
			return;
		}

		if (arCtrl->writeIdx == LOCAL_BUF_SIZE - 1) arCtrl->writeIdx = 0;
		arCtrl->readBuf[arCtrl->writeIdx++] = localBuf[i];
	}
	Serial.print("fillBuffer() - Incoming buf: ");
	for(int i = 0; i < bytesRead; i++)
		Serial.print(arCtrl->toHex(localBuf[i]));
	Serial.println();
	sprintf((char *) msgBuf, "fillBuffer() exit readIdx %d - writeIdx %d", arCtrl->readIdx, arCtrl->writeIdx);
	Serial.println((char *) msgBuf);
	return;
}

void I2CComm::receiveEventOnSerial()
{
	uint8_t bytesRead = 0;
	uint8_t localBuf[64];
	char readBuf[64];
    char *p = readBuf;
    char *str;
	char rc;
	memset(readBuf, '\0', sizeof(readBuf));
	while ((rc = Serial.read()) != '\n')
	{
		if (rc <32)
			continue;
		readBuf[bytesRead++] = rc;
		if (bytesRead >= sizeof(readBuf))
		{
			bytesRead = sizeof(readBuf) - 1;
			readBuf[bytesRead] = 0;
			break;
		}
	}

    bytesRead = 0;
    while ((str = strtok_r(p, " ", &p)) != NULL) // delimiter is the semicolon
	{
    	localBuf[bytesRead++] = atoi(str);
	}
	fillBuffer(localBuf, bytesRead);
}

void receiveEvent(int howMany)
{
	uint8_t localBuf[64];
	uint8_t bytesRead = (uint8_t) Wire.readBytes(localBuf, howMany);
	if (!_DEBUG_)
	{
		fillBuffer(localBuf, bytesRead);
	}
}

void sendEvent()
{
	if (arCtrl->writeBufLen == 0)
	{
		Serial.println("Answer not ready");
		uint8_t buffer[2];
		buffer[0] = 2;
		buffer[1] = I2CCMD_NOT_READY;
		Wire.write(buffer, 2);
	}
	else
	{
		Serial.print("Response pending (len ");
		Serial.print(arCtrl->writeBufLen);
		Serial.println("):");
		for(int y = 0; y < arCtrl->writeBufLen; y++)
		{
			;
			if (isprint(arCtrl->writeBuf[y]))
			{
				Serial.print("   ");
				Serial.print(arCtrl->writeBuf[y]);
				Serial.print(" ");
			}
			else
			{
				Serial.print(arCtrl->toHex(arCtrl->writeBuf[y]));
				Serial.print(" ");
			}
		}
		Serial.println();
		Wire.write((char *) arCtrl->writeBuf, (size_t) arCtrl->writeBufLen);
		memset(arCtrl->writeBuf, '\0', LOCAL_BUF_SIZE);
		arCtrl->writeBufLen = 0;
	}
}

I2CComm::I2CComm(ArduinoControl * control) {
	arCtrl = control;
}

void I2CComm::connectToBus() {
	Wire.begin(I2C_BUS_ADDRESS);
	Serial.print("On the i2c bus with address ");
	Serial.println(I2C_BUS_ADDRESS);
	clearChannel();
}

void I2CComm::setCallback()
{
	Wire.onReceive(receiveEvent);
	Wire.onRequest(sendEvent);
}

I2CComm::~I2CComm() {
}

