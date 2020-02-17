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

    rule = rule.map {|r| r.is_a?(Array) ? r.flatten : [r]}
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

def make_brk; [['AddrSet_BRK', 'AddrImplIncPC'], 'PushPCH', 'PushPCL', 'PushP', 'VectorPCL', 'VectorPCH']; end
def make_reset; [['AddrSet_RESET', 'AddrImpl'], 'DecS', 'DecS', 'DecS', 'VectorPCL', 'VectorPCH']; end

def make_store_zero(prefix); [[prefix, 'AddrZero'], 'WriteRegZero']; end
def make_store_zero_x(prefix); [[prefix, 'AddrZero'], 'AddrZeroX', 'WriteRegZero']; end
def make_store_zero_y(prefix); [[prefix, 'AddrZero'], 'AddrZeroY', 'WriteRegZero']; end
def make_store_absolute(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHi', 'WriteReg']; end
def make_store_absolute_x(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHiX', 'AddrCarry', 'WriteReg']; end
def make_store_absolute_y(prefix); [[prefix, 'AddrAbsLo'], 'AddrAbsHiY', 'AddrCarry', 'WriteReg']; end
def make_store_indirect_x(prefix); [[prefix, 'AddrZero'], 'AddrZeroX', 'AddrIndirectLo', 'AddrIndirectHi', 'WriteReg']; end
def make_store_indirect_y(prefix); [[prefix, 'AddrZero'], 'AddrIndirectLo', ['AddrIndirectHi', 'AddrIndirectY'], 'AddrCarry', 'WriteReg']; end

book = Rulebook.new
book.read_templates ARGV[0]

0x103.times do |i|
  book.add_rule(i, :kill)
end

# Vectors
book.add_rule 0x000, make_brk()
book.add_rule 0x100, make_reset()

# Flag instructions
book.add_rule 0x18, ['FlagClearC']
book.add_rule 0x58, ['FlagClearI']
book.add_rule 0xb8, ['FlagClearV']
book.add_rule 0xd8, ['FlagClearD']

book.add_rule 0x38, ['FlagSetC']
book.add_rule 0x78, ['FlagSetI']
book.add_rule 0xf8, ['FlagSetD']

# Transfer
book.add_rule 0xaa, [['AddrImpl', 'TransferAX']]
book.add_rule 0xa8, [['AddrImpl', 'TransferAY']]
book.add_rule 0xba, [['AddrImpl', 'TransferSX']]
book.add_rule 0x8a, [['AddrImpl', 'TransferXA']]
book.add_rule 0x9a, [['AddrImpl', 'TransferXS']]
book.add_rule 0x98, [['AddrImpl', 'TransferYA']]

# Immediate Loads
book.add_rule 0xa9, ['ImmLoadA']
book.add_rule 0xa2, ['ImmLoadX']
book.add_rule 0xa0, ['ImmLoadY']

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

book.optimize
#book.dump

#puts
#puts book.emit_instructions

book.emit_file ARGV[1]
