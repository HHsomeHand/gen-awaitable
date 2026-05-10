
#include <format>
#include <iostream>

#include <Windows.h>

#include "generator.hpp"
#include "task.hpp"
#include "timer.hpp"

generator<int> gen()
{
    co_yield 1;
    co_yield 2;
    co_yield 3;
    co_yield 4;

    co_return 99;
}

void test_gen()
{
    auto g = gen();

    while (true)
    {
        auto [value, bIsDone] = g.next();

        std::cout << std::format("gen value: {}", value) << std::endl;

        if (bIsDone)
        {
            break;
        }
    } // End while (true)
}

task<int> task_child(int id);

task<void> task_parent(int id)
{
    std::cout << id << "task_parent: start" << std::endl;

    int result = co_await task_child(id);

    std::cout << id << "task_parent: 获取了网络请求的值为:" << result << std::endl;

    co_return;
}

task<int> task_child(int id)
{
    std::cout << id << "task_child: 等待网络请求中" << std::endl;

    co_await sleep_for(2s);

    std::cout << id << "task_child: 完成" << std::endl;

    co_return 100;
}

int main()
{
    // 设置控制台输出为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    // 设置控制台输入为 UTF-8（可选，需要输入中文时加）
    SetConsoleCP(CP_UTF8);

    for (int i = 0; i < 5; i++)
    {
         co_spawn(task_parent(i));
    }

    // task_parent();

    task_loop();
}