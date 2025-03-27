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
using ::testing::Invoke;

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
    TimedDoor realDoor{5};
    DoorTimerAdapter adapter{realDoor};
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
    realDoor.unlock();
    EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST_F(AdapterTest, NoActionWhenClosed) {
    realDoor.lock();
    EXPECT_NO_THROW(adapter.Timeout());
}

// Тесты для Timer
TEST(TimerTest, RegisterTimeout) {
    MockTimerClient client;
    MockTimer timer;
    
    EXPECT_CALL(timer, tregister(5, &client))
        .WillOnce(testing::Invoke([](int, TimerClient* tc) {  // Явное указание пространства имён
            if (tc) tc->Timeout();
        }));
    EXPECT_CALL(client, Timeout()).Times(1);
    
    timer.tregister(5, &client);
}

// Тесты исключений
TEST(TimedDoorExceptionTest, InvalidTimeout) {
    EXPECT_THROW(TimedDoor(-1), std::invalid_argument);
    EXPECT_THROW(TimedDoor(0), std::invalid_argument);
}

TEST(TimedDoorIntegrationTest, FullWorkflow) {
    MockTimer mockTimer;
    TimedDoor door(1);
    
    EXPECT_CALL(mockTimer, tregister(1, _))
        .WillOnce(testing::Invoke([](int, TimerClient* tc) {  // Фикс здесь
            if (tc) tc->Timeout();
        }));
    
    door.unlock();
    EXPECT_THROW(door.throwState(), std::runtime_error);
}
