// Copyright 2024 SecureDoor Team

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;

// Мок-классы для тестирования
class MockTimerClient : public TimerClient {
public:
    MOCK_METHOD(void, OnTimeout, (), (override));
};

class MockDoor : public Door {
public:
    MOCK_METHOD(void, Close, (), (override));
    MOCK_METHOD(void, Open, (), (override));
    MOCK_METHOD(bool, IsOpen, (), (const, override));
};

class TimedDoorTest : public ::testing::Test {
protected:
    void SetUp() override {
        door = new TimedDoor(3);
    }
    void TearDown() override {
        delete door;
    }
    TimedDoor* door;
};

TEST_F(TimedDoorTest, InitializationTest) {
    EXPECT_FALSE(door->IsOpen());
    EXPECT_EQ(door->GetTimeout(), 3);
}

TEST_F(TimedDoorTest, OpenDoorStartsTimer) {
    door->Open();
    EXPECT_TRUE(door->IsOpen());
}

TEST_F(TimedDoorTest, CloseDoorResetsState) {
    door->Open();
    door->Close();
    EXPECT_FALSE(door->IsOpen());
}

TEST_F(TimedDoorTest, TimeoutTriggersExceptionWhenOpen) {
    door->Open();
    EXPECT_THROW(door->CheckState(), std::logic_error);
}

TEST_F(TimedDoorTest, NoExceptionWhenClosed) {
    door->Close();
    EXPECT_NO_THROW(door->CheckState());
}

TEST_F(TimedDoorTest, InvalidTimeoutThrowsOnCreation) {
    EXPECT_THROW(TimedDoor invalidDoor(0), std::invalid_argument);
    EXPECT_THROW(TimedDoor invalidDoor(-5), std::invalid_argument);
}

TEST_F(TimedDoorTest, MultipleOpenCallsKeepDoorOpen) {
    door->Open();
    door->Open(); // Повторный вызов
    EXPECT_TRUE(door->IsOpen());
}

TEST(AdapterTest, AdapterCallsCheckState) {
    TimedDoor door(2);
    TimerAdapter adapter(door);
    door.Open();
    EXPECT_THROW(adapter.OnTimeout(), std::logic_error);
}

TEST(TimerTest, TimerCallsClientMethod) {
    MockTimerClient client;
    EXPECT_CALL(client, OnTimeout()).Times(1);
    
    Timer timer;
    timer.SetTimer(0, &client); // Немедленный вызов
}

TEST(TimerIntegrationTest, FullFlowTest) {
    TimedDoor door(1);
    door.Open();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_THROW(door.CheckState(), std::logic_error);
}
