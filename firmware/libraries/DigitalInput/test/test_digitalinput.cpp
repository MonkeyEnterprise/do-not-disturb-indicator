#include <Arduino.h>
#include <unity.h>
#include <DigitalInput.h>

// Only mock in native environment
#ifdef ARDUINO_ARCH_NATIVE
static bool simulatedPin = false;
static uint32_t fakeMillis = 0;
int digitalRead(uint8_t pin) { return simulatedPin ? HIGH : LOW; }
unsigned long millis() { return fakeMillis; }
#endif

#if defined(ARDUINO_ARCH_NATIVE)
DigitalInput input(1, 10, false, false);
#else
DigitalInput input(2, 10, true, false); // Use a real pin for hardware test
#endif

void setUp(void)
{
#if defined(ARDUINO_ARCH_NATIVE)
    simulatedPin = false;
    fakeMillis = 0;
    input = DigitalInput(1, 10, false, false);
#endif
}

void test_debounce_and_edges(void)
{
#if defined(ARDUINO_ARCH_NATIVE)
    // Initial state INACTIVE
    input.update();
    TEST_ASSERT_FALSE(input.read());

    // Simulate activation
    simulatedPin = true;
    input.update();
    fakeMillis += 5;
    input.update();
    TEST_ASSERT_FALSE(input.read()); // Not debounced yet

    fakeMillis += 6;
    input.update();
    TEST_ASSERT_TRUE(input.read());
    TEST_ASSERT_TRUE(input.rose());

    // Simulate deactivation
    simulatedPin = false;
    input.update();
    fakeMillis += 11;
    input.update();
    TEST_ASSERT_FALSE(input.read());
    TEST_ASSERT_TRUE(input.fell());
#else
    // On hardware: just check that update() does not crash
    input.update();
    TEST_ASSERT_TRUE(true);
#endif
}

void test_active_duration(void)
{
#if defined(ARDUINO_ARCH_NATIVE)
    // Activate input
    simulatedPin = true;
    input.update();
    fakeMillis += 11;
    input.update();
    TEST_ASSERT_TRUE(input.read());
    TEST_ASSERT_TRUE(input.rose());

    // Hold for 50 ms
    fakeMillis += 50;
    input.update();
    TEST_ASSERT_EQUAL_UINT32(50, input.activeDuration());

    // Deactivate input
    simulatedPin = false;
    input.update();
    fakeMillis += 11;
    input.update();
    TEST_ASSERT_FALSE(input.read());
    TEST_ASSERT_TRUE(input.fell());
    TEST_ASSERT_EQUAL_UINT32(0, input.activeDuration());
#else
    input.update();
    TEST_ASSERT_TRUE(true);
#endif
}

void test_invert_logic(void)
{
#if defined(ARDUINO_ARCH_NATIVE)
    DigitalInput invInput(1, 10, false, true);

    // Simulate pin LOW (should be ACTIVE due to invert)
    simulatedPin = false;
    invInput.update();
    TEST_ASSERT_TRUE(invInput.read());

    // Simulate pin HIGH (should be INACTIVE due to invert)
    simulatedPin = true;
    invInput.update();
    TEST_ASSERT_FALSE(invInput.read());
#else
    DigitalInput invInput(2, 10, true, true);
    invInput.update();
    TEST_ASSERT_TRUE(true);
#endif
}

void setup()
{
    UNITY_BEGIN();
    RUN_TEST(test_debounce_and_edges);
    RUN_TEST(test_active_duration);
    RUN_TEST(test_invert_logic);
    UNITY_END();
}

void loop() {}