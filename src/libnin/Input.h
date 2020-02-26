#ifndef LIBNIN_INPUT_H
#define LIBNIN_INPUT_H 1

#include <cstdint>
#include <libnin/NonCopyable.h>

namespace libnin
{

class Input : private NonCopyable
{
public:
    Input();

    void set(std::uint8_t input);
    void poll(bool polling);

    std::uint8_t read();

private:
    void reset();

    bool            _polling;
    std::uint8_t    _state;
    std::uint8_t    _controller;
    std::uint8_t    _controllerLatch;
};

}

#endif
