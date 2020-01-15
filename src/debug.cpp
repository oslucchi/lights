#include <Arduino.h>
#include <stdarg.h>

#ifdef _DEBUG_
void	_DEBUG( int level, ... )
{
	const char	*fmt;
	va_list	dargs;
	char msgBuf[256];

	va_start(dargs, level);
	fmt = va_arg(dargs, char *);

	vsprintf(msgBuf, fmt, dargs );
//	Serial.println(msgBuf);
}
#endif
