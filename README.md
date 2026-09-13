# LittleLog project summary

LittleLog is a simple logging library implemented as a ring-buffer, intended for embedded use.

## Concept

- LittleLog only provides a function for writing to a log
- LittleLog uses an external mechanism/API for accessing log memory
- Reading LittleLog entries is the responsibility of the caller
- LittleLog has no dependencies, other than injected callbacks for memory access

So the caller must:
- provide a mechanism for reading, presenting, downloading, etc. the log
- provide the callback function to write a record
- provide the callback function to tell time, i.e. millis()

The caller may also:
- #include "logEventIds.h" or the like, which may hold an enum of human readable log event id's

## Data format

A LittleLog record has the following format:
- timestamp		uint32_t	miliseconds
- logEventId	uint8_t
- data1			uint8_t
- data2			uint8_t
- data3			uint16_t

## Intended use

The caller may have UI functions for accessing the log, e.g.:
- access special logging menu via magic key in settings menu, for:
	- displaying data
	- provide clearBuffer
	- writing log to SD
	- show data from record X
	- activating one log id
	
Where:
- viewing/saving has nothing to do with LittleLog, it is all caller / app functionality
	- so is writeToSd
- The header of the loag may show logRecordId (count)
- The UI can only read/show logs in the order that they are stored

## Possible UI display format

>tttttt cc dddeeffff

where:
- t = timestamp in s, dus max. 999,999 ms ~ 16 min (en rolls over)
- cc = log code (HEX)
- ddd = data byte 1 (as dec)
- ee = data byte 2 (as hex)
- ffff = data word 3 (as hex)

## Example

ID		data1	data2	data3		meaning
------------------------------------------------
0x00	version	VERSION	bootNo		boot
0x01	any		any		any			misc/temp use
0x02	max							accessory buffer high water mark
...
0xFF								EMPTY

## Architecture

Primary functions:
- initLog(tableId, maxNumRecords, VERSION, version, *getTimeFn, *writeRecordFn);
	- findsLastBootEvent and increases bootNo by 1
	- sets static logRecordId
	- logs bootEvent
- logEvent(eventId, data1, data2, data3);
	- writes log record to db
	- updates logRecordId
- clearLog();
	- copies latest bootEvent to position 0
		- i.e. sets logRecordId to 1
	- sets all other logEventIds to 0xFF

How to keep track of logRecordId between power cycles?
- logRecordId may be 1 byte in the settings (i.e external to LittleLog)
	- is that a good idea? should it be integrated functionality?
	- can the lib be strictly separated then?

(Caller:)
- it should be invisible to the general user
	- How to hide log?
	- How to make it visible? Key combo? At certain location?
	- Is this related to the logRecordId (?)

=====

#       heading
##      subheading
-       bullet
1.      numbered list
- [ ]   checkbox
>       quote
**x**   bold
*x*     italic
`x`     inline code
```      code block
