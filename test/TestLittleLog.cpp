// LittleLog.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "LittleLog.h"
    #include "mocks/dbMock.h"
    #include "mocks/timeMock.h"
}


#define DUMMY 99    // any byte not 0 or 0xFF
#define LOG_SIZE 10


static void fillLogWithDummyRecords(const uint8_t numRecords) {
    for (uint8_t i = 0; i < numRecords; i++) {
        LL_logEvent(DUMMY, DUMMY, DUMMY);
    }
}


TEST_GROUP(testMocks) {
    void setup() {
        DB_MOCK_init();
        LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
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
    DB_setRecord(recordId, data);
    LONGS_EQUAL(0x00010002, DB_MOCK_getRecord(recordId).timeStamp);
    BYTES_EQUAL(0x03, DB_MOCK_getRecord(recordId).eventId);
    BYTES_EQUAL(0x04, DB_MOCK_getRecord(recordId).value8b);
    LONGS_EQUAL(0x0005, DB_MOCK_getRecord(recordId).value16b);
}


///////


TEST_GROUP(writeLog) {
    void setup() {
        DB_MOCK_init();
        LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    }

    void teardown() {
    }
};


TEST(writeLog,
     logEvent_onEmptyTable_writesRecord0) {
    uint8_t eventId = 99;
    LL_logEvent(eventId, DUMMY, DUMMY);
    BYTES_EQUAL(eventId, DB_MOCK_getRecord(0).eventId);
}


TEST(writeLog,
     secondLogEvent_onEmptyTable_writesRecord1) {
    uint8_t eventId = 99;
    LL_logEvent(eventId, DUMMY, DUMMY);
    LL_logEvent(eventId + 1, DUMMY, DUMMY);
    BYTES_EQUAL(eventId + 1, DB_MOCK_getRecord(1).eventId);
}


TEST(writeLog,
     logEvent_writesAllFieldsToRecord) {
    uint8_t eventId = 0x90;
    uint8_t value8b = 0xAB;
    uint16_t value16b = 0xCDEF;
    LL_logEvent(eventId, value8b, value16b);
    uint8_t recordId = 0;
    BYTES_EQUAL(eventId, DB_MOCK_getRecord(recordId).eventId);
    BYTES_EQUAL(value8b, DB_MOCK_getRecord(recordId).value8b);
    LONGS_EQUAL(value16b, DB_MOCK_getRecord(recordId).value16b);
}


TEST(writeLog,
     logEventClearsNextRecord) {
    uint16_t data[5] = {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY};
    DB_setRecord(1, data);
    BYTES_EQUAL(DUMMY, DB_MOCK_getRecord(1).eventId);
    LL_logEvent(DUMMY, DUMMY, DUMMY);  // is written to record 0
    BYTES_EQUAL(LL_EVENT_EMPTY, DB_MOCK_getRecord(1).eventId); // and record 1 is empty
}


TEST(writeLog,
     logEventOnLastRecord_clearsFirstRecord) {
    fillLogWithDummyRecords(LOG_SIZE - 1);
    BYTES_EQUAL(DUMMY, DB_MOCK_getRecord(0).eventId); // 1st record is non-empty
    BYTES_EQUAL(LL_EVENT_EMPTY, DB_MOCK_getRecord(LOG_SIZE - 1).eventId); // last record is empty
    LL_logEvent(12, DUMMY, DUMMY);  // is written to last record
//    DB_MOCK_dump(LOG_SIZE);
    BYTES_EQUAL(12, DB_MOCK_getRecord(LOG_SIZE - 1).eventId); // last record is non-empty
    BYTES_EQUAL(LL_EVENT_EMPTY, DB_MOCK_getRecord(0).eventId); // first record is empty
}


TEST(writeLog,
     afterLogEventOnLastRecord_logEventWritesFirstRecord) {
    fillLogWithDummyRecords(LOG_SIZE);
    BYTES_EQUAL(DUMMY, DB_MOCK_getRecord(LOG_SIZE - 1).eventId); // last record is non-empty
    BYTES_EQUAL(LL_EVENT_EMPTY, DB_MOCK_getRecord(0).eventId); // first record is empty
    LL_logEvent(21, DUMMY, DUMMY);  // is written to first record
    //    DB_MOCK_dump(LOG_SIZE);
    BYTES_EQUAL(21, DB_MOCK_getRecord(0).eventId); // 1st record is non-empty
    BYTES_EQUAL(LL_EVENT_EMPTY, DB_MOCK_getRecord(1).eventId); // 2nd record is empty
}


TEST(writeLog,
     clearLog_setsAllEventIdsToEVENT_EMPTY) {
    fillLogWithDummyRecords(LOG_SIZE);
    LL_clearLog();
    for (uint8_t i = 0; i < LOG_SIZE; i++) {
        BYTES_EQUAL(LL_EVENT_EMPTY, DB_MOCK_getRecord(i).eventId);
    }
}


TEST(writeLog,
     afterClearLog_logEventWritesToRecord0) {
    LL_clearLog();
    LL_logEvent(DUMMY, DUMMY, DUMMY);
    BYTES_EQUAL(DUMMY, DB_MOCK_getRecord(0).eventId);
}


TEST(writeLog,
     bootEventContainsVersion) {
    const uint8_t versionMajor = 12;
    const uint8_t versionMinor = 34;
    LL_logBootEvent(versionMajor, versionMinor);
    BYTES_EQUAL(versionMajor, DB_MOCK_getRecord(0).value8b);
    BYTES_EQUAL(versionMinor, DB_MOCK_getRecord(0).value16b);
}


TEST(writeLog,
     bootEventHasTimeStamp) {
    uint32_t timeStamp = 0x012345;
    TIME_MOCK_setTimeStamp(timeStamp);
    LL_logBootEvent(0, 0);
    BYTES_EQUAL(timeStamp, DB_MOCK_getRecord(0).timeStamp);
}


//////


TEST_GROUP(initLog) {
    void setup() {
        DB_MOCK_init();
    }

    void teardown() {
    }
};


TEST(initLog,
     logEventOnEmptylogWith2records_writesToRecord0) {
    LL_initLog(2, TM_getTimeStamp, DB_setRecord, DB_getValue);
    LL_logEvent(DUMMY, DUMMY, DUMMY);
    BYTES_EQUAL(DUMMY, DB_MOCK_getRecord(0).eventId);
}


TEST(initLog,
     secondLogEventOnlogWith2records_writesToRecord1AndClearsRecord0) {
    LL_initLog(2, TM_getTimeStamp, DB_setRecord, DB_getValue);
    LL_logEvent(DUMMY, DUMMY, DUMMY);
    LL_logEvent(DUMMY, DUMMY, DUMMY);
    BYTES_EQUAL(DUMMY, DB_MOCK_getRecord(1).eventId);
    BYTES_EQUAL(LL_EVENT_EMPTY, DB_MOCK_getRecord(0).eventId);
}


TEST(initLog,
     logEvent_writesTimeStampToRecord) {
    LL_initLog(2, TM_getTimeStamp, DB_setRecord, DB_getValue);
    uint32_t timeStamp = 0x12345678;
    TIME_MOCK_setTimeStamp(timeStamp);
    LL_logEvent(DUMMY, DUMMY, DUMMY);
    uint8_t recordId = 0;
    LONGS_EQUAL(timeStamp, DB_MOCK_getRecord(recordId).timeStamp);
}


TEST(initLog,
     afterReboot_logsStillExist) {
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    LL_logEvent(DUMMY, DUMMY, DUMMY);
    LL_closeLog();
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    BYTES_EQUAL(DUMMY, DB_MOCK_getRecord(0).eventId);
}


TEST(initLog,
     afterReboot_logEventContinuesAtNextRecord) {
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    LL_logEvent(1, DUMMY, DUMMY);
    LL_closeLog();
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    LL_logEvent(2, DUMMY, DUMMY);
    BYTES_EQUAL(1, DB_MOCK_getRecord(0).eventId);
    BYTES_EQUAL(2, DB_MOCK_getRecord(1).eventId);
}


TEST(initLog,
     logEventAfterReboot_withOnlyFirstRecordEmpty_writesToFirstRecord) {
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    fillLogWithDummyRecords(LOG_SIZE);
    LL_closeLog();
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    LL_logEvent(20, DUMMY, DUMMY);
    BYTES_EQUAL(20, DB_MOCK_getRecord(0).eventId);
}


TEST(initLog,
     logEventAfterReboot_withOnlyLastRecordEmpty_writesToLastRecord) {
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    fillLogWithDummyRecords(LOG_SIZE - 1);
    LL_closeLog();
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    LL_logEvent(30, DUMMY, DUMMY);
    BYTES_EQUAL(30, DB_MOCK_getRecord(LOG_SIZE - 1).eventId);
}


TEST(initLog,
     logEventAfterReboot_withPartiallyFilledRecords_writesToFirstEmptyRecord) {
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    fillLogWithDummyRecords(LOG_SIZE - 5);
    LL_closeLog();
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    LL_logEvent(40, DUMMY, DUMMY);
    BYTES_EQUAL(40, DB_MOCK_getRecord(LOG_SIZE - 5).eventId);
}


// should be impossible, but anyway...
TEST(initLog,
     logEventAfterReboot_withCompletelyFullRecords_writesToFirstEmptyRecord) {
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    fillLogWithDummyRecords(LOG_SIZE - 4); // set recordId to != 0
    uint16_t data[5] = {1, 2, 3, 4, 5};
    for (uint8_t i = 0; i < LOG_SIZE; i++) {
        DB_setRecord(i, data); // fills dbase without clearing next record
    }
    LL_closeLog();
    LL_initLog(LOG_SIZE, TM_getTimeStamp, DB_setRecord, DB_getValue);
    LL_logEvent(40, DUMMY, DUMMY);
    BYTES_EQUAL(40, DB_MOCK_getRecord(0).eventId);
}
