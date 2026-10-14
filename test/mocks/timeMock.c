// timeMock.c

#include <stdint.h>
#include "timeMock.h"


static uint32_t TimeStamp = 0;


uint32_t TM_getTimeStamp(void) {
    return TimeStamp;
}


// functions for setup / inspection:


void TIME_MOCK_init(void) {
    TimeStamp = 0;
}


void TIME_MOCK_setTimeStamp(const uint32_t timeStamp) {
    TimeStamp = timeStamp;
}
