// LittleLog.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "LittleLog.h"
    #include "mocks/dbMock.h"
}


/*
 * write location
 *      Lege database, eerste EMPTY, meerdere EMPTY-markers.
 *      Wrap-around, volle buffer en herhaald overschrijven.


4 Initialisatie en boot-event
    Versie, timestamp, hervatten na reboot.

    logEvent_onMultipleEmptyRecords_usesFirstEmptyRecord*
    logEvent_onFilledLog_writesToFirstEmptyRecord

* initLog_onEmptyLog_writesBootEventToRecord0
 * initLog_onEmptyLog_startsAtRecord0
 * bootEventContains_VERSIONversion_fromInitInput
 * bootEventContains_time_fromTimeSource
 * initLog_onFilledLog_writesBootEvent_ToFirstEmptyRecord
 * initLog_rollsOverRecordId_whenAtLastRecord
 * initLog_onMultipleEmptyRecords_usesFirstEmptyRecord
 * initLog_afterPreviousSession_reconstructsWritePosition
 *
 */

#define DUMMY 99    // any byte not 0 or 0xFF

TEST_GROUP(testMocks) {
    void setup() {
        DB_MOCK_init();
        LL_init(DUMMY);
    }

    void teardown() {
    }
};


TEST(testMocks,
     emptyLogTableIsFilledWith0xFF) {
    for (uint8_t i = 0; i < LOG_SIZE; i++) {
        BYTES_EQUAL(0xFFFFFFFF, DB_MOCK_getRecord(i).timeStamp);
        BYTES_EQUAL(0xFF, DB_MOCK_getRecord(i).eventId);
        BYTES_EQUAL(0xFF, DB_MOCK_getRecord(i).value8b);
        BYTES_EQUAL(0xFFFF, DB_MOCK_getRecord(i).value16b);
    }
}


TEST(testMocks,
     getRecordReturnsSetRecord) {
    uint8_t recordId = 4;
    uint16_t data[5] = {1, 2, 3, 4, 5};
    DB_setRecord(DUMMY, recordId, data);
    LONGS_EQUAL(0x00010002, DB_MOCK_getRecord(recordId).timeStamp); // MSB(W) 1st
    BYTES_EQUAL(0x03, DB_MOCK_getRecord(recordId).eventId);
    BYTES_EQUAL(0x04, DB_MOCK_getRecord(recordId).value8b);
    LONGS_EQUAL(0x0005, DB_MOCK_getRecord(recordId).value16b);
}


///////


TEST_GROUP(writeLog) {
    void setup() {
        DB_MOCK_init();
        LL_init(DUMMY);
    }

    void teardown() {
    }
};


TEST(writeLog,
     logEvent_onEmptyTable_writesRecord0) {
    uint8_t eventId = 99;
    LL_logEvent(DUMMY, eventId, DUMMY, DUMMY);
    BYTES_EQUAL(eventId, DB_MOCK_getRecord(0).eventId);
}


TEST(writeLog,
     secondLogEvent_onEmptyTable_writesRecord1) {
    uint8_t eventId = 99;
    LL_logEvent(DUMMY, eventId, DUMMY, DUMMY);
    LL_logEvent(DUMMY, eventId + 1, DUMMY, DUMMY);
    BYTES_EQUAL(eventId + 1, DB_MOCK_getRecord(1).eventId);
}


TEST(writeLog,
     logEvent_writesAllFieldsToRecord) {
    uint32_t timeStamp = 0x12345678;
    uint8_t eventId = 0x90;
    uint8_t value8b = 0xAB;
    uint16_t value16b = 0xCDEF;
    LL_logEvent(timeStamp, eventId, value8b, value16b);
    uint8_t recordId = 0;
    LONGS_EQUAL(timeStamp, DB_MOCK_getRecord(recordId).timeStamp);
    BYTES_EQUAL(eventId, DB_MOCK_getRecord(recordId).eventId);
    BYTES_EQUAL(value8b, DB_MOCK_getRecord(recordId).value8b);
    LONGS_EQUAL(value16b, DB_MOCK_getRecord(recordId).value16b);
}

/*
 *
 logEventClearsNextRecord
 logEvent_onFullLog_preservesEmptyMarker
 logEventRollsOverRecordId_whenAtLastRecord
 */

