// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>
#include <memory>

// DoorTimerAdapter implementation
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& timedDoor) : door(timedDoor) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

// TimedDoor implementation
TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    if(timeout <= 0) throw std::invalid_argument("Timeout must be positive");
    adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
    delete adapter;
}

void TimedDoor::unlock() {
    isOpened = true;
    Timer timer;
    timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
    isOpened = false;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::throwState() {
    if(isOpened) throw std::runtime_error("Door open timeout exceeded");
}

// Timer implementation
void Timer::tregister(int seconds, TimerClient* client) {
    std::thread([seconds, client]() {
        if(seconds > 0) std::this_thread::sleep_for(std::chrono::seconds(seconds));
        if(client) client->Timeout();
    }).detach();
}
