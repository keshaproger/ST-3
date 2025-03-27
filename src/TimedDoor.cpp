// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <thread>
#include <stdexcept>
#include <chrono>

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int timeout, Timer* extTimer) 
    : iTimeout(timeout), isOpened(false), timer(extTimer ? extTimer : new Timer()) {
    adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
    delete adapter;
    if (!dynamic_cast<Timer*>(timer)) delete timer;
}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    timer->tregister(iTimeout, adapter);
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

void Timer::tregister(int timeout, TimerClient* client) {
    std::thread([client, timeout]() {
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        client->Timeout();
    }).detach();
}
