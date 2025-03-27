// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <thread>
#include <stdexcept>
#include <chrono>

// DoorTimerAdapter
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

// TimedDoor
TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
    adapter = new DoorTimerAdapter(*this);
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    Timer timer;
    timer.tregister(iTimeout, adapter);
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    if (isOpened) {
        throw std::runtime_error("Door is still open after timeout!");
    }
}

// Timer
void Timer::tregister(int timeout, TimerClient* client) {
    this->client = client;
    std::thread([this, timeout]() {
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        client->Timeout();
    }).detach();
}
