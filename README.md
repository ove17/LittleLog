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

Where logEventId = 0 and 0xFF are predefined values, see **logEventId** below.

## Potential use

The caller may provide a (hidden) logging menu, where the user could:
- browse/display log data
- clear the log buffer
- export the log

These are examples, none of this is implemented in LittleLog.

### Possible UI display format

```
>tttttt cc dddeeffff
```

where:
- t = timestamp in ms, dus max. 999,999 ms ~ 16 min (and then rolls over)
- cc = logEventId (HEX)
- ddd = data byte 1 (as dec)
- ee = data byte 2 (as hex)
- ffff = data word 3 (as hex)

## logEventId

The meaning of logEventId's could be:
```
ID		data1	data2	data3		meaning
------------------------------------------------
0x00	version	VERSION	bootNo		boot
0x01	any		any		any			misc/temp use
0x02	max							accessory buffer high water mark
...
0xFF								EMPTY
```

Where only logEventId = 0 and 0xFF are predefined values, all others are user defined.
LittleLog accepts any logEventId other than 0 and 0xFF as input. It has no idea
of the meaning of a logEventId. Keeping track of that is the responsibility of 
the caller.

## Architecture

LittleLog only has 3 public functions:

- initLog(tableId, maxNumRecords, VERSION, version, *getTimeFn, *writeRecordFn, *readRecordFn);
	- stores tableId, maxNumRecords, *getTimeFn, *writeRecordFn and *readRecordFn to static vars
	- findsLastBootEvent and increases bootNo by 1
	- sets static currentLogRecordId
	- logs bootEvent at currentLogRecordId
	- increases currentLogRecordId by 1

- logEvent(eventId, data1, data2, data3);
	- writes log record to db
	- increases currentLogRecordId by 1

- clearLog();
	- copies latest bootEvent to position 0
	- sets currentLogRecordId to 1
	- sets all other logEventIds to 0xFF

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
