// dbMock.c

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>


#include "dbMock.h"

#define MAX_LOG_SIZE 0xFF

static mockRecord table[MAX_LOG_SIZE];


bool DB_setRecord(const uint8_t recordId,
                  const uint16_t* data) {
    table[recordId] = (mockRecord){
        .timeStamp = (data[0] << 16) + data[1],
        .eventId = (uint8_t)data[2],
        .value8b = (uint8_t)data[3],
        .value16b = data[4]
    };
    return true;
}


uint8_t DB_getValue(const uint8_t recordId) {
    return table[recordId].eventId;
}


// functions for setup / inspection:


void DB_MOCK_init(void) {
    for (uint8_t i = 0; i < MAX_LOG_SIZE; i++) {
        table[i] = (mockRecord){
            .timeStamp = 0xFFFFFFFF,
            .eventId = 0xFF,
            .value8b = 0xFF,
            .value16b = 0xFFFF,
        };
    }
}


void DB_MOCK_dump(const uint8_t numRecords) {
    printf("\n");
    for (uint8_t i = 0; i < numRecords; i++) {
        printf("%u\t%i\t%i\t%i\t\n", table[i].timeStamp, table[i].eventId, table[i].value8b, table[i].value16b);
    }
    printf("\n");
}


mockRecord DB_MOCK_getRecord(uint16_t recordId) {
    return table[recordId];
}
