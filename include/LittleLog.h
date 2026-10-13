/* LittleLog.h
 *
 *
 * opening (and closing) the database is the responsibility of the caller
 */


#ifndef LITTLE_LOG_H
#define LITTLE_LOG_H

#include <stdint.h>



void LL_init(const uint8_t logTableId);

void LL_logEvent(const uint32_t timeStamp,
                 const uint8_t eventId,
                 const uint8_t value8b,
                 const uint16_t value16b);


#endif
