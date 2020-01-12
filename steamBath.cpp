// Do not remove the include below
#include "steamBath.h"

I2CComm *i2c;
ArduinoControl *ac;
unsigned long startTime;
uint8_t status = 1;
uint16_t blinkTime = 1000;

//The setup function is called once at startup of the sketch
void setup()
{
	//	i2c = new I2CComm(&receiveEvent, &sendEvent);
	//	Serial.print("On the i2c bus with address ");
	//	Serial.println(I2C_BUS_ADDRESS);
	//	Wire.onReceive(receiveEvent);
	//	Wire.onRequest(sendEvent);

	Serial.begin(19200);
	Serial.println("Starting controls");
	delay(2000);
	ac = new ArduinoControl();
	i2c = new I2CComm(ac);
	if (_DEBUG_)
	{
		blinkTime = 100;
	}
	else
	{
		// I2C to communicate
		i2c->connectToBus();
		i2c->setCallback();
	}
	startTime = millis();
	digitalWrite(LED_BUILTIN, status);
	ac->writeIdx = 0;
	ac->readIdx = 0;
	memset(ac->readBuf, '\0', sizeof(ac->readBuf));
	ac->getTimerManager().dump();
}

// The loop function is called in an endless loop
void loop()
{
	unsigned long now = millis();
	if (_DEBUG_ && Serial.available())
	{
		Serial.println("New data arrived");
		i2c->receiveEventOnSerial();
	}
	ac->loop(now);
	if (_DEBUG_ && (ac->writeBufLen != 0))
	{
		Serial.println("From control:");
		for(int i = 0; i < ac->writeBufLen; i++)
		{
			Serial.print(ac->toHex(ac->writeBuf[i]));
		}
		Serial.println();
		memset(ac->writeBuf, '\0', sizeof(ac->writeBuf));
		ac->writeBufLen = 0;
	}
	if (now - startTime > blinkTime)
	{
		digitalWrite(LED_BUILTIN, (status = !status));
		startTime = now;
	}

}


