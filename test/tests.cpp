// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <atomic>
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
    
    std::atomic<bool> done{false};
    std::thread([&door, &done]() {
        door.throwState();
        done = true;
    }).detach();
    
    for (int i = 0; i < 10 && !done; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    EXPECT_TRUE(done);
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
    door.unlock();
    
    Timer timer;
    timer.tregister(1, &adapter);
    
    EXPECT_THROW({
        try {
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                door.throwState();
            }
        } catch (const std::runtime_error& e) {
            EXPECT_STREQ(e.what(), "Door left open!");
            throw;
        }
    }, std::runtime_error);
}

TEST(TimedDoorTest, UnlockAndTimeoutThrows) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_THROW({
        try {
            door.throwState();
        } catch (const std::runtime_error& e) {
            EXPECT_STREQ(e.what(), "Door left open!");
            throw;
        }
    }, std::runtime_error);
}

TEST(TimedDoorTest, TimerDoesNotThrowIfDoorClosedInTime) {
    TimedDoor door(2);
    door.unlock();
    
    std::thread([&door]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        door.lock();
    }).detach();
    
    EXPECT_NO_THROW(door.throwState());
}
