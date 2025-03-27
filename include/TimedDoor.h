// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

class DoorTimerAdapter;
class Timer;
class Door;
class TimedDoor;

class TimerClient {
 public:
  virtual void Timeout() = 0;
  virtual ~TimerClient() = default;
};

class Door {
 public:
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool isDoorOpened() = 0;
  virtual ~Door() = default;
};

class DoorTimerAdapter : public TimerClient {
 private:
  TimedDoor& door;
  // Добавлен friend-класс для тестов
  friend class TimedDoorTest_AdapterBoundToCorrectDoor_Test;
 public:
  explicit DoorTimerAdapter(TimedDoor&);
  void Timeout() override;
};

class TimedDoor : public Door {
 private:
  DoorTimerAdapter* adapter;
  Timer* timer;
  int iTimeout;
  bool isOpened;
  // Добавлен friend-класс для тестов
  friend class TimedDoorTest_UnlockStartsTimer_Test;
  friend class TimedDoorTest_TimeoutThrowsIfDoorOpen_Test;
  friend class TimedDoorTest_NoExceptionIfDoorClosed_Test;
  friend class TimedDoorTest_ReopenResetsTimer_Test;
  friend class TimedDoorTest_AdapterBoundToCorrectDoor_Test;
  friend class TimedDoorTest_TimerReceivesCorrectTimeout_Test;
 public:
  explicit TimedDoor(int timeout, Timer* extTimer = nullptr);
  ~TimedDoor();
  bool isDoorOpened() override;
  void unlock() override;
  void lock() override;
  int getTimeOut() const;
  void throwState();
};

class Timer {
 public:
  virtual void tregister(int timeout, TimerClient* client);
  virtual ~Timer() = default;
};

#endif  // INCLUDE_TIMEDDOOR_H_
