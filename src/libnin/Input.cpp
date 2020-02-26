#include <libnin/Input.h>

using namespace libnin;

Input::Input()
: _polling{}
, _state{}
, _controller{}
, _controllerLatch{}
{

}

void Input::set(std::uint8_t value)
{
    _state = value;
}

void Input::poll(bool polling)
{
    _polling = polling;
    if (_polling)
        reset();
}


std::uint8_t Input::read()
{
    if (_polling)
        reset();
    if (_controllerLatch < 8)
        return ((_controller >> _controllerLatch++) & 0x01);
    return 0x01;
}

void Input::reset()
{
    _controller = _state;
    _controllerLatch = 0;
}
