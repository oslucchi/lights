/*
 * I2CComm.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */

#include <I2CComm.h>
#include <debug.h>
#include <steamBath.h>

ArduinoControl * arCtrl;

void I2CComm::clearChannel()
{
	while( Wire.available())
	{
		Wire.read();
	}
}

void fillBuffer(uint8_t *localBuf, uint8_t bytesRead)
{
	DebugI2CComm(( 1, "fillBuffer() r %d - w %d", arCtrl->readIdx, arCtrl->writeIdx ));
//	Serial.print("fillBuffer() r ");
//	Serial.print(arCtrl->readIdx);
//	Serial.print(" w ");
//	Serial.println(arCtrl->writeIdx );
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

//	for(int i = 0; i < bytesRead; i++)
//	{
//		msgBuf[i] = arCtrl->toHex(localBuf[i]);
//	}
	DebugI2CComm(( 1, "fillBuffer()-incoming: '%s'", arCtrl->toHex(localBuf) ));
	DebugI2CComm(( 1, "fillBuffer() readIdx %d - writeIdx %d", arCtrl->readIdx, arCtrl->writeIdx ));
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
		if (rc < 32)
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
    while ((str = strtok_r(p, " ", &p)) != NULL)
	{
    	localBuf[bytesRead++] = atoi(str);
	}
	fillBuffer(localBuf, bytesRead);
}

void receiveEvent(int howMany)
{
	uint8_t localBuf[32];
	uint8_t bytesRead = (uint8_t) Wire.readBytes(localBuf, howMany);
	fillBuffer(localBuf, bytesRead);
}

void sendEvent()
{
	if (arCtrl->writeBufLen == 0)
	{
		DebugI2CComm(( 1, "Answer not ready" ));
		uint8_t buffer[2];
		buffer[0] = 2;
		buffer[1] = I2CCMD_NOT_READY;
		Wire.write(buffer, 2);
	}
	else
	{
		DebugI2CComm(( 1, "Response pending (len %d)", arCtrl->writeBufLen ));
		memset(msgBuf, '\0', sizeof(msgBuf));
		for(int y = 0; y < arCtrl->writeBufLen; y++)
		{
			if (isprint(arCtrl->writeBuf[y]))
			{
				msgBuf[strlen(msgBuf)] = arCtrl->writeBuf[y];
			}
			else
			{
				strcat(msgBuf, arCtrl->toHex(arCtrl->writeBuf[y]));
			}
			strcat(msgBuf, " ");
		}
		DebugI2CComm(( 1, "Response pending (len %d)", arCtrl->writeBufLen ));
#ifndef _DEBUG_
		Wire.write(arCtrl->writeBuf, (size_t) arCtrl->writeBufLen);
#else
		Serial.println(msgBuf);
#endif
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
