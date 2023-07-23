require "csv"

csv = CSV.read("tools/colors.csv")

csv.each do |c|
    /^rgb\((?<r>\d+), (?<g>\d+), (?<b>\d+)\)$/ =~ c[2]
    puts "#define glColor#{c[0]} (GLcolor){ .r = #{r}, .g = #{g}, .b = #{b}, .a = 255 }"
end

puts

csv.each do |c|
    /^rgb\((?<r>\d+), (?<g>\d+), (?<b>\d+)\)$/ =~ c[2]
    name = c[0].split(/(?=[A-Z])/).map { |s| s.upcase }.join("_")
    puts "#define GL_COLOR_#{name} #{(r.to_i / 255.0).round 2}, #{(g.to_i / 255.0).round 2}, #{(b.to_i / 255.0).round 2}, 1"
end
