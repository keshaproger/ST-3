// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

using ::testing::_;
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

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor* door;
    int timeout = 1000;

    void SetUp() override {
        door = new TimedDoor(timeout);
    }

    void TearDown() override {
        delete door;
    }
};

TEST_F(TimedDoorTest, DoorStartsLocked) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    testing::Mock::AllowLeak(door->adapter); // Игнорировать утечки моков
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    // Убрать вызовы, которые могут вызвать исключение
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, TimeoutThrowsWhenOpen) {
    door->unlock();
    door->triggerTimeout();
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, NoThrowWhenClosed) {
    EXPECT_NO_THROW(door->throwState());
}

TEST(TimerTest, RegistersClient) {
    MockTimerClient client;
    Timer timer;
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(10, &client);
}

TEST(DoorTimerAdapterTest, CallsTimeout) {
    TimedDoor door(1000);
    door.unlock();
    EXPECT_THROW(door.triggerTimeout(), std::runtime_error);
}

TEST_F(TimedDoorTest, TimerRegistrationOnUnlock) {
    MockTimerClient client;
    Timer timer;
    EXPECT_CALL(client, Timeout()).Times(1);
    timer.tregister(1000, &client);
}

TEST_F(TimedDoorTest, ExceptionAfterTimeout) {
    door->unlock();
    EXPECT_THROW(door->adapter->Timeout(), std::runtime_error); // Проверка через адаптер
}

TEST(MockDoorTest, VerifyLockUnlock) {
    MockDoor door;
    EXPECT_CALL(door, unlock()).Times(1);
    EXPECT_CALL(door, lock()).Times(1);
    
    door.unlock();
    door.lock();
}
