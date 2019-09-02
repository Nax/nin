cycs = []

File.open(ARGV[0], "r") do |f|
    f.each_line do |line|
        cycs << /CYC:([0-9]+)/.match(line)[1].to_i
    end
end

File.open(ARGV[1], "w") do |f|
    cycs.each do |c|
        f.puts("#{c},")
    end
end
