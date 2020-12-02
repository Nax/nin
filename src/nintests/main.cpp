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

#include "TestSuite.h"
#include <nin/nin.h>

#define SEC_NTSC(x) ((std::size_t)((x)*1789773))

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
    std::uint8_t  tmp[0x800];
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
    suite.add("Blargg CPU Test v5 - 01 Basics", "blargg_cpu_test5/01-basics.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 02 Implied", "blargg_cpu_test5/02-implied.nes", SEC_NTSC(1.75f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 03 Immediate", "blargg_cpu_test5/03-immediate.nes", SEC_NTSC(1.50f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 04 Zero Page", "blargg_cpu_test5/04-zero_page.nes", SEC_NTSC(2.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 05 Zero Page XY", "blargg_cpu_test5/05-zp_xy.nes", SEC_NTSC(4.50f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 06 Absolute", "blargg_cpu_test5/06-absolute.nes", SEC_NTSC(2.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 07 Absolute XY", "blargg_cpu_test5/07-abs_xy.nes", SEC_NTSC(6.50f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 08 Indirect X", "blargg_cpu_test5/08-ind_x.nes", SEC_NTSC(3.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 09 Indirect Y", "blargg_cpu_test5/09-ind_y.nes", SEC_NTSC(3.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 10 Branches", "blargg_cpu_test5/10-branches.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 11 Stack", "blargg_cpu_test5/11-stack.nes", SEC_NTSC(2.75f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 12 Jumps", "blargg_cpu_test5/12-jmp_jsr.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 13 RTS", "blargg_cpu_test5/13-rts.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 14 RTI", "blargg_cpu_test5/14-rti.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 15 BRK", "blargg_cpu_test5/15-brk.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });
    suite.add("Blargg CPU Test v5 - 16 Special", "blargg_cpu_test5/16-special.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchBlargg(state); });

    /* Blargg branch timing */
    suite.add("Blargg Branch Timing - 1 Basics", "blargg_branch_timing/1-basics.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x7f7239ea); });
    suite.add("Blargg Branch Timing - 2 Backward", "blargg_branch_timing/2-backward.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x66f61ebc); });
    suite.add("Blargg Branch Timing - 3 Forward", "blargg_branch_timing/3-forward.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x0598cd78); });

    /* Blargg PPU Tests */
    suite.add("Blargg PPU Tests - 1 Palette RAM", "blargg_ppu_tests/1-palette_ram.nes", SEC_NTSC(0.50f), [](NinState* state) { return matchHash(state, 0x65dde75d); });
    suite.add("Blargg PPU Tests - 2 Sprite RAM", "blargg_ppu_tests/2-sprite_ram.nes", SEC_NTSC(0.50f), [](NinState* state) { return matchHash(state, 0x65dde75d); });
    suite.add("Blargg PPU Tests - 3 VBlank Clear Time", "blargg_ppu_tests/3-vbl_clear_time.nes", SEC_NTSC(0.50f), [](NinState* state) { return matchHash(state, 0x65dde75d); });
    suite.add("Blargg PPU Tests - 4 VRAM Access", "blargg_ppu_tests/4-vram_access.nes", SEC_NTSC(0.50f), [](NinState* state) { return matchHash(state, 0x13aa27ec); });

    /* Blargg Sprite Hit Tests */
    suite.add("Blargg Sprite Hit Tests - 01 Basics", "blargg_sprite_hit_tests/01-basics.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0xab309e07); });
    suite.add("Blargg Sprite Hit Tests - 02 Alignment", "blargg_sprite_hit_tests/02-alignment.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x8a961a81); });
    suite.add("Blargg Sprite Hit Tests - 03 Corners", "blargg_sprite_hit_tests/03-corners.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x22790ce3); });
    suite.add("Blargg Sprite Hit Tests - 04 Flip", "blargg_sprite_hit_tests/04-flip.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x04524085); });
    suite.add("Blargg Sprite Hit Tests - 05 Left Clip", "blargg_sprite_hit_tests/05-left_clip.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0xf0b6b9fa); });
    suite.add("Blargg Sprite Hit Tests - 06 Right Edge", "blargg_sprite_hit_tests/06-right_edge.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x6d74802d); });
    suite.add("Blargg Sprite Hit Tests - 07 Screen Bottom", "blargg_sprite_hit_tests/07-screen_bottom.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x355c0002); });
    suite.add("Blargg Sprite Hit Tests - 08 Double Height", "blargg_sprite_hit_tests/08-double_height.nes", SEC_NTSC(1.00f), [](NinState* state) { return matchHash(state, 0x2195fb97); });
    suite.add("Blargg Sprite Hit Tests - 09 Timing Basics", "blargg_sprite_hit_tests/09-timing_basics.nes", SEC_NTSC(1.50f), [](NinState* state) { return matchHash(state, 0xfce52375); });
    suite.add("Blargg Sprite Hit Tests - 10 Timing Order", "blargg_sprite_hit_tests/10-timing_order.nes", SEC_NTSC(1.50f), [](NinState* state) { return matchHash(state, 0x24ff7b4e); });
    suite.add("Blargg Sprite Hit Tests - 11 Edge Timing", "blargg_sprite_hit_tests/11-edge_timing.nes", SEC_NTSC(1.50f), [](NinState* state) { return matchHash(state, 0x7f6aa2ed); });

    /* Blargg VBL NMI Timing */
    suite.add("Blargg VBL NMI Timing - 1 Frame Basics", "blargg_vbl_nmi_timing/1-frame_basics.nes", SEC_NTSC(3.00f), [](NinState* state) { return matchHash(state, 0xa11f46bb); });
    suite.add("Blargg VBL NMI Timing - 2 VBL Timing", "blargg_vbl_nmi_timing/2-vbl_timing.nes", SEC_NTSC(3.00f), [](NinState* state) { return matchHash(state, 0x9a79f4b7); });
    suite.add("Blargg VBL NMI Timing - 3 Even/Odd Frames", "blargg_vbl_nmi_timing/3-even_odd_frames.nes", SEC_NTSC(2.00f), [](NinState* state) { return matchHash(state, 0x2e123d7c); });
    suite.add("Blargg VBL NMI Timing - 4 VBL Clear Timing", "blargg_vbl_nmi_timing/4-vbl_clear_timing.nes", SEC_NTSC(2.00f), [](NinState* state) { return matchHash(state, 0x3c3ca36c); });
    suite.add("Blargg VBL NMI Timing - 5 NMI Suppression", "blargg_vbl_nmi_timing/5-nmi_suppression.nes", SEC_NTSC(3.00f), [](NinState* state) { return matchHash(state, 0x10c406ee); });
    suite.add("Blargg VBL NMI Timing - 6 NMI Disable", "blargg_vbl_nmi_timing/6-nmi_disable.nes", SEC_NTSC(2.00f), [](NinState* state) { return matchHash(state, 0xe4716bdc); });
    suite.add("Blargg VBL NMI Timing - 7 NMI Timing", "blargg_vbl_nmi_timing/7-nmi_timing.nes", SEC_NTSC(2.00f), [](NinState* state) { return matchHash(state, 0x90bdf687); });

    /* Blargg PPU VBL/NMI */
    suite.add("Blargg PPU VBL/NMI - 01 VBL Basics", "blargg_ppu_vbl_nmi/01-vbl_basics.nes", SEC_NTSC(2.50f), [](NinState* state) { return matchHash(state, 0x00e1584a); });
    suite.add("Blargg PPU VBL/NMI - 02 VBL Set Time", "blargg_ppu_vbl_nmi/02-vbl_set_time.nes", SEC_NTSC(3.00f), [](NinState* state) { return matchHash(state, 0x06ee352e); });
    suite.add("Blargg PPU VBL/NMI - 03 VBL Clear Time", "blargg_ppu_vbl_nmi/03-vbl_clear_time.nes", SEC_NTSC(3.00f), [](NinState* state) { return matchHash(state, 0xfafe1935); });
    suite.add("Blargg PPU VBL/NMI - 04 NMI Control", "blargg_ppu_vbl_nmi/04-nmi_control.nes", SEC_NTSC(2.50f), [](NinState* state) { return matchHash(state, 0x4977c566); });
    suite.add("Blargg PPU VBL/NMI - 05 NMI Timing", "blargg_ppu_vbl_nmi/05-nmi_timing.nes", SEC_NTSC(3.50f), [](NinState* state) { return matchHash(state, 0xfaa58c51); });
    suite.add("Blargg PPU VBL/NMI - 06 Suppression", "blargg_ppu_vbl_nmi/06-suppression.nes", SEC_NTSC(4.00f), [](NinState* state) { return matchHash(state, 0xe23dbd3e); });
    suite.add("Blargg PPU VBL/NMI - 07 NMI ON Timing", "blargg_ppu_vbl_nmi/07-nmi_on_timing.nes", SEC_NTSC(3.50f), [](NinState* state) { return matchHash(state, 0x40ed2f23); });
    suite.add("Blargg PPU VBL/NMI - 08 NMI OFF Timing", "blargg_ppu_vbl_nmi/08-nmi_off_timing.nes", SEC_NTSC(4.00f), [](NinState* state) { return matchHash(state, 0xa4fe769e); });
    suite.add("Blargg PPU VBL/NMI - 09 Even/Odd Frames", "blargg_ppu_vbl_nmi/09-even_odd_frames.nes", SEC_NTSC(2.50f), [](NinState* state) { return matchHash(state, 0xbc57013a); });
    suite.add("Blargg PPU VBL/NMI - 10 Even/Odd Timing", "blargg_ppu_vbl_nmi/10-even_odd_timing.nes", SEC_NTSC(2.50f), [](NinState* state) { return matchHash(state, 0x0f4abf9d); });

    return suite.run();
}
