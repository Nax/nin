class Rulebook
  def initialize
    @templates = {}
    @steps = {}
    @next_step_id = 0x102
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
    str = "template<> Handler CPU::instruction<#{"0x%03x" % index}>(void) { #{@templates[step[0]]} }"
    str.sub!("$1", ref_step(step[1]))
    str
  end

  def emit
    a = []
    @steps.keys.sort.each do |k|
      a << emit_step(k)
    end
    a.join("\n")
  end
end

def make_brk
  ['PushPCH', 'PushPCL', 'PushP', 'VectorPCL_BRK', 'VectorPCH_BRK']
end

book = Rulebook.new
book.read_templates ARGV[0]

book.add_rule 0, make_brk()

puts
puts book.emit
