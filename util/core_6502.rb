class CoreRule
  attr_reader :pattern, :irq, :nmi, :size, :symop

  def initialize(pattern, irq, nmi, size, symop)
    @pattern = pattern
    @irq = (irq.to_i > 0)
    @nmi = (nmi.to_i > 0)
    @size = size.to_i
    @symop = symop.to_sym
  end

  def match?(op)
    str = "%08b" % op
    8.times.to_a.map{|i| @pattern[i] == 'x' || @pattern[i] == str[i]}.all?
  end
end

module Core6502
  def self.parse_rules(path)
    File.open(path, "r") do |f|
      rules = []
      f.each_line do |line|
        next if line[0] == '#' || line.strip.empty?
        pattern, irq, nmi, size, symop = *line.chomp.split(' ')
        rules << CoreRule.new(pattern, irq, nmi, size, symop)
      end
      rules
    end
  end
end
