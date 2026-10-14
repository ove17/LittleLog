/* LittleLog.h
 *
 *
 * Opening (and closing) the database is the responsibility of the caller
 *
 * db record schema MUST be:
 *  column  type        description
 *  0       uint16_t    MSW of uint32_t timeStamp
 *  1       uint16_t    LSW of uint32_t timeStamp
 *  2       uint8_t     logEventId, 0 and 0xFF are reserved, other values user defined
 *  3       uint8_t     8 bits value for general use
 *  4       uint16_t    16 bits value for general use
 */


#ifndef LITTLE_LOG_H
#define LITTLE_LOG_H

#include <stdint.h>
#include <stdbool.h>


/*
 * The following logEventId values are for internal use only  and must not be used
 *  in an LL_logEvent() call.
 */
enum {
    LL_EVENT_BOOT = 0,
    LL_EVENT_EMPTY = 0xFF
};

/*
 * Callback used to obtain a timestamp.
 * The returned timestamp must be a uint32_t.
 */
typedef uint32_t (*LL_getTimeStampFn)(void);

/*
 * Callback used to write a record to the database.
 * The caller must make sure the database is open and must make sure this
 *  function writes to the correct table with the correct schema
 * The *data array must contain (at least) 5 items.
 * This function returns false if the provided data does not conform to the
 *  dbase schema.
 */
typedef bool (*LL_writeLogRecordFn)(const uint8_t recordId,
                                    const uint16_t *data);

/*
 * Callback used to read the logEventId from a log record in the dbase.
 * The caller must make sure the database is open and must make sure this
 *  function writes to the correct column in the correct table with the correct
 *  schema.
 */
typedef uint8_t (*LL_readLogEventIdFn)(const uint8_t recordId);

/*
 * The log must have more than 1 record to function.
 * The callback functions are described above.
 */
void LL_initLog(const uint8_t logSize,
                LL_getTimeStampFn getTimeStampFn,
                LL_writeLogRecordFn writeLogRecordFn,
                LL_readLogEventIdFn readLogEventIdFn);

/*
 * Closes the log by resetting all static variables.
 * (only?) intended for testing
 */
void LL_closeLog(void);

/*
 * Writes a log event to the database. The eventId may be user defined other
 *  than the reserved values.
 */
void LL_logEvent(const uint8_t eventId,
                 const uint8_t value8b,
                 const uint16_t value16b);

/*
 * Writes a boot event to the database.
 */
void LL_logBootEvent(const uint8_t fwVersionMajor,
                     const uint16_t fwVersionMinor);

/*
 * Clears the log by writing LL_EVENT_EMPTY to all eventId's
 */
void LL_clearLog(void);

#endif
