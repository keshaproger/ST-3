// Copyright 2021 GHA Test Team
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::AtLeast;

class MockTimerSubscriber : public TimerSubscriber {
 public:
  MOCK_METHOD(void, OnTimerExpired, (), (override));
};

class SecurityTest : public ::testing::Test {
 protected:
  void SetUp() override { door = new TimedSmartDoor(3); }
  void TearDown() override { delete door; }
  
  TimedSmartDoor* door;
};

TEST_F(SecurityTest, DefaultSecurityState) {
  EXPECT_FALSE(door->CheckStatus());
  EXPECT_EQ(door->GetDuration(), 3);
}

TEST_F(SecurityTest, DoorActivationSequence) {
  door->Secure();
  door->Release();
  EXPECT_TRUE(door->CheckStatus());
}

TEST_F(SecurityTest, EmergencyLockProcedure) {
  door->Release();
  door->Secure();
  EXPECT_FALSE(door->CheckStatus());
}

TEST_F(SecurityTest, SecurityBreachScenario) {
  door->Release();
  EXPECT_THROW(door->TriggerSecurityCheck(), std::logic_error);
}

TEST_F(SecurityTest, ConsecutiveAccessAttempts) {
  door->Release();
  door->Release();
  EXPECT_TRUE(door->CheckStatus());
}

TEST_F(SecurityTest, InvalidTimerConfiguration) {
  EXPECT_THROW(TimedSmartDoor invalid_door(0), std::invalid_argument);
}

TEST_F(SecurityTest, PostLockInspection) {
  door->Secure();
  EXPECT_NO_THROW(door->TriggerSecurityCheck());
}

TEST(TimerIntegrationTest, SubscriberNotification) {
  MockTimerSubscriber mock_sub;
  TimerScheduler scheduler;

  EXPECT_CALL(mock_sub, OnTimerExpired()).Times(1);
  scheduler.ScheduleTimer(0, &mock_sub);
}

TEST(HandlerTest, ClosedDoorNoAlert) {
  TimedSmartDoor test_door(2);
  test_door.Secure();
  EXPECT_NO_THROW(test_door.TriggerSecurityCheck());
}

TEST(HandlerTest, ImmediateTimeoutResponse) {
  TimedSmartDoor test_door(1);
  test_door.Release();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_THROW(test_door.TriggerSecurityCheck(), std::logic_error);
}
