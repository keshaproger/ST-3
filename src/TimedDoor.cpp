// Copyright 2024 SecureDoor Team

#include "TimedDoor.h"
#include <chrono>
#include <thread>
#include <iostream>

TimerAdapter::TimerAdapter(TimedDoor& door) : doorRef(door) {}

void TimerAdapter::OnTimeout() {
    if (doorRef.IsOpen()) {
        doorRef.CheckState();
    }
}

TimedDoor::TimedDoor(int timeoutSec) : isOpen(false), timeout(timeoutSec) {
    if (timeoutSec <= 0) {
        throw std::invalid_argument("Timeout must be positive");
    }
    adapter = new TimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
    delete adapter;
}

void TimedDoor::Open() {
    if (!isOpen) {
        isOpen = true;
        Timer timer;
        timer.SetTimer(timeout, adapter);
    }
}

void TimedDoor::Close() {
    isOpen = false;
}

bool TimedDoor::IsOpen() const {
    return isOpen;
}

void TimedDoor::CheckState() {
    if (isOpen) {
        throw std::logic_error("Door remained open beyond allowed time");
    }
}

void Timer::SetTimer(int duration, TimerClient* client) {
    std::thread([duration, client]() {
        if (duration > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(duration));
        }
        if (client) {
            try {
                client->OnTimeout();
            } catch (const std::exception& e) {
                std::cerr << "Timer error: " << e.what() << '\n';
            }
        }
    }).detach();
}
