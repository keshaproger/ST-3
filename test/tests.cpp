// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::AtLeast;
using ::testing::Return;

class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimedDoorTest, DoorOpensAndCloses) {
    TimedDoor door(3);
    EXPECT_FALSE(door.isDoorOpened());
    door.unlock();
    EXPECT_TRUE(door.isDoorOpened());
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, ThrowsExceptionWhenLeftOpen) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, NoExceptionWhenClosed) {
    TimedDoor door(1);
    door.unlock();
    door.lock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_NO_THROW(door.throwState());
}

TEST(TimerTest, CallsTimeoutAfterDelay) {
    MockTimerClient client;
    Timer timer;
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(1, &client);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

TEST(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    TimedDoor door(5);
    EXPECT_EQ(door.getTimeOut(), 5);
}

TEST(TimedDoorTest, MultipleDoorInstances) {
    TimedDoor door1(3);
    TimedDoor door2(5);
    door1.unlock();
    door2.unlock();
    EXPECT_TRUE(door1.isDoorOpened());
    EXPECT_TRUE(door2.isDoorOpened());
}

TEST(TimedDoorTest, LockingAfterUnlockPreventsException) {
    TimedDoor door(3);
    door.unlock();
    door.lock();
    EXPECT_NO_THROW(door.throwState());
}

TEST(TimedDoorTest, AdapterTimeoutInvokesThrowState) {
    TimedDoor door(1);
    DoorTimerAdapter adapter(door);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimedDoorTest, UnlockAndTimeoutThrows) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorTest, TimerDoesNotThrowIfDoorClosedInTime) {
    TimedDoor door(1);
    door.unlock();
    door.lock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_NO_THROW(door.throwState());
}
