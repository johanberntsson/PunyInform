#!/bin/ruby

$is_windows = (ENV['OS'] == 'Windows_NT')

def runtest(filename, version)
    basename = File.basename(filename, ".inf")
    command_file = basename + ".cmd"
    transcript_file = basename + ".scr"
    output_file = basename + ".output"
    template_file = basename + ".txt"

    if $is_windows then
        puts "Someone needs to provide Windows commands"
        exit
    else
        inform_cmd = "inform +.  +../lib -v#{version} -D #{filename}"
        frotz_cmd = "frotz #{basename}.z#{version} < #{command_file}"
        prune_cmd = "tail +6 #{transcript_file} > #{output_file}"
        diff_cmd = "diff #{template_file} #{output_file}"
    end
    # Remove old transcripts
    File.delete transcript_file if File.exist? transcript_file

    print "#{basename}: "
    begin
        result = %x[#{inform_cmd}]
        if result.include? "(no output)" then
            puts result
            raise Errno::ENOENT
        end
        result = %x[#{frotz_cmd}]
        result = %x[#{prune_cmd}]
        result = %x[#{diff_cmd}]
        if result.empty? then
            puts "passed"
        else
            puts "failed"
            puts result
        end
    rescue Errno::ENOENT
        puts "unable to run this test (compilation error?)"
        exit
    end
end

puts "Testing v3"
Dir["*.inf"].sort.each { |filename| runtest filename, 3 }
puts "Testing v5"
Dir["*.inf"].sort.each { |filename| runtest filename, 5 }
