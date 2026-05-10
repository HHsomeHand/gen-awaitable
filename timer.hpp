//
// Created by hwh on 2026/5/10.
//

#ifndef N01_GEN_AWAITABLE_TIMER_HPP
#define N01_GEN_AWAITABLE_TIMER_HPP

#include <thread>
#include <chrono>
#include <coroutine>

#include "task.hpp"

using namespace std::chrono_literals;

class TimerAwaiter
{
public:
    bool await_ready()
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> h)
    {
        auto dur = m_dur;

        std::thread([dur, h]
        {
            std::this_thread::sleep_for(dur);

            // h.resume();

            add_task(h);
        }).detach();
    }

    void await_resume()
    {

    }
private:
    std::chrono::milliseconds m_dur;

public:
    TimerAwaiter(const std::chrono::milliseconds& dur): m_dur(dur) {}

};



TimerAwaiter sleep_for(std::chrono::milliseconds m_dur);

#endif //N01_GEN_AWAITABLE_TIMER_HPP
