// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::AtLeast;
using ::testing::Throw;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

TEST(TimedDoorTest, UnlockStartsTimer) {
    TimedDoor door(5);
    testing::NiceMock<MockTimerClient> client;
    EXPECT_CALL(client, Timeout()).Times(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(6));
}

TEST(TimedDoorTest, TimeoutThrowsIfDoorOpen) {
    TimedDoor door(2);
    door.unlock();
    EXPECT_THROW({
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }, std::runtime_error);
}

TEST(TimedDoorTest, NoExceptionIfDoorClosed) {
    TimedDoor door(2);
    door.unlock();
    door.lock();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    SUCCEED();
}

// Дополнительные тесты (всего 10+)...
