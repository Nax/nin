class Rulebook
  def initialize
    @templates = {}
    @steps = {}
    @next_step_id = 0x103
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

  def add_rule(index, rule)
    if rule == :kill
      @steps[index] = :kill
      return
    end

    rule = rule.reject(&:nil?)
    rule = rule.map {|r| r.is_a?(Array) ? r.flatten.reject(&:nil?) : [r]}
    next_step = nil
    rule[1..-1].reverse.each do |r|
      next_step = add_step(r, next_step)
    end
    @steps[index] = [rule[0], next_step]
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

    aliases.each do |k|
      @steps.delete(k)
    end
  end

  def optimize_pass
    dups = []
    @steps.each do |i, step|
      next if i < 0x103 || step == :kill
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
      next if step == :kill || step[1].nil?
      step[1] = new_mapping[step[1]]
    end

    @steps = new_steps
  end

  def ref_step(step)
    handler = nil
    if step.nil?
      handler = 'dispatch';
    else
      handler = "instruction<#{"0x%03x" % step}>"
    end
    "((Handler)&CPU::#{handler})"
  end

  def emit_step(index)
    step = @steps[index]
    tpl = nil
    if step == :kill
      tpl = "return debug_not_impl(#{"0x%03x" % index});"
      #tpl = "return #{ref_step(index)};"
    else
      next_step = ref_step(step[1]);
      tpl = (step[0].map{|x| (@templates[x] or raise StandardError, "Missing template: #{x}")} + ["return #{next_step};"]).join(" ")
    end
    str = "template<> CPU::Handler CPU::instruction<#{"0x%03x" % index}>(void) { #{tpl} }\n"
    str
  end

  def emit_instructions
    a = []
    keys = @steps.keys.sort
    keys_lo = keys.select{|x| x <= 0x102}
    keys_hi = keys - keys_lo
    (keys_hi + keys_lo).each do |k|
      a << emit_step(k)
    end
    a.join("")
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

def make_load_zero(prefix); [[prefix, 'AddrZero'], 'ReadRegZero']; end
def make_load_zero_x(prefix); [[prefix, 'AddrZero'], 'AddrZeroX', 'ReadRegZero']; end
def make_load_zero_y(prefix); [[prefix, 'AddrZero'], 'AddrZeroY', 'ReadRegZero']; end
def make_load_absolute(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHi', 'ReadReg']; end
def make_load_absolute_x(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHiX', 'ReadRegCarry', 'ReadReg']; end
def make_load_absolute_y(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHiY', 'ReadRegCarry', 'ReadReg']; end
def make_load_indirect_x(prefix); [[prefix, 'AddrZero'], 'AddrZeroX', 'AddrIndirectLo', 'AddrIndirectHi', 'ReadReg']; end
def make_load_indirect_y(prefix); [[prefix, 'AddrZero'], 'AddrIndirectLo', ['AddrIndirectHi', 'AddrIndirectY'], 'ReadRegCarry', 'ReadReg']; end

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

0x103.times do |i|
  book.add_rule(i, :kill)
end

# Vectors
book.add_rule 0x000, make_brk()
book.add_rule 0x100, make_reset()
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
book.add_rule 0xea, [['Nop', 'Nop']]

book.optimize
#book.dump

#puts
#puts book.emit_instructions

book.emit_file ARGV[1]
