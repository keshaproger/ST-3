// Copyright 2021 GHA Test Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <thread>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Throw;

// Mocks for testing
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

class MockTimer : public Timer {
public:
    MOCK_METHOD(void, tregister, (int, TimerClient*), ());
};

// Test Fixtures
class TimedDoorTest : public ::testing::Test {
protected:
    void SetUp() override {
        door = new TimedDoor(5);
    }
    void TearDown() override {
        delete door;
    }
    TimedDoor* door;
};

class AdapterTest : public ::testing::Test {
protected:
    MockDoor mockDoor;
    DoorTimerAdapter adapter{*dynamic_cast<TimedDoor*>(&mockDoor)};
};

// Тесты для TimedDoor
TEST_F(TimedDoorTest, InitialStateIsLocked) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowWhenOpenTimeout) {
    door->unlock();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoThrowWhenClosed) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

// Тесты для адаптера
TEST_F(AdapterTest, TimeoutTriggersWhenOpen) {
    EXPECT_CALL(mockDoor, isDoorOpened())
        .WillOnce(::testing::Return(true));
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(AdapterTest, NoActionWhenClosed) {
    EXPECT_CALL(mockDoor, isDoorOpened())
        .WillOnce(::testing::Return(false));
    EXPECT_NO_THROW(adapter.Timeout());
}

// Тесты для Timer
TEST(TimerTest, RegisterTimeout) {
    MockTimerClient client;
    MockTimer timer;
    
    EXPECT_CALL(client, Timeout()).Times(1);
    EXPECT_CALL(timer, tregister(5, &client));
    
    timer.tregister(5, &client);
}

// Тесты исключений
TEST(TimedDoorExceptionTest, InvalidTimeout) {
    EXPECT_THROW(TimedDoor(-1), std::invalid_argument);
    EXPECT_THROW(TimedDoor(0), std::invalid_argument);
}

TEST(TimedDoorIntegrationTest, FullWorkflow) {
    TimedDoor door(1);
    door.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_THROW(door.throwState(), std::runtime_error);
}
