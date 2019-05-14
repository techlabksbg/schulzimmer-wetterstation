require 'midilib'
require 'midilib/io/seqreader'

POLY = 5

# Create a new, empty sequence.
seq = MIDI::Sequence.new()

mul = 0.04*seq.ppqn.to_f/seq.bpm.to_f
p mul
# quit

# Read the contents of a MIDI file into the sequence.
File.open(ARGV[0], 'rb') { | file |
    seq.read(file) { | track, num_tracks, i |
        # Print something when each track is read.
        puts "read track #{i} of #{num_tracks}"
    }
}
    

myEv = []    

seq.each { | track |
    puts "------------------------------"
    track.each { | event |
        if MIDI::NoteEvent === event 
            if event.instance_of?(MIDI::NoteOn)
              myEv.push([event.time_from_start, event.note, event.off.time_from_start])
            end
        end
    }
}    
puts "Collected #{myEv.size} Events"
myEv.sort!{|a,b| a[0]<=>b[0]}


channels=Array.new(POLY,-1)

ev = []
myEv.each{|e|
    start = e[0]   # StartTime
    ende = e[2]    # EndTime
    note = e[1]
    channels.map!{|x| x<=start ? -1 : x}
    c = channels.index(-1)
    raise "Not enough channels" if (c==nil)
    channels[c] = ende
    ev.push([start, note,c])
    ev.push([ende, 0 , c])
}
ev.sort!{|a,b| a[0]<=>b[0]}

ev.map!{|e| [(e[0]*mul).to_i, e[1], e[2]]}
#ev.select!{|e| e[0]<28000}

code = "#define POLY #{POLY}\n"
code += "unsigned int numEv = #{ev.size};\n"
code += "unsigned long evTime[#{ev.size}] = {"+ev.map{|e| e[0].to_s}.join(",")+"};\n"
code += "unsigned char evNotes[#{ev.size}] = {"+ev.map{|e| e[1].to_s}.join(",")+"};\n"
code += "unsigned char evChannels[#{ev.size}] = {"+ev.map{|e| e[2].to_s}.join(",")+"};\n"

File.write("song.h",code);
