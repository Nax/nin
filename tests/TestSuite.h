#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <functional>
#include <vector>

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

    std::vector<Test>   _tests;
};

#endif
