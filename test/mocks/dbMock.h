// dbMock.h

#ifndef DB_MOCK_H
#define DB_MOCK_H

#include <stdint.h>
#include <stdbool.h>


typedef struct {
    uint32_t timeStamp;
    uint8_t eventId;
    uint8_t value8b;
    uint16_t value16b;
} mockRecord;



bool DB_setRecord(const uint8_t recordId,
                  const uint16_t* data);

uint8_t DB_getValue(const uint8_t recordId);


// functions for setup / inspection:


void DB_MOCK_init(void);

mockRecord DB_MOCK_getRecord(uint16_t recordId);

void DB_MOCK_dump(const uint8_t numRecords);

#endif
