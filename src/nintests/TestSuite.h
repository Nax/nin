/*
 * Nin, a Nintendo Entertainment System Emulator.
 * 
 * Copyright (c) 2018-2022 Maxime Bacoux
 * All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2, as published by the Free Software Foundation.
 * 
 * Alternatively, this program can be licensed under a commercial license
 * upon request.
 * 
 * When using the program under the GNU General Public License Version 2 license,
 * the following apply:
 * 
 *  1. This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *  2. You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <vector>

struct NinState;
class TestSuite
{
public:
    using Pred = std::function<bool(NinState*)>;

    TestSuite();

    void add(const char* name, const char* path, std::size_t cycles, Pred pred);
    int  run();

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

    std::vector<Test>       _tests;
    std::mutex              _mutex;
    std::condition_variable _cv;
};

#endif
