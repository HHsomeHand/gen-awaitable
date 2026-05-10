一个函数，如果内部用了 co_return co_yeild  co_await

那么它就会认为这个函数是 continuation，会从返回值找 promise_type 来控制整个 continuation 的行为。

promise_type 是一组协议，包含:
这个函数执行后，返回什么类型，get_return_object

这个函数体是否是立即执行?
initial_suspend

执行完，是否直接销毁?
final_suspend

如果函数 co_return 返回什么内容
return_void
return_value

函数内调用 co_yield 行为
yield_value

co_yield 会变成
co_await promise.yield_value(x)

---
co_await child()

child 这里是一个 Awaiter

这里调用 child::await_suspend(coro_handler)

参数传递的是当前协程的句柄

也就是 child 父协程的句柄

child 的 promise 应该保存父协程句柄，如定义如下成员变量:
std::coroutine_handle<> continuation;

注意这里不要用handle_type，父协程类型(task<int>)，可能和子协程不一致(task<double>)

也就是 child 的 promise continuation保存 父句柄。

然后在 final_suspend 里面返回 FinalAwaiter。

child.promise().continuation = parent_handle;

--
FinalAwaiter

void await_suspend(handle h)

这里可以接收当前 continuation 的句柄

await_suspend 可以返回：
+ void
+ bool
true  -> suspend
false -> 不 suspend

+ coroutine_handle:
直接 resume 返回的 coroutine

这里可以通过子协程(当前协程) 参数 h，取到 promise，获取父协程句柄，然后直接返回

如果父协程句柄为空，if 判断为false，可以直接返回 std::noop_coroutine。

不然如果返回一个无效的句柄，程序会直接崩溃。

---

让 final_suspend 返回，FinalAwaiter

final_suspend 不能直接返回 coro_handler，需要返回 awaiter

final_suspend await_resume 的返回值会被忽略。

---

awaiter

可以被 co await 等待的对象。

包含是否立刻 suspend
await_ready

suspend 行为
await_suspend

恢复后的行为，以及 co_await 返回值
await_resume

---

task 析构的时候，不要 destory coro，不然 event loop 有问题。

--

记得不要同时 initial_suspend 和 await_suspend 同时把 coro_handle 加入 queue，不然会重复 resume。

我们的协程 coro destory方案是，final_suspend的时候，自己 destory自己。

return_value 调用时机比 final_suspend 早，所以不用担心 coro frame destroy 后，return_value 无法拿到 coro frame 的 promise 对象的值。