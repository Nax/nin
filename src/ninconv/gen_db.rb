# Nin, a Nintendo Entertainment System Emulator.
#
# Copyright (c) 2018-2020 Maxime Bacoux
# All rights reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# Version 2, as published by the Free Software Foundation.
#
# Alternatively, this program can be licensed under a commercial license
# upon request.
#
# When using the program under the GNU General Public License Version 2 license,
# the following apply:
#
#  1. This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#  2. You should have received a copy of the GNU General Public License
#     along with this program; if not, write to the Free Software
#     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

require 'nokogiri'

Game = Struct.new(
  :name,
  :crc32,
  :region,
  :mapper,
  :mirror,
  :battery,
  :prg_rom_size,
  :prg_ram_size,
  :chr_rom_size,
  :chr_ram_size
) do
  def serialize
    n = name.tr('"', '')
    %Q[{ "#{n}", #{"0x%08x" % crc32}, #{region}, #{mapper}, #{mirror}, #{battery}, #{"0x%0x" % prg_rom_size}, #{"0x%0x" % prg_ram_size}, #{"0x%0x" % chr_rom_size}, #{"0x%0x" % chr_ram_size} },]
  end
end

def convert_size(x)
  x.to_i * 1024
end

db_file = ARGV[0]
db = File.open(db_file) { |f| Nokogiri::XML(f) }

games = []

db.root.css('game').each do |g|
  g.css('cartridge').each do |c|
    game = Game.new
    name = g.attr('name')
    region = g.attr('region')
    rev = c.attr('revision')
    game.name = name + " (" + region + ")"
    game.name.tr!('.<>:|?*', '')
    if (rev)
      game.name += " (Rev " + rev + ")"
    end
    game.name.tr!('\\/', '-')
    game.crc32 = c.attr('crc').to_s.to_i(16)
    game.region = 0
    if c.attr('system').include?('PAL')
      game.region = 1
    end

    board = c.css('board').first
    pad = c.css('pad').first

    game.mirror = 0
    game.mapper = board.attr('mapper').to_i
    if pad && pad.attr("h") == "1"
      game.mirror = 1
    end

    game.prg_rom_size = 0
    game.prg_ram_size = 0
    game.chr_rom_size = 0
    game.chr_ram_size = 0
    game.battery = 0

    prg = c.css('prg').first
    if prg
      game.prg_rom_size = convert_size(prg.attr('size'))
    end

    chr = c.css('chr').first
    if chr
      game.chr_rom_size = convert_size(chr.attr('size'))
    end

    wram = c.css('wram').first
    if wram
      game.prg_ram_size = convert_size(wram.attr('size'))
      game.battery = wram.attr('battery') ? 1 : 0
    end

    vram = c.css('vram').first
    if vram
      game.chr_ram_size = convert_size(vram.attr('size'))
    end

    games << game
  end
end

games.uniq! { |x| x.crc32 }
games.sort_by! { |x| x.crc32 }

File.open("inc/db.inc", "wb") do |f|
  games.each do |g|
    f << g.serialize
    f << "\n"
  end
end
