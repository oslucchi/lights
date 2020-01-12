/*
 * CommandHandler.h
 *
 *  Created on: Feb 15, 2019
 *      Author: osvaldo
 */

#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_
#include "Command.h"

#define  PRO_MINI_1 0
#define  PRO_MINI_2 1
#define	 MAX_CONTROLLERS	2

#define  I2CCMD_ACK (byte)0x00
#define  I2CCMD_NACK (byte)0xFF
#define  I2CCMD_NOT_READY	(byte) 0xFE
#define  I2CCMD_INITIALIZE_UNIT 0x01
#define  I2CCMD_GET_STATUS 0x02
#define  I2CCMD_SET_STATUS 0x03

#define  I2C_ADDR_BUS 0
#define  I2C_ADDR_STARRYSKY_RED 1
#define  I2C_ADDR_STARRYSKY_GREEN 2
#define  I2C_ADDR_STARRYSKY_BLUE 3
#define  I2C_ADDR_LIGHTSTRIPE_RED 4
#define  I2C_ADDR_LIGHTSTRIPE_GREEN 5
#define  I2C_ADDR_LIGHTSTRIPE_BLUE 6
#define  I2C_ADDR_HUMIDITY_AND_TEMP 7
#define  I2C_ADDR_SG_TEMP 8
#define  I2C_ADDR_SG 9

#define  I2C_ADDR_WATER_IN_A 1
#define  I2C_ADDR_WATER_IN_B 2
#define  I2C_ADDR_WATER_OUT_A 3
#define  I2C_ADDR_WATER_OUT_B 4
#define  I2C_ADDR_WATER_RINSE 5
#define  I2C_ADDR_OVERFLOW 6
#define  I2C_ADDR_UNDERFLOW 7
#define  I2C_ADDR_RINSE_PUMP 8
#define  I2C_ADDR_RINSE_FAN 9

typedef struct _cmdList {
	Command *cmd;
	struct _cmdList* next;
} cmdList;

class CommandHandler {
	cmdList controllerRoot;
	cmdList *current;
	TimerManager *timerManager;

public:
	CommandHandler();
	virtual ~CommandHandler();
	void addCommand(unsigned char ctrlId, byte ctrlType, uint8_t* pin, uint8_t* upperBound,
					uint8_t *lowerBound, uint8_t* actualValue, TimerManager *tm, char *ctrlName);
	cmdList *getController();
};

#endif /* COMMANDHANDLER_H_ */
