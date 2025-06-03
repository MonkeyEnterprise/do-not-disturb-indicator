#include <Arduino.h>
#include <unity.h>
#include <Register.h>

struct TestStruct {
    uint8_t version;
    int value;
    float gain;
};

Register testReg = Register::create<TestStruct>();
const TestStruct defaultStruct = {42, 123, 3.14f};

void setUp(void) {
    // Optional: clear EEPROM for a clean test
    TestStruct blank = {0, 0, 0.0f};
    testReg.save(blank);
}

void tearDown(void) {
    // Not used
}

void test_save_and_load(void) {
    // Save defaultStruct
    TEST_ASSERT_TRUE(testReg.save(defaultStruct));

    // Load it back
    TestStruct loaded;
    bool ok = testReg.load(loaded, defaultStruct);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT8(defaultStruct.version, loaded.version);
    TEST_ASSERT_EQUAL_INT(defaultStruct.value, loaded.value);
    TEST_ASSERT_FLOAT_WITHIN(0.0001, defaultStruct.gain, loaded.gain);
}

void test_crc_fail_returns_default(void) {
    // Save a struct
    testReg.save(defaultStruct);

    // Corrupt EEPROM (simulate CRC fail)
    EEPROM.write(testReg.getAddress() + sizeof(TestStruct), 0x00);

    TestStruct loaded = {0, 0, 0.0f};
    bool ok = testReg.load(loaded, defaultStruct);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL_UINT8(defaultStruct.version, loaded.version);
    TEST_ASSERT_EQUAL_INT(defaultStruct.value, loaded.value);
    TEST_ASSERT_FLOAT_WITHIN(0.0001, defaultStruct.gain, loaded.gain);
}

void test_no_write_if_unchanged(void) {
    // Save once
    TEST_ASSERT_TRUE(testReg.save(defaultStruct));
    // Save again, should not write
    TEST_ASSERT_FALSE(testReg.save(defaultStruct));
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_save_and_load);
    RUN_TEST(test_crc_fail_returns_default);
    RUN_TEST(test_no_write_if_unchanged);
    UNITY_END();
}

void loop() {
    // Not used
}