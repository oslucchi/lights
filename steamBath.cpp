// Do not remove the include below
#include "steamBath.h"

I2CComm *i2c;
ArduinoControl *ac;
unsigned long startTime;
uint8_t status = 1;
uint16_t blinkTime = 1000;
char msgBuf[MAX_DBG_LEN];

//The setup function is called once at startup of the sketch
void setup()
{
	Serial.begin(19200);
	Serial.println("Starting controls");
	delay(2000);
	ac = new ArduinoControl();
	i2c = new I2CComm(ac);

#ifdef _DEBUG_
	blinkTime = 100;
#else
	// I2C to communicate
	i2c->connectToBus();
	i2c->clearChannel();
	i2c->setCallback();
#endif

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
#ifdef _DEBUG_
	if (Serial.available())
	{
		i2c->receiveEventOnSerial();
	}
#endif
	ac->loop(now);
	if (now - startTime > blinkTime)
	{
		digitalWrite(LED_BUILTIN, (status = !status));
		startTime = now;
	}
}


