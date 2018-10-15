require 'sinatra'

$DATADIR = "./"
$LOGFILE = "/var/log/loraconcentrator.log"

# {Station => [[MeasureKey, number of measures]]}
$STATIONDEFS = {1=>[['co2',2],['temp',3], ['humidity',1]]}

$SEMAPHORE = Mutex.new

def storePacket(hex)
  
end

def getLEint(hex, pos, nr)
  res = 0
  nr.times{|i|
    res |= hex[((pos+i)*2)..((pos+i)*2+1)].to_i(16)<<(i*8)
  }
  return res
end

def decodePacket(hex)
  res = {'station'=>getLEint(hex,0,2),
         'packetID'=>getLEint(hex,2,2),
         'time'=>Time.now.to_s
        }
  return false unless $STATIONDEFS[res['station']]
  pos = 2
  $STATIONDEFS[res['station']].each{|mdef|
    res[mdef[0]] = Array.new(mdef[1]){|p|
      err("Packet too short! Packet #{hex} Definition #{$STATIONDEFS[res['station']]}") if (hex.size<pos*2+4)
      getLEint(hex,pos+=2,2)
    }
  }
end
xc
def err(what)
  log(what)
  raise what
end


def log(what)
  $SEMAPHORE.synchronize {
    log = File.open($LOGFILE,"a")
    what = Time.now.to_s+","+what
    log.puts(what)
    puts what
    log.close
  }
end


def checkSum(hex)
  sum = 0
  (hex.size/2).times{|i|
    sum ^= hex[(2*i)..(2*i+1)].to_i(16)
  }
  return sum==0
end


get '/loraconcentrator/packetin/*' do
  #TODO url extraction
  hex = "deadbeefdeadbeef"
  if checkSum(hex)
    
  else
    log("Rejected packet for wrong checksum: #{hex}")
  end
end
