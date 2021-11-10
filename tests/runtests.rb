#!/bin/ruby

$is_windows = (ENV['OS'] == 'Windows_NT')

def checkforfailures() 
    if $num_tests_failed > 0 then
        puts "\n#{$num_tests_failed}/#{$num_tests} TESTS FAILED!"
        exit
    end
end

def runtest(filename, version, inform_args)
    basename = File.basename(filename, ".inf")
    command_file = basename + ".cmd"
    compileroutput_file = basename + "_z#{version}.messages"
    transcript_file = basename + ".scr"
    output_file = basename + ".output"
    template_file = basename + ".txt"

    if $is_windows then
        puts "Someone needs to provide Windows commands"
        exit
    else
        # use specific template file is available
        specific_template = "#{basename}.z#{version}.txt"
        template_file = specific_template if File.exists? specific_template
        inform_cmd = "inform +.  +../lib -v#{version} #{inform_args} #{filename}"
        frotz_cmd = "frotz #{basename}.z#{version} < #{command_file}"
        prune_cmd = "tail -n +6 #{transcript_file} | grep -v PunyInform > #{output_file}"
        diff_cmd = "diff -Z #{template_file} #{output_file}"
        diff_cmd_ignore_banner = "diff -ZB #{template_file} #{output_file} | grep -v PunyInform"
    end
    # Remove old transcripts
    File.delete transcript_file if File.exist? transcript_file

    print "#{basename}: "
    begin
        result = %x[#{inform_cmd}]
        File.open(compileroutput_file, 'w') { |file| file.write(result) }
        if result.include?("(no output)") || result.include?("Warning") then
            puts result
            raise Errno::ENOENT
        end
        $num_tests += 1
        result = %x[#{frotz_cmd}]
        result = %x[#{prune_cmd}]
        result = %x[#{diff_cmd_ignore_banner}]
        if result.empty? then
            puts "passed"
        else
            puts "failed"
            $num_tests_failed += 1
            puts %x[#{diff_cmd}]
        end
    rescue Errno::ENOENT
        puts "unable to run this test (compilation error?)"
        exit
    end
end

$num_tests = 0
$num_tests_failed = 0

puts "Testing v3 debug"
Dir["*.inf"].sort.each { |filename| runtest filename, 3, "-D" }
checkforfailures

puts "Testing v3 release"
Dir["*.inf"].sort.each { |filename| runtest filename, 3, "" }
checkforfailures

puts "Testing v5 debug"
Dir["*.inf"].sort.each { |filename| runtest filename, 5, "-D" }
checkforfailures

puts "Testing v5 release"
Dir["*.inf"].sort.each { |filename| runtest filename, 5, "" }
checkforfailures

puts "\nALL TESTS PASSED"
