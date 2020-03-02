#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <functional>
#include <vector>
#include <mutex>
#include <condition_variable>

struct NinState;
class TestSuite
{
public:
    using Pred = std::function<bool(NinState*)>;

    TestSuite();

    void    add(const char* name, const char* path, std::size_t cycles, Pred pred);
    int     run();

private:
    enum class TestState
    {
        Pending,
        Running,
        Ok,
        Fail,
        Error
    };

    struct Test
    {
        const char* name;
        const char* path;
        std::size_t cycles;
        Pred        pred;
        TestState   state;
    };

    void run_thread();
    void run_test(Test& t);

    std::vector<Test>           _tests;
    std::mutex                  _mutex;
    std::condition_variable     _cv;
};

#endif
