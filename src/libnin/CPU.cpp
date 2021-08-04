#include <libnin/CPU.h>

using namespace libnin;

CPU::CPU()
: cycle{}
, pc{}
, regs{}
, p{PFLAG_I}
, p2{p}
{

}
