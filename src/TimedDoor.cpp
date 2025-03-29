// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <chrono>
#include <thread>
#include <memory>
#include <iostream>

TimerHandler::TimerHandler(TimedSmartDoor& door) : linked_door(door) {}

void TimerHandler::OnTimerExpired() {
  if (linked_door.CheckStatus()) {
    linked_door.TriggerSecurityCheck();
  }
}

TimedSmartDoor::TimedSmartDoor(unsigned int duration) 
  : door_open(false), timeout_duration(duration) {
  if (duration == 0) throw std::invalid_argument("Invalid timer configuration");
  timer_handler = new TimerHandler(*this);
}

TimedSmartDoor::~TimedSmartDoor() {
  delete timer_handler;
}

void TimedSmartDoor::Release() {
  door_open = true;
  auto scheduler = std::make_unique<TimerScheduler>();
  scheduler->ScheduleTimer(timeout_duration, timer_handler);
}

void TimedSmartDoor::Secure() {
  door_open = false;
}

bool TimedSmartDoor::CheckStatus() const {
  return door_open;
}

void TimedSmartDoor::TriggerSecurityCheck() {
  if (door_open) {
    door_open = false;
    throw std::logic_error("Security breach detected!");
  }
}

void TimerScheduler::ScheduleTimer(unsigned int delay, TimerSubscriber* sub) {
  std::thread([delay, sub]() {
    if (delay > 0) {
      std::this_thread::sleep_for(std::chrono::seconds(delay));
    }
    
    if (sub) {
      try {
        sub->OnTimerExpired();
      } catch (const std::exception& e) {
        std::cerr << "Timer error: " << e.what() << '\n';
      }
    }
  }).detach();
}
