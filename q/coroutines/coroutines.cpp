// g++ -std=c++20 -masm=intel -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables -fcoroutines -lpthread -g -Wall -Wextra -Wpedantic

#include <cassert>
#include <condition_variable>
#include <coroutine>
#include <iostream>
#include <new>
#include <queue>
#include <thread>
#include <version>

#define FWD(x) std::forward<decltype(x)>(x)

#ifdef __cpp_lib_hardware_interference_size
static constexpr std::size_t hardware_destructive_interference_size
    = std::hardware_destructive_interference_size;
#else
static constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

static constexpr std::size_t N = 1024 * 1024;

struct base_promise {
    void return_void() {}
    void unhandled_exception() const noexcept { std::terminate(); }
};

class thread_pool {
public:
    explicit thread_pool(std::size_t n);
    ~thread_pool();
    auto schedule();
private:
    struct alignas(hardware_destructive_interference_size) thread_data {
        std::thread thread = {};
        bool stop = {};
    };
    void thread_loop(std::size_t i);
    void enqueue(std::coroutine_handle<> h);
    std::vector<thread_data> v = {};
    std::queue<std::coroutine_handle<>> q = {};
    std::mutex m = {};
    std::condition_variable c = {};
};

thread_pool::thread_pool(std::size_t n) {
    this->v.reserve(n);
    for(std::size_t i = 0; i < n; ++i)
        this->v.emplace_back(std::thread{&thread_pool::thread_loop, this, i});
}

thread_pool::~thread_pool() {
    for(auto &x : this->v)
        std::atomic_ref{x.stop} = true;
    this->c.notify_all();
    for(auto &x : this->v)
        x.thread.join();
}

void thread_pool::thread_loop(std::size_t i) {
    auto stop = std::atomic_ref{this->v[i].stop};
    std::coroutine_handle<> h;
    while(!stop) {
        std::unique_lock lock(this->m);
        while(!stop && this->q.empty())
            this->c.wait(lock);
        if(this->q.empty())
            continue;
        h = std::move(this->q.front());
        this->q.pop();
        lock.unlock();
        if(!h.done())
            h.resume();
        if(h.done())
            h.destroy();
        else
            this->enqueue(h);
    }
}

void thread_pool::enqueue(std::coroutine_handle<> h) {
    std::lock_guard lock(this->m);
    this->q.emplace(h);
    this->c.notify_one();
}

auto thread_pool::schedule() {
    struct : std::suspend_always {
        thread_pool &p;
        void await_suspend(std::coroutine_handle<> h) { this->p.enqueue(h); }
    } ret = {{}, *this};
    return ret;
}

void time(auto &&f) {
    using C = std::chrono::steady_clock;
    using D = std::chrono::milliseconds;
    const auto start = C::now();
    std::forward<decltype(f)>(f)();
    const auto dt = C::now() - start;
    std::cout << std::chrono::duration_cast<D>(dt).count() << "ms\n";
}

/******************************************************************************/

class simple_task {
public:
    struct promise_type : base_promise {
        auto get_return_object() noexcept {
            using H = std::coroutine_handle<promise_type>;
            return simple_task{H::from_promise(*this)};
        }
        auto initial_suspend() const noexcept { return std::suspend_always{}; }
        auto final_suspend() const noexcept { return std::suspend_always{}; }
    };
    simple_task(const simple_task&) = delete;
    simple_task(simple_task&&) = delete;
    void resume() const noexcept;
private:
    explicit simple_task(std::coroutine_handle<promise_type> h_) : h{h_} {}
    std::coroutine_handle<promise_type> h;
};

void simple_task::resume() const noexcept {
    assert(this->h);
    if(!this->h.done())
        this->h.resume();
}

simple_task test0_f() {
//    std::cout << "start\n";
    co_await std::suspend_always();
//    std::cout << "end\n";
}

void test0() {
//    std::cout << "caller start\n";
    auto t = test0_f();
//    std::cout << "caller resume\n";
    t.resume();
//    std::cout << "caller end\n";
}

/******************************************************************************/

struct task {
    struct promise_type : base_promise {
        auto get_return_object() const noexcept { return task{}; }
        auto initial_suspend() const noexcept { return std::suspend_never{}; }
        auto final_suspend() const noexcept { return std::suspend_always{}; }
    };
};

static std::atomic<std::size_t> test1_n = {};

task test1_f(thread_pool *p) {
    co_await p->schedule();
//    std::cout << "thread " << std::this_thread::get_id() << '\n';
    test1_n.fetch_add(1);
}

void test1() {
//    std::cout << "main thread: " << std::this_thread::get_id() << '\n';
    thread_pool p{8};
    time([&p] {
        for(std::size_t i = 0; i < N; ++i)
            test1_f(&p);
        while(test1_n != N);
    });
}

/******************************************************************************/

class continuation {
public:
    class promise_type : public base_promise {
    public:
        using handle_type = std::coroutine_handle<promise_type>;
        auto get_return_object() noexcept
            { return continuation{handle_type::from_promise(*this)}; }
        auto initial_suspend() const noexcept { return std::suspend_always{}; }
        auto final_suspend() const noexcept;
        void set_continuation(std::coroutine_handle<> h_) noexcept
            { this->h = h_; }
    private:
        std::coroutine_handle<> h = std::noop_coroutine();
    };
    ~continuation() { h.destroy(); }
    auto operator co_await() noexcept;
    void resume() { this->h.resume(); }
private:
    explicit continuation(promise_type::handle_type h_) : h{h_} {}
    promise_type::handle_type h;
};

auto continuation::promise_type::final_suspend() const noexcept {
    struct : std::suspend_always {
        auto await_suspend(handle_type coro) noexcept {
            return coro.promise().h;
        }
    } ret = {};
    return ret;
}

auto continuation::operator co_await() noexcept {
    struct {
        bool await_ready() const noexcept { return !this->h || this->h.done(); }
        void await_resume() const noexcept {}
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> h_) {
            this->h.promise().set_continuation(h_);
            return this->h;
        }
        promise_type::handle_type h;
    } ret = {this->h};
    return ret;
}

static std::atomic<std::size_t> test2_n = {};

continuation test2_f(thread_pool *p, auto f) {
    co_await p->schedule();
    std::move(f)();
    std::cout << "c0 done\n";
}

continuation test2_sc() {
    co_return;
}

task test2_s() {
    co_await std::suspend_always{};
}

continuation test2_scc(continuation *c, auto &&f) {
    co_await *c;
    co_await f;
}

continuation test2_c(continuation *c, auto f) {
    co_await *c;
    std::move(f)();
    std::cout << "c1 done\n";
}

task schedule(thread_pool *p, auto &&f) {
    co_await p->schedule();
    std::this_thread::sleep_for(std::chrono::seconds{1});
    co_await f;
}

void test2() {
    thread_pool p{8};
    auto sc = test2_sc();
    auto s = test2_scc(&sc, test2_s());
    schedule(&p, s);
    std::cout << "scheduled\n";
//    time([&p] {
//        {
//            auto t0 = test2_f(&p, [] { test2_n.fetch_add(1); });
//            auto t1 = test2_c(&t0, [] { test2_n.fetch_add(1); });
//            auto t2 = test2_c(&t1, [] { test2_n.fetch_add(1); });
//            std::cout << "resuming\n";
//            t2.resume();
//            std::cout << "resumed\n";
//        }
//        std::cout << "outside\n";
//        while(test2_n != 3);
//    });
}

int main() {
//    test0();
//    test1();
    test2();
}
