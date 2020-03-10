#include <nin/nin.h>
#include "TestSuite.h"

#define SEC_NTSC(x) ((std::size_t)((x) * 1789773))

static std::uint32_t qhash(const std::uint8_t* data, std::size_t len)
{
    uint32_t hash = 0;

    for (size_t i = 0; i < len; ++i)
    {
        hash += data[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

static bool matchBlargg(NinState* state)
{
    std::uint8_t tmp[4];

    ninDumpMemory(state, tmp, 0x6000, 0x04);
    return (tmp[0] == 0 && tmp[1] == 0xDE && tmp[2] == 0xB0 && tmp[3] == 0x61);
}

static bool matchHash(NinState* state, std::uint32_t expected)
{
    std::uint8_t tmp[0x800];
    std::uint32_t hash;

    ninDumpNametable(state, tmp, 0);
    ninDumpNametable(state, tmp + 0x400, 1);

    hash = qhash(tmp, 0x800);
    /*
    if (hash != expected)
        printf("\n\n\nEXPECTED: 0x%08x, RESULT: 0x%08x\n\n\n", expected, hash);
    */
    return (hash == expected);
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

    /* Blargg branch timing */
    suite.add("Blargg Branch Timing - 1 Basics",   "roms/blargg_branch_timing/1-basics.nes",    SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x7f7239ea); });
    suite.add("Blargg Branch Timing - 2 Backward", "roms/blargg_branch_timing/2-backward.nes",  SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x66f61ebc); });
    suite.add("Blargg Branch Timing - 3 Forward",  "roms/blargg_branch_timing/3-forward.nes",   SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x0598cd78); });

    /* Blargg PPU Tests */
    suite.add("Blargg PPU Tests - 1 Palette RAM",       "roms/blargg_ppu_tests/1-palette_ram.nes",      SEC_NTSC(0.50f), [](NinState* state) { return matchHash(state, 0x65dde75d); });
    suite.add("Blargg PPU Tests - 2 Sprite RAM",        "roms/blargg_ppu_tests/2-sprite_ram.nes",       SEC_NTSC(0.50f), [](NinState* state) { return matchHash(state, 0x65dde75d); });
    suite.add("Blargg PPU Tests - 3 VBlank Clear Time", "roms/blargg_ppu_tests/3-vbl_clear_time.nes",   SEC_NTSC(0.50f), [](NinState* state) { return matchHash(state, 0x65dde75d); });
    suite.add("Blargg PPU Tests - 4 VRAM Access",       "roms/blargg_ppu_tests/4-vram_access.nes",      SEC_NTSC(0.50f), [](NinState* state) { return matchHash(state, 0x13aa27ec); });

    /* Blargg Sprite Hit Tests */
    suite.add("Blargg Sprite Hit Tests - 01 Basics",        "roms/blargg_sprite_hit_tests/01-basics.nes",           SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0xab309e07); });
    suite.add("Blargg Sprite Hit Tests - 02 Alignment",     "roms/blargg_sprite_hit_tests/02-alignment.nes",        SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x8a961a81); });
    suite.add("Blargg Sprite Hit Tests - 03 Corners",       "roms/blargg_sprite_hit_tests/03-corners.nes",          SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x22790ce3); });
    suite.add("Blargg Sprite Hit Tests - 04 Flip",          "roms/blargg_sprite_hit_tests/04-flip.nes",             SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x04524085); });
    suite.add("Blargg Sprite Hit Tests - 05 Left Clip",     "roms/blargg_sprite_hit_tests/05-left_clip.nes",        SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0xf0b6b9fa); });
    suite.add("Blargg Sprite Hit Tests - 06 Right Edge",    "roms/blargg_sprite_hit_tests/06-right_edge.nes",       SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x6d74802d); });
    suite.add("Blargg Sprite Hit Tests - 07 Screen Bottom", "roms/blargg_sprite_hit_tests/07-screen_bottom.nes",    SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x355c0002); });
    suite.add("Blargg Sprite Hit Tests - 08 Double Height", "roms/blargg_sprite_hit_tests/08-double_height.nes",    SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x2195fb97); });
    suite.add("Blargg Sprite Hit Tests - 09 Timing Basics", "roms/blargg_sprite_hit_tests/09-timing_basics.nes",    SEC_NTSC(1.50f), [](NinState* state) { return matchHash(state, 0xfce52375); });
    suite.add("Blargg Sprite Hit Tests - 10 Timing Order",  "roms/blargg_sprite_hit_tests/10-timing_order.nes",     SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x24ff7b4e); });
    suite.add("Blargg Sprite Hit Tests - 11 Edge Timing",   "roms/blargg_sprite_hit_tests/11-edge_timing.nes",      SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x7f6aa2ed); });

    return suite.run();
}
