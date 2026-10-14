// LittleLog.c

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "LittleLog.h"


static uint8_t LogSize = 0;
static uint8_t RecordId = 0;
static LL_getTimeStampFn GetTimeStampFn = NULL;
static LL_writeLogRecordFn WriteLogRecordFn = NULL;
static LL_readLogEventIdFn ReadLogEventIdFn = NULL;


/* if no empty record was found, it returns 0 to allow the log system to
 * recover from an invalid state and operate normally.
 */
uint8_t findFirstEmptyRecord(void) {
    for (uint8_t i = 0; i < LogSize; i++) {
        if (ReadLogEventIdFn(i) == LL_EVENT_EMPTY) {
            return i;
        }
    }
    return 0;
}


void LL_initLog(const uint8_t logSize,
                LL_getTimeStampFn getTimeStampFn,
                LL_writeLogRecordFn writeLogRecordFn,
                LL_readLogEventIdFn readLogEventIdFn) {
    assert(logSize > 1);
    assert(getTimeStampFn != NULL);
    assert(writeLogRecordFn != NULL);
    assert(readLogEventIdFn != NULL);
    LogSize = logSize;
    GetTimeStampFn = getTimeStampFn;
    WriteLogRecordFn = writeLogRecordFn;
    ReadLogEventIdFn = readLogEventIdFn;
    RecordId = findFirstEmptyRecord();
}


void LL_closeLog(void) {
    LogSize = 0;
    RecordId = 0;
    GetTimeStampFn = NULL;
    WriteLogRecordFn = NULL;
    ReadLogEventIdFn = NULL;
}


void writeLogRecord(const uint8_t eventId,
                    const uint8_t value8b,
                    const uint16_t value16b) {
    assert(GetTimeStampFn != NULL);
    assert(WriteLogRecordFn != NULL);
    uint32_t timeStamp = GetTimeStampFn();
    uint16_t data[5] = {
        [0] = timeStamp >> 16,
        [1] = timeStamp & 0x0FFFF,
        [2] = eventId,
        [3] = value8b,
        [4] = value16b,
    };
    bool recordSchemaIsValid = WriteLogRecordFn(RecordId, data);
    assert(recordSchemaIsValid);
}


void clearRecord(void) {
    writeLogRecord(LL_EVENT_EMPTY, 0, 0);
}


void nextRecord(void) {
    RecordId++;
    if (RecordId >= LogSize) {
        RecordId = 0;
    }
}


void logEvent(const uint8_t eventId,
              const uint8_t value8b,
              const uint16_t value16b) {
    writeLogRecord(eventId, value8b, value16b);
    nextRecord();
    clearRecord();
}


void LL_logEvent(const uint8_t eventId,
                 const uint8_t value8b,
                 const uint16_t value16b) {
    assert (eventId != LL_EVENT_BOOT && eventId != LL_EVENT_EMPTY);
    logEvent(eventId, value8b, value16b);
}


void LL_logBootEvent(const uint8_t fwVersionMajor,
                     const uint16_t fwVersionMinor) {
    logEvent(LL_EVENT_BOOT, fwVersionMajor, fwVersionMinor);
}


void LL_clearLog(void) {
    for (RecordId = 0; RecordId < LogSize; RecordId++) {
        clearRecord();
    }
    RecordId = 0;
}
