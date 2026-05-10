//
// Created by hwh on 2026/5/10.
//

#include "timer.hpp"

TimerAwaiter sleep_for(std::chrono::milliseconds m_dur)
{
    return TimerAwaiter(m_dur);
}