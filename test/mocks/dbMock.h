// dbMock.h

#ifndef DB_MOCK_H
#define DB_MOCK_H

#include <stdint.h>
#include <stdbool.h>


#define LOG_SIZE 10

typedef struct {
    uint32_t timeStamp;
    uint8_t eventId;
    uint8_t value8b;
    uint16_t value16b;
} mockRecord;



bool DB_setRecord(const uint8_t tableId,
                  const uint8_t recordId,
                  const uint16_t* data);




// functions for setup / inspection:


void DB_MOCK_init(void);

mockRecord DB_MOCK_getRecord(uint16_t recordId);

#endif
