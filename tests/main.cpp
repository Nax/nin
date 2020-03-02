#include <nin/nin.h>
#include "TestSuite.h"

#define SEC_NTSC(x) ((std::size_t)((x) * 1789773))

static bool matchBlargg(NinState* state)
{
    std::uint8_t tmp[4];

    ninDumpMemory(state, tmp, 0x6000, 0x04);
    return (tmp[0] == 0 && tmp[1] == 0xDE && tmp[2] == 0xB0 && tmp[3] == 0x61);
}

int main(void)
{
    TestSuite suite;

    /* Blargg CPU Test v5 */
    suite.add("Blargg CPU Test v5 - 01 Basics",         "roms/blargg_cpu_test5/01-basics.nes",      SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 02 Implied",        "roms/blargg_cpu_test5/02-implied.nes",     SEC_NTSC(1.75f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 03 Immediate",      "roms/blargg_cpu_test5/03-immediate.nes",   SEC_NTSC(1.50f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 04 Zero Page",      "roms/blargg_cpu_test5/04-zero_page.nes",   SEC_NTSC(2.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 05 Zero Page XY",   "roms/blargg_cpu_test5/05-zp_xy.nes",       SEC_NTSC(4.50f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 06 Absolute",       "roms/blargg_cpu_test5/06-absolute.nes",    SEC_NTSC(2.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 07 Absolute XY",    "roms/blargg_cpu_test5/07-abs_xy.nes",      SEC_NTSC(6.50f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 08 Indirect X",     "roms/blargg_cpu_test5/08-ind_x.nes",       SEC_NTSC(3.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 09 Indirect Y",     "roms/blargg_cpu_test5/09-ind_y.nes",       SEC_NTSC(3.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 10 Branches",       "roms/blargg_cpu_test5/10-branches.nes",    SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 11 Stack",          "roms/blargg_cpu_test5/11-stack.nes",       SEC_NTSC(2.75f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 12 Jumps",          "roms/blargg_cpu_test5/12-jmp_jsr.nes",     SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 13 RTS",            "roms/blargg_cpu_test5/13-rts.nes",         SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 14 RTI",            "roms/blargg_cpu_test5/14-rti.nes",         SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 15 BRK",            "roms/blargg_cpu_test5/15-brk.nes",         SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 16 Special",        "roms/blargg_cpu_test5/16-special.nes",     SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });

    return suite.run();
}
