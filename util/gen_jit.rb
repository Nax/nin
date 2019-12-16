require_relative 'core_6502'

rules = Core6502.parse_rules(__dir__ + '/../data/6502.txt')

File.open(ARGV[0], "w") do |file|
  256.times do |op|
    file << ("case 0x%02x:\n" % op)
    rules.filter{|r| r.match?(op)}.each do |r|
       file << "    X(SYMOP_#{r.symop.to_s.upcase}, #{r.size});\n"
    end
    file << "    break;\n"
  end
end
