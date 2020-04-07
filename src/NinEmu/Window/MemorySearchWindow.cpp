/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstring>
#include <QtWidgets>
#include <NinEmu/Window/MemorySearchWindow.h>

MemorySearchWindow::MemorySearchWindow(QWidget* parent)
: QWidget(parent, Qt::Window)
, _searchFunc{}
, _searchValue{}
, _buffer{}
, _bufferPrev{}
, _bufferFirst{}
{
    setWindowTitle("Memory Search");

    QVBoxLayout* layout;
    layout = new QVBoxLayout;

    QGridLayout* layoutToolbox = new QGridLayout;

    QComboBox* boxSearchFunc = new QComboBox;
    boxSearchFunc->addItem("Equal to");
    boxSearchFunc->addItem("Not equal to");
    boxSearchFunc->addItem("Lesser than");
    boxSearchFunc->addItem("Greater than");
    boxSearchFunc->addItem("Lesser or equal to");
    boxSearchFunc->addItem("Greater or equal to");
    connect(boxSearchFunc, qOverload<int>(&QComboBox::activated), this, &MemorySearchWindow::setSearchFunc);
    layoutToolbox->addWidget(boxSearchFunc, 0, 1);

    QComboBox* boxSearchValue = new QComboBox;
    boxSearchValue->addItem("the previous value");
    boxSearchValue->addItem("the first value");
    boxSearchValue->addItem("a custom value");
    connect(boxSearchValue, qOverload<int>(&QComboBox::activated), this, &MemorySearchWindow::setSearchValue);
    layoutToolbox->addWidget(boxSearchValue, 0, 2);

    QPushButton* btnReset = new QPushButton("New Search");
    connect(btnReset, &QPushButton::pressed, this, &MemorySearchWindow::reset);
    layoutToolbox->addWidget(btnReset, 0, 0);

    QPushButton* btnSearch = new QPushButton("Search");
    connect(btnSearch, &QPushButton::pressed, this, &MemorySearchWindow::search);
    layoutToolbox->addWidget(btnSearch, 1, 0);

    layout->addLayout(layoutToolbox);

    _labelResults = new QLabel;
    layout->addWidget(_labelResults);

    _table = new QTableWidget;
    _table->setColumnCount(4);
    _table->setHorizontalHeaderItem(0, new QTableWidgetItem("Address"));
    _table->setHorizontalHeaderItem(1, new QTableWidgetItem("Current"));
    _table->setHorizontalHeaderItem(2, new QTableWidgetItem("Previous"));
    _table->setHorizontalHeaderItem(3, new QTableWidgetItem("First"));
    _table->verticalHeader()->hide();
    _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(_table);

    setLayout(layout);

    updateResults();
}

void MemorySearchWindow::refresh(NinState* state)
{
    ninDumpMemory(state, _buffer, 0, 0x10000);
    updateTable();
}

void MemorySearchWindow::setSearchFunc(int searchFunc)
{
    _searchFunc = (SearchFunc)searchFunc;
    updateTable();
}

void MemorySearchWindow::setSearchValue(int searchValue)
{
    _searchValue = (SearchValue)searchValue;
    updateTable();
}

void MemorySearchWindow::updateTable()
{
    QTableWidgetItem* itemAddr;
    QTableWidgetItem* itemValue;
    QTableWidgetItem* itemValuePrev;
    QTableWidgetItem* itemValueFirst;
    std::uint16_t addr;
    int count{};
    bool match;

    _table->setRowCount(0);
    for (const auto& span : _spans)
    {
        for (int j = 0; j <= span.size; ++j)
        {
            _table->setRowCount(count + 1);
            addr = span.base + j;
            match = searchPredicate(addr);

            itemAddr = new QTableWidgetItem(QString("0x%1").arg((int)addr, 4, 16, QChar('0')));
            itemValue = new QTableWidgetItem(QString("0x%1").arg((int)_buffer[addr], 2, 16, QChar('0')));
            itemValuePrev = new QTableWidgetItem(QString("0x%1").arg((int)_bufferPrev[addr], 2, 16, QChar('0')));
            itemValueFirst = new QTableWidgetItem(QString("0x%1").arg((int)_bufferFirst[addr], 2, 16, QChar('0')));

            if (!match)
                itemValue->setForeground(QBrush(QColor(255, 0, 0)));

            _table->setItem(count, 0, itemAddr);
            _table->setItem(count, 1, itemValue);
            _table->setItem(count, 2, itemValuePrev);
            _table->setItem(count, 3, itemValueFirst);

            count++;
            if (count >= 1000)
                return;
        }
    }
}

void MemorySearchWindow::updateResults()
{
    updateTable();
    if (_spans.size())
    {
        int sum{};

        for (const auto& span : _spans)
        {
            sum += (span.size + 1);
        }

        _labelResults->setText(QString("Results (") + QString::number(sum) + ")");
    }
    else
    {
        _labelResults->setText("No result");
    }
}

void MemorySearchWindow::reset()
{
    _spans.clear();
    _spans.push_back({ 0x0000, 0xffff });
    std::memcpy(_bufferPrev, _buffer, 0x10000);
    std::memcpy(_bufferFirst, _buffer, 0x10000);
    updateResults();
}

void MemorySearchWindow::search()
{
    std::uint16_t addr;
    std::vector<MemorySpan> matchingSpans;
    bool inSpan{};

    for (const auto& span : _spans)
    {
        for (int j = 0; j <= span.size; ++j)
        {
            addr = span.base + j;
            if (searchPredicate(addr))
            {
                if (inSpan)
                    matchingSpans.back().size++;
                else
                {
                    matchingSpans.push_back({ addr, 0x0000 });
                    inSpan = true;
                }
            }
            else
            {
                inSpan = false;
            }
        }
    }

    _spans = matchingSpans;
    std::memcpy(_bufferPrev, _buffer, 0x10000);
    updateResults();
}

bool MemorySearchWindow::searchPredicate(std::uint16_t addr) const
{
    int value;
    int other;

    value = _buffer[addr];
    other = _bufferPrev[addr];

    switch (_searchFunc)
    {
    case SearchFunc::Equal:
        return value == other;
    case SearchFunc::NotEqual:
        return value != other;
    case SearchFunc::Lesser:
        return value < other;
    case SearchFunc::Greater:
        return value > other;
    case SearchFunc::LesserEqual:
        return value <= other;
    case SearchFunc::GreaterEqual:
        return value >= other;
    }
    return false;
}
