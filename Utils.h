#pragma once
#ifndef UTILS_H
#define UTILS_H

#include "Jukebox.h"

#define JBOX_TRACE_

void TraceMultiple(const char* format, ...) {
	char temp[256];
	va_ list args;
	va_ start(args,fmt);

	vsprintf(temp, format,args);

	JBOX_TRACE(temp);
    
	va_ end(args)
}

#endif