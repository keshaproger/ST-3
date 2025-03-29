// Copyright 2024 SecureDoor Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <exception>

class TimerClient {
 public:
    virtual ~TimerClient() = default;
    virtual void Timeout() = 0;  // Возвращаем оригинальное имя
};

class Door {
 public:
    virtual ~Door() = default;
    virtual void lock() = 0;      // Оригинальные имена методов
    virtual void unlock() = 0;
    virtual bool isDoorOpened() const = 0;
};

class TimedDoor;

class DoorTimerAdapter : public TimerClient {
 private:
    TimedDoor& doorRef;
 public:
    explicit DoorTimerAdapter(TimedDoor& door);
    void Timeout() override;
};

class TimedDoor : public Door {
 public:
    explicit TimedDoor(int timeoutSec);
    ~TimedDoor() override;
    void lock() override;         // Оригинальные имена
    void unlock() override;
    bool isDoorOpened() const override;
    void throwState();
    int getTimeout() const { return timeout; }
 private:
    int timeout;
    bool isOpened;
    DoorTimerAdapter* adapter;
    friend class DoorTimerAdapter;
};

class Timer {
 public:
    void tregister(int duration, TimerClient* client);  // Оригинальное имя
 private:
    void sleep(int sec) const;
};

#endif  // INCLUDE_TIMEDDOOR_H_
