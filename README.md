# LittleLog project summary

LittleLog is a simple logging library implemented as a ring-buffer, intended for embedded use.

## Concept

- LittleLog only provides a function for writing to a log
- LittleLog uses an external mechanism/API for accessing log memory
- Reading LittleLog entries is the responsibility of the caller
- LittleLog has no dependencies, other than injected callbacks for timestamp and memory access

So the caller must:
- provide a mechanism for reading, presenting, downloading, etc. the log
- provide the callback function to write a record consisting of 5 uint16_t's
- provide the callback function to read the eventId of a record
- provide the callback function to generate a time stamp, i.e. millis()

The caller may also:
- Keep a "logEventIds.h" or the like, to keep track of human readable logEventId's

## Data format

A LittleLog record has the following format:
- timestamp		uint32_t	miliseconds
- logEventId	uint8_t
- data1			uint8_t
- data2			uint16_t

Where logEventId = 0 and 0xFF are predefined values, see **logEventId** below.

## Potential use

The caller may provide a (hidden) logging menu, where the user could:
- browse/display log data
- clear the log buffer
- export the log

These are examples, none of this is implemented in LittleLog.

### Possible UI display format

Again, this is an example, and not implemented in LittleLog.
```
>ttttttt cc ddd eeee
```

where:
- t = timestamp in ms, so max. visible is 9,999,999 ms ~ 160 min
- cc = logEventId (HEX)
- ddd = data byte 1 (as dec)
- eeee = data byte 2 (as hex)

Note that the timestamp is a uint32_t, so the log may contain higher values. It is up to the caller to deal with that.

## logEventId

The meaning of logEventId's could be:
```
ID	data1	data2	meaning
------------------------------------------------
0x00	version	VERSION	boot
0x01	any	any	misc/temp use
0x02	max		accessory buffer high water mark
...
0xFF			EMPTY
```

Where only logEventId = 0 and 0xFF are reserved values, all others are user defined.
LittleLog accepts any logEventId other than 0 and 0xFF as input. It has no idea of the meaning of a logEventId. Keeping track of that is the responsibility of the caller.

## Architecture

LittleLog only has few public functions:

- initLog
must be called before logging any events.

- closeLog
resets all static variables, mostly (only) useful for testing.

- logEvent
writes a user-defined logEventId and two user defined values to the log, along with a timestamp.

- logBootEvent
writes the bootLogEventId along with minor and major fw version to the log, along with a timestamp.

- clearLog
clears the entire log and restarts at record 0

Note that after every write to the log, the next record is cleared. This is to make sure that it is always clear what the last written record is.
