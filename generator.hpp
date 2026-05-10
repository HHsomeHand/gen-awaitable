//
// Created by hwh on 2026/5/10.
//

#ifndef N01_GEN_AWAITABLE_GENERATOR_HPP
#define N01_GEN_AWAITABLE_GENERATOR_HPP
#include <coroutine>
#include <exception>
#include <tuple>

template <typename T>
class generator
{
public:
    struct promise_type;

    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        T m_current_value;

        generator<T> get_return_object()
        {
            return generator<T>(handle_type::from_promise(*this));
        }

        std::suspend_always initial_suspend()
        {
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        std::suspend_always yield_value(T value)
        {
            m_current_value = value;

            return {};
        }

        // void return_void() {}

        void return_value(T value)
        {
            m_current_value = value;
        }

        void unhandled_exception()
        {
            std::rethrow_exception(std::current_exception());
        }
    };

private:
    handle_type m_coro;

public:
    explicit generator(const handle_type coro) : m_coro(coro) {}

    handle_type handle() const {return m_coro;}

    std::tuple<T, bool> next()
    {
        m_coro.resume();

        return std::make_tuple(
            m_coro.promise().m_current_value,
            m_coro.done()
        );
    } // End of next()

    ~generator()
    {
        m_coro.destroy();
    }
};


#endif //N01_GEN_AWAITABLE_GENERATOR_HPP
