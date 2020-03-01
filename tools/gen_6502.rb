class Rulebook
  def initialize
    @templates = {}
    @steps = {}
    @next_step_id = 0
    @ops = {}
  end

  def read_templates(path)
    File.open(path) do |f|
      f.each_line do |line|
        line.chomp!
        unless line.empty?
          name, tpl = *line.split(" ", 2)
          @templates[name] = tpl
        end
      end
    end
  end

  def add_rule(op, rule)
    if rule == :kill
      @ops[op] = :kill
      return
    end

    rule = rule.reject(&:nil?)
    rule = rule.map {|r| r.is_a?(Array) ? r.flatten.reject(&:nil?) : [r]}
    next_step = nil
    rule.reverse.each do |r|
      next_step = add_step(r, next_step)
    end
    @ops[op] = next_step
  end

  def add_step(step, next_step_id)
    id = @next_step_id
    s = [step, next_step_id]
    @steps[id] = s
    @next_step_id += 1
    id
  end

  def optimize
    8.times { optimize_pass }
    rename_pass
  end

  def canonize(canonical, aliases)
    @steps.each do |index, step|
      next if step == :kill
      if aliases.include?(step[1])
        step[1] = canonical
      end
    end

    @ops.each do |op, index|
      if aliases.include?(index)
        @ops[op] = canonical
      end
    end

    aliases.each do |k|
      @steps.delete(k)
    end
  end

  def optimize_pass
    dups = []
    @steps.each do |i, step|
      duplicates = @steps.map{|k, v| [k, v]}.select{|x| x[1] == step}.map{|x| x[0]}
      dups.push(duplicates) if duplicates.size > 1
    end
    dups.uniq!
    dups.each do |d|
      canonize(d[0], d[1..-1])
    end
  end

  def rename_pass
    new_mapping = {}
    new_steps = {}
    new_index = 0

    @steps.each do |index, step|
      new_steps[new_index] = step
      new_mapping[index] = new_index
      new_index += 1
    end

    new_steps.each do |index, step|
      next if step[1].nil?
      step[1] = new_mapping[step[1]]
    end

    @ops.each do |k, v|
      next if v == :kill
      @ops[k] = new_mapping[v]
    end

    @steps = new_steps
  end

  def ref_step(step)
    handler = nil
    if step == :kill
      handler = 'kil'
    elsif step.nil?
      handler = 'dispatch';
    else
      handler = "instruction<#{"0x%03x" % step}>"
    end
    "((CPU::Handler)&CPU::#{handler})"
  end

  def emit_step(index)
    step = @steps[index]
    tpl = nil
    next_step = ref_step(step[1]);
    tpl = (["Handler next = #{next_step};"] + step[0].map{|x| (@templates[x] or raise StandardError, "Missing template: #{x}")} + ["return next;"]).join(" ")
    str = "template<> CPU::Handler CPU::instruction<#{"0x%03x" % index}>(void) { #{tpl} }\n"
    str
  end

  def emit_instructions
    a = []
    keys = @steps.keys.sort
    keys.each do |k|
      a << emit_step(k)
    end
    a.join("")
  end

  def emit_ops
    ops = @ops.keys.sort.map{|op| ref_step(@ops[op]) + ","}.each_slice(4).map{|x| x.join(" ")}.map{|x| "    " + x}.join("\n")
    "const CPU::Handler CPU::kOps[] = {\n#{ops}\n};\n"
  end

  def emit_states
    states = (0..@steps.keys.max).to_a.map{|x| "((CPU::Handler)&CPU::instruction<#{"0x%03x" % x}>),"}.each_slice(4).map{|x| x.join(" ")}.map{|x| "    " + x}.join("\n")
    "const CPU::Handler CPU::kStates[] = {\n#{states}\n};\n"
  end

  def emit_file(path)
    File.open(path, "w") do |f|
      f.write "#include <libnin/CPU.h>\n"
      f.write "#include <libnin/Memory.h>\n"
      f.write "\n"
      f.write "using namespace libnin;\n"
      f.write "\n"
      f.write emit_instructions
      f.write "\n"
      f.write emit_ops
      f.write "\n"
      f.write emit_states
    end
  end

  def dump
    p @steps
  end
end

