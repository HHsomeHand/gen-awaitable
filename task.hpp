//
// Created by hwh on 2026/5/10.
//

#ifndef N01_GEN_AWAITABLE_TASK_HPP
#define N01_GEN_AWAITABLE_TASK_HPP
#include <coroutine>
#include <exception>
#include <future>
#include <utility>

inline std::atomic<int> g_task_count{0};

void task_loop();
void add_task(const std::coroutine_handle<>& task);


template<typename ValueType>
struct task_promise;

template<typename T>
class task
{
public:
    using promise_type = task_promise<T>;

    using handle_type = std::coroutine_handle<promise_type>;

private:
    handle_type m_handle;

public:
    task(handle_type handle) : m_handle(handle) {}

    bool await_ready() noexcept
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> h) noexcept
    {
        add_task(m_handle);

        m_handle.promise().continuation = h;
    }

    T await_resume() noexcept
    {
        T result = std::move(m_handle.promise().m_result_value);

        m_handle.destroy();

        --g_task_count;

        return result;
    }

    auto handler() const
    {
        return m_handle;
    }
};

template<typename Derived>
struct promise_basic
{
    std::coroutine_handle<> continuation = nullptr;

    using promise_type = Derived;

    using handle_type = std::coroutine_handle<Derived>;

    std::suspend_always initial_suspend() noexcept
    {
        g_task_count++;

        return {};
    }

    auto final_suspend() noexcept
    {
        struct awaiter
        {
            bool await_ready() noexcept
            {
                return false;
            }

            std::coroutine_handle<> await_suspend(handle_type h) noexcept
            {
                auto continuation = h.promise().continuation;

                if (continuation)
                {
                    return continuation;
                }

                h.destroy();
                --g_task_count;

                return std::noop_coroutine();
            }

            void await_resume() noexcept
            {
                return;
            }
        };
        return awaiter{};
    }

    void unhandled_exception()
    {
        std::rethrow_exception(std::current_exception());
    }
};

template<typename T>
struct task_promise: promise_basic<task_promise<T>>
{
    using promise_type = task_promise<T>;
    using handle_type = std::coroutine_handle<promise_type>;

    task<T> get_return_object() noexcept
    {
        return task<T>(handle_type::from_promise(
            static_cast<promise_type&>(*this)
        ));
    }

    T m_result_value;

    void return_value(T value) noexcept
    {
        m_result_value = std::move(value);
    }
};

template<>
struct task_promise<void> : promise_basic<task_promise<void>>
{
    using promise_type = task_promise<void>;
    using handle_type = std::coroutine_handle<promise_type>;

    task<void> get_return_object() noexcept
    {
        return task<void>(handle_type::from_promise(
            static_cast<promise_type&>(*this)
        ));
    }

    void return_void() noexcept
    {

    }
};

template<typename T>
void co_spawn(const task<T>& task) noexcept
{
    add_task(task.handler());
}

#endif //N01_GEN_AWAITABLE_TASK_HPP
