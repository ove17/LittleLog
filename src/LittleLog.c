// LittleLog.c

#include <stdint.h>
//#include <stdbool.h>
#include <assert.h>
#include "LittleLog.h"


#include "dbMock.h"


/*
 * The following eventId values are for internal use only  and must not be used
 *  in an LL_logEvent() call.
 */
enum {
    LL_EVENT_ID_BOOT = 0,
    LL_EVENT_ID_EMPTY = 0xFF
};


static uint8_t TableId = 0;
static uint8_t RecordId = 0;


void LL_init(const uint8_t logTableId) {
    TableId = logTableId;
    RecordId = 0;
}


void LL_logEvent(const uint32_t timeStamp,
                 const uint8_t eventId,
                 const uint8_t value8b,
                 const uint16_t value16b) {
    assert (eventId != LL_EVENT_ID_BOOT && eventId != LL_EVENT_ID_EMPTY);
    uint16_t data[5];
    data[0] = timeStamp >> 16;
    data[1] = timeStamp & 0x0FFFF;
    data[2] = eventId;
    data[3] = value8b;
    data[4] = value16b;
    DB_setRecord(TableId, RecordId, data);
    RecordId++;
}
