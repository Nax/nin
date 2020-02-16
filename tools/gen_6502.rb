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
      tpl = "return #{ref_step(index)};"
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
    states = (0...@next_step_id).to_a.map{|x| "((CPU::Handler)&CPU::instruction<#{"0x%03x" % x}>),"}.each_slice(4).map{|x| x.join(" ")}.map{|x| "    " + x}.join("\n")
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

def make_brk; [['AddrSet_BRK', 'AddImplIncPC'], 'PushPCH', 'PushPCL', 'PushP', 'VectorPCL', 'VectorPCH']; end
def make_reset; [['AddrSet_RESET', 'AddImplIncPC'], 'DecS', 'DecS', 'DecS', 'VectorPCL', 'VectorPCH']; end

book = Rulebook.new
book.read_templates ARGV[0]

0x103.times do |i|
  book.add_rule(i, :kill)
end

book.add_rule 0x000, make_brk()
book.add_rule 0x100, make_reset()

book.dump

puts
puts book.emit_instructions

book.emit_file ARGV[1]
