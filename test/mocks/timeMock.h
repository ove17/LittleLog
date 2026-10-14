// timeMock.h

#ifndef TIME_MOCK_H
#define TIME_MOCK_H

#include <stdint.h>


uint32_t TM_getTimeStamp(void);

// functions for setup / inspection:


void TIME_MOCK_init(void);

void TIME_MOCK_setTimeStamp(const uint32_t timeStamp);

#endif
