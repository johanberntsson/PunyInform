#!/usr/bin/env ruby
$inform_compiler = "inform"
$is_windows = (ENV['OS'] == 'Windows_NT')

$SPECIFIC_FILES = [
	"../minimal.inf",
	"../cloak.inf",
	"../library_of_horror.inf",
	"../testbench.inf",
];

class CompilationError < StandardError
  def initialize(msg="Compilation error or warning.")
    super(msg)
  end
end

def checkforfailures() 
    if $num_tests_failed > 0 then
        puts "\n#{$num_tests_failed}/#{$num_tests} TESTS FAILED!"
        exit
    end
end

def runtest(filename, version, inform_args)
    basename = File.basename(filename, ".inf")
	out_name = basename + "_test.z#{version}"
	inform_cmd = $inform_compiler + " +.  +../lib -v#{version} #{inform_args} #{filename} #{out_name}"

#    if $is_windows then
#    else
#    end

    print "#{filename}: "
    begin
        result = %x[#{inform_cmd}]
        if result =~ /\(no output\)|Warning|Error/
            puts "COMPILER OUTPUT: "
			puts result
            raise CompilationError
        end
		File.delete(out_name)
        $num_tests += 1
		puts "passed"
		return nil
    rescue Errno::ENOENT
        puts "Failed to find a specified file or directory"
        exit
    rescue CompilationError
        puts "Compilation error or warning!"
        exit
    end
end

if ARGV.length > 0 then
  $inform_compiler = ARGV[0]
end
$num_tests = 0
$num_tests_failed = 0

puts "=================="
puts "Testing v3 release"
puts "=================="
$SPECIFIC_FILES.each { |filename| runtest filename, 3, "" }
Dir["../howto/*.inf"].sort.each { |filename| runtest filename, 3, "" }
Dir["*.inf"].sort.each { |filename| runtest filename, 3, "" }

puts "\n=================="
puts "Testing v3 debug"
puts "=================="
$SPECIFIC_FILES.each { |filename| runtest filename, 3, "-D" }
Dir["../howto/*.inf"].sort.each { |filename| runtest filename, 3, "-D" }
Dir["*.inf"].sort.each { |filename| runtest filename, 3, "-D" }

puts "\n=================="
puts "Testing v5 release"
puts "=================="
$SPECIFIC_FILES.each { |filename| runtest filename, 5, "" }
Dir["../howto/*.inf"].sort.each { |filename| runtest filename, 5, "" }
Dir["*.inf"].sort.each { |filename| runtest filename, 5, "" }

puts "\n=================="
puts "Testing v5 debug"
puts "=================="
$SPECIFIC_FILES.each { |filename| runtest filename, 3, "-D" }
Dir["../howto/*.inf"].sort.each { |filename| runtest filename, 5, "-D" }
Dir["*.inf"].sort.each { |filename| runtest filename, 5, "-D" }

puts "\nALL TESTS PASSED"
