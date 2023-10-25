#ifndef SYSTEM5x7_H
#define SYSTEM5x7_H

#include <MDR32F1QI.h>                // Milandr::Device:Startup
#include "wchar.h"
#include "wctype.h"

typedef struct {
	const char *fontArray;
	uint8_t  width;
	uint8_t  height;
	uint8_t  cntChars;
} FontStruct;

extern FontStruct MyFont;

#endif
