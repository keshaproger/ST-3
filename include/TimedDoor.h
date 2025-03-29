// Copyright 2021 GHA Test Team
#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <stdexcept>

class TimerSubscriber {
 public:
  virtual void OnTimerExpired() = 0;
  virtual ~TimerSubscriber() = default;
};

class SmartDoor {
 public:
  virtual void Secure() = 0;
  virtual void Release() = 0;
  virtual bool CheckStatus() const = 0;
  virtual ~SmartDoor() = default;
};

class TimerHandler;

class TimedSmartDoor : public SmartDoor {
 public:
  explicit TimedSmartDoor(unsigned int duration);
  ~TimedSmartDoor() override;
  void Release() override;
  void Secure() override;
  bool CheckStatus() const override;
  void TriggerSecurityCheck();
  unsigned int GetDuration() const { return timeout_duration; }

 private:
  TimerHandler* timer_handler;
  bool door_open;
  unsigned int timeout_duration;
};

class TimerHandler : public TimerSubscriber {
 public:
  explicit TimerHandler(TimedSmartDoor& door);
  void OnTimerExpired() override;

 private:
  TimedSmartDoor& linked_door;
};

class TimerScheduler {
 public:
  void ScheduleTimer(unsigned int delay, TimerSubscriber* subscriber);
  virtual ~TimerScheduler() = default;
};

#endif  // INCLUDE_TIMEDDOOR_H_
