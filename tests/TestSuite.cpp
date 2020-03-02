#include <cstdio>
#include <nin/nin.h>
#include "TestSuite.h"

TestSuite::TestSuite()
{

}

void TestSuite::add(const char* name, const char* path, std::size_t cycles, Pred pred)
{
    _tests.push_back({ name, path, cycles, pred, TestState::Pending });
}

int TestSuite::run()
{
    int passed = 0;

    for (auto& t : _tests)
    {
        NinState* s;

        if (ninCreateState(&s, t.path) != NIN_OK)
        {
            t.state = TestState::Error;
            std::putchar('E');
            continue;
        }

        ninRunCycles(s, t.cycles, nullptr);
        if (t.pred(s))
        {
            t.state = TestState::Ok;
            std::putchar('.');
            passed++;
        }
        else
        {
            t.state = TestState::Fail;
            std::putchar('F');
        }

        ninDestroyState(s);
    }

    std::putchar('\n');
    std::putchar('\n');
    std::printf("Passed: %d/%d\n", passed, (int)_tests.size());

    if (passed == (int)_tests.size())
        return 0;

    std::putchar('\n');
    for (auto& t : _tests)
    {
        switch (t.state)
        {
        case TestState::Fail:
            std::printf("FAIL: %s\n", t.name);
            break;
        case TestState::Error:
            std::printf("ERROR: %s\n", t.name);
            break;
        default:
            break;
        }
    }
    return 1;
}
