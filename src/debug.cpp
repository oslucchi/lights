#include <Arduino.h>
#include <stdarg.h>

#ifdef _DEBUG_
void	_DEBUG( const char *module, ... )
{
	const char	*fmt;
	va_list	dargs;
	char msgBuf[256];

	va_start(dargs, module);
	fmt = va_arg(dargs, char *);

	if (strcmp(module, MODULE) ==0)
	{
		vsprintf(msgBuf, fmt, dargs );
		Serial.println(msgBuf);
	}
}
#endif
