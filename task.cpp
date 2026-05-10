//
// Created by hwh on 2026/5/10.
//

#include "task.hpp"

#include <condition_variable>
#include <queue>

std::condition_variable g_cv;

std::mutex g_queue_mutex;

std::queue<std::coroutine_handle<>> g_task_queue;

void task_loop()
{
    std::queue<std::coroutine_handle<>> task_queue;

    while (true)
    {
        {
            std::unique_lock<std::mutex> mutex(g_queue_mutex);

            g_cv.wait(mutex, []
            {
                return !g_task_queue.empty();
            });

            std::swap(task_queue, g_task_queue);
        }


        while (!task_queue.empty())
        {
            auto& task = task_queue.front();

            task.resume();

            task_queue.pop();
        } // End of while (!task_queue.empty())
    } // END of while (true)
}

void add_task(const std::coroutine_handle<>& task)
{
    std::lock_guard<std::mutex> lock(g_queue_mutex);

    g_task_queue.push(task);

    g_cv.notify_one();
}