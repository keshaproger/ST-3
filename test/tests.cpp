// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::StrictMock;
using ::testing::Exactly;

class MockTimer : public Timer {
 public:
    MOCK_METHOD(void, tregister, (int, TimerClient*), (override));
};

TEST(TimedDoorTest, UnlockStartsTimer) {
    StrictMock<MockTimer> mockTimer;
    TimedDoor door(5, &mockTimer);
    
    EXPECT_CALL(mockTimer, tregister(5, door.adapter))
        .Times(Exactly(1));
    
    door.unlock();
}

TEST(TimedDoorTest, TimeoutThrowsIfDoorOpen) {
    TimedDoor door(2);
    door.unlock();
    
    EXPECT_THROW(door.adapter->Timeout(), std::runtime_error);
}

TEST(TimedDoorTest, NoExceptionIfDoorClosed) {
    TimedDoor door(2);
    door.unlock();
    door.lock();
    
    EXPECT_NO_THROW(door.adapter->Timeout());
}

TEST(TimedDoorTest, LockClosesDoor) {
    TimedDoor door(5);
    door.unlock();
    door.lock();
    EXPECT_FALSE(door.isDoorOpened());
}

TEST(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    TimedDoor door(10);
    EXPECT_EQ(door.getTimeOut(), 10);
}
