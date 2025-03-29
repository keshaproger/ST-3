// Copyright 2021 GHA Test Team
#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <stdexcept>
#include <thread>
#include <chrono>

class TimerClient {
 public:
  virtual void Timeout() = 0;
};

class Door {
 public:
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool isDoorOpened() = 0;
};

class TimedDoor;

class DoorTimerAdapter : public TimerClient {
 private:
  TimedDoor& door;
 public:
  explicit DoorTimerAdapter(TimedDoor&);
  void Timeout() override;
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter * adapter;
  int iTimeout;
  bool isOpened;
 public:
  explicit TimedDoor(int);
  ~TimedDoor();
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int  getTimeOut() const;
  void throwState();
};

class Timer {
  TimerClient *client;
  void sleep(int);
 public:
  void tregister(int, TimerClient*);
};

#endif  // INCLUDE_TIMEDDOOR_H_
