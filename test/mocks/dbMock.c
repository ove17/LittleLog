// dbMock.c

#include <stdbool.h>
#include <stdint.h>


#include "dbMock.h"

static mockRecord table[LOG_SIZE];


bool DB_setRecord(const uint8_t tableId,
                  const uint8_t recordId,
                  const uint16_t* data) {
    table[recordId] = (mockRecord){
        .timeStamp = (data[0] << 16) + data[1],
        .eventId = (uint8_t)data[2],
        .value8b = (uint8_t)data[3],
        .value16b = data[4]
    };
    return true;
}


// functions for setup / inspection:


void DB_MOCK_init(void) {
    for (uint8_t i = 0; i < LOG_SIZE; i++) {
        table[i] = (mockRecord){
            .timeStamp = 0xFFFFFFFF,
            .eventId = 0xFF,
            .value8b = 0xFF,
            .value16b = 0xFFFF,
        };
    }
}


mockRecord DB_MOCK_getRecord(uint16_t recordId) {
    return table[recordId];
}
