/*
 * TimerManager.h
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */

#ifndef TIMERMANAGER_H_
#define TIMERMANAGER_H_

#include <Timer.h>
#define	HOW_MANY	10


struct _timers {
	Timer	*timer;
	bool	allocated = false;
};

class TimerManager {
	_timers timers[HOW_MANY];
private:
	;
public:
	TimerManager();
	void update(unsigned long);
	Timer* getNewTimer(const char *);
	void dropTimer(Timer* timer);
	void dump();
};

#endif /* TIMERMANAGER_H_ */