def make_brk; [['AddrSet_BRK', 'AddrImplIncPC'], ['PushPCH', 'DecS'], ['PushPCL', 'DecS'], ['PushP', 'DecS', 'FlagSetI'], 'VectorPCL', 'VectorPCH']; end
def make_reset; [['AddrSet_RESET', 'AddrImpl', 'FlagSetI'], 'DecS', 'DecS', 'DecS', 'VectorPCL', 'VectorPCH']; end
def make_irq; [['AddrSet_BRK', 'AddrImpl'], ['PushPCH', 'DecS'], ['PushPCL', 'DecS'], ['PushP_NoB', 'DecS', 'FlagSetI'], 'VectorPCL', 'VectorPCH']; end
def make_nmi; [['AddrSet_NMI', 'AddrImpl'], ['PushPCH', 'DecS'], ['PushPCL', 'DecS'], ['PushP_NoB', 'DecS'], 'VectorPCL', 'VectorPCH']; end

def make_branch(cond); [['AddrZero', cond, 'BranchTake'], 'BranchTake2', 'Nop']; end

def make_store_zero(prefix); [[prefix, 'AddrZero'], 'WriteRegZero']; end
def make_store_zero_x(prefix); [[prefix, 'AddrZero'], 'AddrZeroX', 'WriteRegZero']; end
def make_store_zero_y(prefix); [[prefix, 'AddrZero'], 'AddrZeroY', 'WriteRegZero']; end
def make_store_absolute(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHi', 'WriteReg']; end
def make_store_absolute_x(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHiX', 'AddrCarry', 'WriteReg']; end
def make_store_absolute_y(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHiY', 'AddrCarry', 'WriteReg']; end
def make_store_indirect_x(prefix); [[prefix, 'AddrZero'], 'AddrZeroX', 'AddrIndirectLo', 'AddrIndirectHi', 'WriteReg']; end
def make_store_indirect_y(prefix); [[prefix, 'AddrZero'], 'AddrIndirectLo', ['AddrIndirectHi', 'AddrIndirectY'], 'AddrCarry', 'WriteReg']; end

def make_store_ax_zero(); [['AddrZero'], 'WriteRegZero_SAX']; end
def make_store_ax_zero_x(); [['AddrZero'], 'AddrZeroX', 'WriteRegZero_SAX']; end
def make_store_ax_zero_y(); [['AddrZero'], 'AddrZeroY', 'WriteRegZero_SAX']; end
def make_store_ax_absolute(); [['AddrAbsLo'], 'AddrAbsHi', 'WriteReg_SAX']; end
def make_store_ax_absolute_x(); [['AddrAbsLo'], 'AddrAbsHiX', 'AddrCarry', 'WriteReg_SAX']; end
def make_store_ax_absolute_y(); [['AddrAbsLo'], 'AddrAbsHiY', 'AddrCarry', 'WriteReg_SAX']; end
def make_store_ax_indirect_x(); [['AddrZero'], 'AddrZeroX', 'AddrIndirectLo', 'AddrIndirectHi', 'WriteReg_SAX']; end
def make_store_ax_indirect_y(); [['AddrZero'], 'AddrIndirectLo', ['AddrIndirectHi', 'AddrIndirectY'], 'AddrCarry', 'WriteReg_SAX']; end

def make_load_zero(prefix); [[prefix, 'AddrZero'], 'ReadRegZero']; end
def make_load_zero_x(prefix); [[prefix, 'AddrZero'], 'AddrZeroX', 'ReadRegZero']; end
def make_load_zero_y(prefix); [[prefix, 'AddrZero'], 'AddrZeroY', 'ReadRegZero']; end
def make_load_absolute(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHi', 'ReadReg']; end
def make_load_absolute_x(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHiX', 'ReadRegCarry', 'ReadReg']; end
def make_load_absolute_y(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHiY', 'ReadRegCarry', 'ReadReg']; end
def make_load_indirect_x(prefix); [[prefix, 'AddrZero'], 'AddrZeroX', 'AddrIndirectLo', 'AddrIndirectHi', 'ReadReg']; end
def make_load_indirect_y(prefix); [[prefix, 'AddrZero'], 'AddrIndirectLo', ['AddrIndirectHi', 'AddrIndirectY'], 'ReadRegCarry', 'ReadReg']; end

def make_load_ax_zero(); [['AddrZero'], 'ReadRegZero_AX']; end
def make_load_ax_zero_x(); [['AddrZero'], 'AddrZeroX', 'ReadRegZero_AX']; end
def make_load_ax_zero_y(); [['AddrZero'], 'AddrZeroY', 'ReadRegZero_AX']; end
def make_load_ax_absolute(); [['AddrAbsLo'], 'AddrAbsHi', 'ReadReg_AX']; end
def make_load_ax_absolute_x(); [['AddrAbsLo'], 'AddrAbsHiX', 'ReadRegCarry_AX', 'ReadReg_AX']; end
def make_load_ax_absolute_y(); [['AddrAbsLo'], 'AddrAbsHiY', 'ReadRegCarry_AX', 'ReadReg_AX']; end
def make_load_ax_indirect_x(); [['AddrZero'], 'AddrZeroX', 'AddrIndirectLo', 'AddrIndirectHi', 'ReadReg_AX']; end
def make_load_ax_indirect_y(); [['AddrZero'], 'AddrIndirectLo', ['AddrIndirectHi', 'AddrIndirectY'], 'ReadRegCarry_AX', 'ReadReg_AX']; end

def make_arith_imm(prefix, op); [[prefix, 'TmpLoadImm', op]]; end
def make_arith_zero(prefix, op); [[prefix, 'AddrZero'], ['TmpLoadZero', op]]; end
def make_arith_zero_x(prefix, op); [[prefix, 'AddrZero'], 'AddrZeroX', ['TmpLoadZero', op]]; end
def make_arith_absolute(prefix, op); [[prefix, 'AddrAbsLo'], 'AddrAbsHi', ['TmpLoad', op]]; end
def make_arith_absolute_x(prefix, op); [[prefix, 'AddrAbsLo'], 'AddrAbsHiX', ['TmpLoad', 'AddrCarryIf', op, 'AddrCarryEnd', 'AddrCarryFix'], ['TmpLoad', op]]; end
def make_arith_absolute_y(prefix, op); [[prefix, 'AddrAbsLo'], 'AddrAbsHiY', ['TmpLoad', 'AddrCarryIf', op, 'AddrCarryEnd', 'AddrCarryFix'], ['TmpLoad', op]]; end
def make_arith_indirect_x(prefix, op); [[prefix, 'AddrZero'], 'AddrZeroX', 'AddrIndirectLo', 'AddrIndirectHi', ['TmpLoad', op]]; end
def make_arith_indirect_y(prefix, op); [[prefix, 'AddrZero'], 'AddrIndirectLo', ['AddrIndirectHi', 'AddrIndirectY'], ['TmpLoad', 'AddrCarryIf', op, 'AddrCarryEnd', 'AddrCarryFix'], ['TmpLoad', op]]; end

def make_rmw_acc(op); [['AddrImpl', 'TmpLoadAcc', op, 'TmpStoreAcc']]; end
def make_rmw_zero(op); ['AddrZero', 'RmwLoadZero', ['TmpLoadRmw', op, 'TmpStoreRmw'], 'RmwStoreZero']; end
def make_rmw_zero_x(op); ['AddrZero', 'AddrZeroX', 'RmwLoadZero', ['TmpLoadRmw', op, 'TmpStoreRmw'], 'RmwStoreZero']; end
def make_rmw_absolute(op); ['AddrAbsLo', 'AddrAbsHi', 'RmwLoad', ['RmwStore', 'TmpLoadRmw', op, 'TmpStoreRmw'], 'RmwStore']; end
def make_rmw_absolute_x(op); ['AddrAbsLo', 'AddrAbsHiX', ['DummyLoad', 'CarryFix'], 'RmwLoad', ['RmwStore', 'TmpLoadRmw', op, 'TmpStoreRmw'], 'RmwStore']; end
def make_rmw_absolute_y(op); ['AddrAbsLo', 'AddrAbsHiY', ['DummyLoad', 'CarryFix'], 'RmwLoad', ['RmwStore', 'TmpLoadRmw', op, 'TmpStoreRmw'], 'RmwStore']; end
def make_rmw_indirect_x(op); ['AddrZero', 'AddrZeroX', 'AddrIndirectLo', 'AddrIndirectHi', 'RmwLoad', ['RmwStore', 'TmpLoadRmw', op, 'TmpStoreRmw'], 'RmwStore']; end
def make_rmw_indirect_y(op); ['AddrZero', 'AddrIndirectLo', ['AddrIndirectHi', 'AddrIndirectY'], ['TmpLoad', 'AddrCarry'], 'RmwLoad', ['RmwStore', 'TmpLoadRmw', op, 'TmpStoreRmw'], 'RmwStore']; end

def make_nop_impl(); ['Nop']; end
def make_nop_imm(); [['AddrImplIncPC', 'Nop']]; end
def make_nop_zero(); ['AddrImplIncPC', 'Nop']; end
def make_nop_zero_x(); ['AddrImplIncPC', 'Nop', 'Nop']; end
def make_nop_absolute(); ['AddrAbsLo', 'AddrAbsHi', 'ReadAddr']; end
def make_nop_absolute_x(); ['AddrAbsLo', 'AddrAbsHiX', ['ReadAddrCarry', 'CarryFix'], 'ReadAddr']; end

book = Rulebook.new
book.read_templates ARGV[0]

def build_arith_block(book, base, prefix, op)
  book.add_rule base + 0x09, make_arith_imm(prefix, op)
  book.add_rule base + 0x05, make_arith_zero(prefix, op)
  book.add_rule base + 0x15, make_arith_zero_x(prefix, op)
  book.add_rule base + 0x0d, make_arith_absolute(prefix, op)
  book.add_rule base + 0x1d, make_arith_absolute_x(prefix, op)
  book.add_rule base + 0x19, make_arith_absolute_y(prefix, op)
  book.add_rule base + 0x01, make_arith_indirect_x(prefix, op)
  book.add_rule base + 0x11, make_arith_indirect_y(prefix, op)
end

def build_rmw_noacc_block(book, base, op)
  book.add_rule base + 0x06, make_rmw_zero(op)
  book.add_rule base + 0x16, make_rmw_zero_x(op)
  book.add_rule base + 0x0e, make_rmw_absolute(op)
  book.add_rule base + 0x1e, make_rmw_absolute_x(op)
end

def build_rmw_block(book, base, op)
  book.add_rule base + 0x0a, make_rmw_acc(op)
  build_rmw_noacc_block(book, base, op)
end

def build_extended_rmw_block(book, base, op)
  book.add_rule base + 0x07, make_rmw_zero(op)
  book.add_rule base + 0x17, make_rmw_zero_x(op)
  book.add_rule base + 0x0f, make_rmw_absolute(op)
  book.add_rule base + 0x1f, make_rmw_absolute_x(op)
  book.add_rule base + 0x1b, make_rmw_absolute_y(op)
  book.add_rule base + 0x03, make_rmw_indirect_x(op)
  book.add_rule base + 0x13, make_rmw_indirect_y(op)
end

0x103.times do |i|
  book.add_rule(i, :kill)
end

# Vectors
book.add_rule 0x000, make_brk()
book.add_rule 0x100, make_reset()
book.add_rule 0x101, make_irq()
book.add_rule 0x102, make_nmi()

# Stack
book.add_rule 0x08, ['AddrImpl', ['PushP', 'DecS']]
book.add_rule 0x28, ['AddrImpl', 'IncS', 'PullP']
book.add_rule 0x48, ['AddrImpl', ['PushA', 'DecS']]
book.add_rule 0x68, ['AddrImpl', 'IncS', 'PullA']

# Jumps
book.add_rule 0x20, ['AddrZero', 'Nop', ['PushPCH', 'DecS'], ['PushPCL', 'DecS'], 'SwitchPC']
book.add_rule 0x40, ['AddrImpl', 'IncS', ['PullP', 'IncS'], ['PullPCL', 'IncS'], 'PullPCH']
book.add_rule 0x60, ['AddrImpl', 'IncS', ['PullPCL', 'IncS'], 'PullPCH', ['AddrImpl', 'IncPC']]

book.add_rule 0x4c, ['AddrZero', 'SwitchPC']
book.add_rule 0x6c, ['AddrAbsLo', 'AddrAbsHi', 'VectorPCL', 'VectorPCH_NoCarry']

# Flag instructions
book.add_rule 0x18, ['FlagClearC']
book.add_rule 0x58, ['FlagClearI']
book.add_rule 0xb8, ['FlagClearV']
book.add_rule 0xd8, ['FlagClearD']

book.add_rule 0x38, ['FlagSetC']
book.add_rule 0x78, ['FlagSetI']
book.add_rule 0xf8, ['FlagSetD']

# Branches
book.add_rule 0x10, make_branch('BranchClearN')
book.add_rule 0x30, make_branch('BranchSetN')
book.add_rule 0x50, make_branch('BranchClearV')
book.add_rule 0x70, make_branch('BranchSetV')
book.add_rule 0x90, make_branch('BranchClearC')
book.add_rule 0xb0, make_branch('BranchSetC')
book.add_rule 0xd0, make_branch('BranchClearZ')
book.add_rule 0xf0, make_branch('BranchSetZ')

# Transfer
book.add_rule 0xaa, [['AddrImpl', 'TransferAX']]
book.add_rule 0xa8, [['AddrImpl', 'TransferAY']]
book.add_rule 0xba, [['AddrImpl', 'TransferSX']]
book.add_rule 0x8a, [['AddrImpl', 'TransferXA']]
book.add_rule 0x9a, [['AddrImpl', 'TransferXS']]
book.add_rule 0x98, [['AddrImpl', 'TransferYA']]

# Store
book.add_rule 0x85, make_store_zero('SelectSourceA')
book.add_rule 0x95, make_store_zero_x('SelectSourceA')
book.add_rule 0x8d, make_store_absolute('SelectSourceA')
book.add_rule 0x9d, make_store_absolute_x('SelectSourceA')
book.add_rule 0x99, make_store_absolute_y('SelectSourceA')
book.add_rule 0x81, make_store_indirect_x('SelectSourceA')
book.add_rule 0x91, make_store_indirect_y('SelectSourceA')

book.add_rule 0x86, make_store_zero('SelectSourceX')
book.add_rule 0x96, make_store_zero_y('SelectSourceX')
book.add_rule 0x8e, make_store_absolute('SelectSourceX')

book.add_rule 0x84, make_store_zero('SelectSourceY')
book.add_rule 0x94, make_store_zero_x('SelectSourceY')
book.add_rule 0x8c, make_store_absolute('SelectSourceY')

# Load
book.add_rule 0xa9, ['ImmLoadA']
book.add_rule 0xa5, make_load_zero('SelectDestA')
book.add_rule 0xb5, make_load_zero_x('SelectDestA')
book.add_rule 0xad, make_load_absolute('SelectDestA')
book.add_rule 0xbd, make_load_absolute_x('SelectDestA')
book.add_rule 0xb9, make_load_absolute_y('SelectDestA')
book.add_rule 0xa1, make_load_indirect_x('SelectDestA')
book.add_rule 0xb1, make_load_indirect_y('SelectDestA')

book.add_rule 0xa2, ['ImmLoadX']
book.add_rule 0xa6, make_load_zero('SelectDestX')
book.add_rule 0xb6, make_load_zero_y('SelectDestX')
book.add_rule 0xae, make_load_absolute('SelectDestX')
book.add_rule 0xbe, make_load_absolute_y('SelectDestX')

book.add_rule 0xa0, ['ImmLoadY']
book.add_rule 0xa4, make_load_zero('SelectDestY')
book.add_rule 0xb4, make_load_zero_x('SelectDestY')
book.add_rule 0xac, make_load_absolute('SelectDestY')
book.add_rule 0xbc, make_load_absolute_x('SelectDestY')

# Arith
build_arith_block(book, 0x00, 'SelectDestA', 'OpORA')
build_arith_block(book, 0x20, 'SelectDestA', 'OpAND')
build_arith_block(book, 0x40, 'SelectDestA', 'OpEOR')
build_arith_block(book, 0xc0, 'SelectDestA', 'OpCMP')
build_arith_block(book, 0x60, 'SelectDestA', 'OpADC')
build_arith_block(book, 0xe0, 'SelectDestA', 'OpSBC')

# Compare-Index
[['SelectDestX', 0xe0], ['SelectDestY', 0xc0]].each do |b|
  prefix = b[0]
  base = b[1]
  book.add_rule base + 0x00, make_arith_imm(prefix, 'OpCMP')
  book.add_rule base + 0x04, make_arith_zero(prefix, 'OpCMP')
  book.add_rule base + 0x0c, make_arith_absolute(prefix, 'OpCMP')
end

# RMW
build_rmw_block book, 0x00, 'OpASL'
build_rmw_block book, 0x20, 'OpROL'
build_rmw_block book, 0x40, 'OpLSR'
build_rmw_block book, 0x60, 'OpROR'

build_rmw_noacc_block book, 0xe0, 'OpINC'
build_rmw_noacc_block book, 0xc0, 'OpDEC'

# Bit
book.add_rule 0x24, make_arith_zero(nil, 'OpBIT')
book.add_rule 0x2c, make_arith_absolute(nil, 'OpBIT')

# Misc
book.add_rule 0xe8, ['INX']
book.add_rule 0xc8, ['INY']
book.add_rule 0xca, ['DEX']
book.add_rule 0x88, ['DEY']

# Official NOP
book.add_rule 0xea, make_nop_impl()

# Unofficial SBC
book.add_rule 0xeb, make_arith_imm('SelectDestA', 'OpSBC')

# Unofficial NOPs
book.add_rule 0x1a, make_nop_impl()
book.add_rule 0x3a, make_nop_impl()
book.add_rule 0x5a, make_nop_impl()
book.add_rule 0x7a, make_nop_impl()
book.add_rule 0xda, make_nop_impl()
book.add_rule 0xfa, make_nop_impl()

book.add_rule 0x80, make_nop_imm()
book.add_rule 0x82, make_nop_imm()
book.add_rule 0x89, make_nop_imm()
book.add_rule 0xc2, make_nop_imm()
book.add_rule 0xe2, make_nop_imm()

book.add_rule 0x04, make_nop_zero()
book.add_rule 0x44, make_nop_zero()
book.add_rule 0x64, make_nop_zero()

book.add_rule 0x14, make_nop_zero_x()
book.add_rule 0x34, make_nop_zero_x()
book.add_rule 0x54, make_nop_zero_x()
book.add_rule 0x74, make_nop_zero_x()
book.add_rule 0xd4, make_nop_zero_x()
book.add_rule 0xf4, make_nop_zero_x()

book.add_rule 0x0c, make_nop_absolute()

book.add_rule 0x1c, make_nop_absolute_x()
book.add_rule 0x3c, make_nop_absolute_x()
book.add_rule 0x5c, make_nop_absolute_x()
book.add_rule 0x7c, make_nop_absolute_x()
book.add_rule 0xdc, make_nop_absolute_x()
book.add_rule 0xfc, make_nop_absolute_x()

# Unofficial immediates
book.add_rule 0x0b, make_arith_imm(nil, 'OpANC')
book.add_rule 0x2b, make_arith_imm(nil, 'OpANC')
book.add_rule 0x4b, make_arith_imm(nil, 'OpALR')
book.add_rule 0x6b, make_arith_imm(nil, 'OpARR')
book.add_rule 0x8b, make_arith_imm(nil, 'OpXAA')
book.add_rule 0xab, make_arith_imm(nil, 'OpLAX')
book.add_rule 0xcb, make_arith_imm(nil, 'OpAXS')

# Unofficial RMW
build_extended_rmw_block book, 0x00, 'OpSLO'
build_extended_rmw_block book, 0x20, 'OpRLA'
build_extended_rmw_block book, 0x40, 'OpSRE'
build_extended_rmw_block book, 0x60, 'OpRRA'
build_extended_rmw_block book, 0xc0, 'OpDCP'
build_extended_rmw_block book, 0xe0, 'OpISC'

# Store HiAddr
book.add_rule 0x9e, ['AddrAbsLo', 'AddrAbsHiY', 'AddrCarry_SHX', 'WriteReg_SHX'];
book.add_rule 0x9c, ['AddrAbsLo', 'AddrAbsHiX', 'AddrCarry_SHY', 'WriteReg_SHY'];

# SAX
book.add_rule 0x87, make_store_ax_zero()
book.add_rule 0x97, make_store_ax_zero_y()
book.add_rule 0x8f, make_store_ax_absolute()
book.add_rule 0x83, make_store_ax_indirect_x()

# LAX
book.add_rule 0xa7, make_load_ax_zero()
book.add_rule 0xb7, make_load_ax_zero_y()
book.add_rule 0xaf, make_load_ax_absolute()
book.add_rule 0xbf, make_load_ax_absolute_y()
book.add_rule 0xa3, make_load_ax_indirect_x()
book.add_rule 0xb3, make_load_ax_indirect_y()

book.optimize
#book.dump

#puts
#puts book.emit_instructions

book.emit_file ARGV[1]
