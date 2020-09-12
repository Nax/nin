/*
 * Nin, a Nintendo Entertainment System Emulator.
 *
 * Copyright (c) 2018-2020 Maxime Bacoux
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

#ifndef MEMORY_SEARCH_WINDOW_H
#define MEMORY_SEARCH_WINDOW_H 1

#include <QLabel>
#include <QTableWidget>
#include <QWidget>
#include <cstdint>
#include <nin/nin.h>
#include <vector>

class MemorySearchWindow : public QWidget
{
    Q_OBJECT;

public:
    explicit MemorySearchWindow(QWidget* parent = nullptr);

public slots:
    void refresh(NinState* state);

private:
    enum class SearchFunc
    {
        Equal = 0,
        NotEqual,
        Lesser,
        Greater,
        LesserEqual,
        GreaterEqual
    };

    enum class SearchValue
    {
        Previous = 0,
        First,
        Custom
    };

    struct MemorySpan
    {
        std::uint16_t base;
        std::uint16_t size;
    };

    void setSearchFunc(int searchFunc);
    void setSearchValue(int searchValue);

    void updateTable();
    void updateResults();
    void reset();
    void search();
    bool searchPredicate(std::uint16_t addr) const;

    QLabel*       _labelResults;
    QTableWidget* _table;

    SearchFunc              _searchFunc;
    SearchValue             _searchValue;
    std::vector<MemorySpan> _spans;
    uint8_t                 _buffer[0x10000];
    uint8_t                 _bufferPrev[0x10000];
    uint8_t                 _bufferFirst[0x10000];
};

#endif
