// LittleLog.cpp

#include "CppUTest/TestHarness.h"

extern "C" {
    #include "LittleLog.h"
}


/*
 */


TEST_GROUP(LittleLog) {
    void setup() {
    }

    void teardown() {
    }
};


TEST(LittleLog,
     initialTest) {
    FAIL("write a test");;
}
