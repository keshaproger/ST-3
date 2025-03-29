// Copyright 2024 SecureDoor Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <exception>

class TimerClient {
public:
    virtual ~TimerClient() = default;
    virtual void OnTimeout() = 0;  // Переименован метод
};

class Door {
public:
    virtual ~Door() = default;
    virtual void Close() = 0;      // Изменены названия методов
    virtual void Open() = 0;
    virtual bool IsOpen() const = 0;
};

class TimedDoor; // Предварительное объявление

class TimerAdapter : public TimerClient {  // Изменено имя класса
private:
    TimedDoor& doorRef;
public:
    explicit TimerAdapter(TimedDoor& door);
    void OnTimeout() override;     // Используем новое имя метода
};

class TimedDoor : public Door {
public:
    explicit TimedDoor(int timeoutSec);
    ~TimedDoor() override;
    void Close() override;
    void Open() override;
    bool IsOpen() const override;
    void CheckState();            // Переименован метод
    int GetTimeout() const { return timeout; }
private:
    int timeout;
    bool isOpen;
    TimerAdapter* adapter;
    friend class TimerAdapter;     // Добавлена дружественная связь
};

class Timer {
public:
    void SetTimer(int duration, TimerClient* client);  // Изменено название метода
private:
    void Wait(int sec) const;
};

#endif  // INCLUDE_TIMEDDOOR_H_
