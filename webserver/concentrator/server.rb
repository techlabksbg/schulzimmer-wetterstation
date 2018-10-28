require 'fileutils'
require 'sinatra'
require 'mqtt'

set :port, 4568
set :bind, '0.0.0.0'

$APPDIR = "/home/techlab/schulzimmer-wetterstation/"
$LOGFILE = $APPDIR+"concentrator.log"
$TEMPFILE = $APPDIR+"temp.json"
$DATADIR = "/var/www/html/schulzimmer-wetterstation/"
$MQTTURI = File.exist?($APPDIR+"mqtt.uri") ? File.read($APPDIR+"mqtt.uri").chomp : "mqtt://localhost"
if ENV['USER']=="ivo"
  $DATADIR = "./"
  $LOGFILE = "loraconcentrator.log"
  $TEMPFILE = "temp.json"
end

# {Station => [[MeasureKey, number of measures]]}
$STATIONDEFS = {1=>{"name"=>'h21',
                    "measures"=>[['co2',2],['tvoc', 1],['temp',3], ['humidity',1]],                   
                   }
               }

$CONVERTERS ={'temp'=>lambda{|x| x*0.1-273.0},
              'humidity'=>lambda{|x| x*0.01}
             }

# Catch bogous value due to transmission errors with
# per chance correct check sum
$VALIDATORS = {'temp'=>lambda{|x| x>=0.0 && x<=40},
               'humidity'=>lambda{|x| x>=10.0 && x<=90.0},
               'co2'=>lambda{|x| x>350 && x<=5000},
               'tvoc'=>lambda{|x| x>=0 && x<=3000}
              }

$SEMAPHORE = Mutex.new


def toJSON(packet)
  json = "{\n  \"timestamp\":#{packet['timestamp'].to_s.inspect},\n"
  json +="  \"rssi\":#{packet['rssi']},\n"
  entries = []
  packet.keys.each{|k|
    unless %w(station packetID hex room timestamp rssi).include?(k)
      entries.push("  \"#{k}\": [#{packet[k].map{|x| x.is_a?(Float) ? "%.2f" % x : x.to_s}.join(',')}]")
    end
  }
  json += entries.join(",\n")
  json += "\n}"
  return json
end

def storePacket(packet)
  room = packet['room']
  date = packet['timestamp'].strftime("%Y-%m-%d")
  file = "#{$DATADIR}#{room}/#{date}.json"
  dir = File.dirname(file)
  FileUtils.mkdir_p(dir)
  contents=""
  if File.exists?(file)
    contents = File.read(file)
  else
    contents = "[\n#{toJSON(packet)}\n]\n"
  end
  contents = contents[0..-4]+",\n"+toJSON(packet)+"\n]\n"
  File.write($TEMPFILE,contents)
  File.rename($TEMPFILE,file)
  log("FILE=#{file}")
end

def mqtt(packet)
  log("Publish to mqtt")
  client = MQTT::Client.connect($MQTTURI) {|c|
    %w(co2 tvoc temp humidity).each{|key|
      c.publish("rooms/#{packet['room']}/#{key}", packet[key][0])
      #log("rooms/#{packet['room']}/#{key}, "+packet[key][0].to_s)
    }
  }
end

def getLEint(hex, pos, nr)
  res = 0
  nr.times{|i|
    res |= hex[((pos+i)*2)..((pos+i)*2+1)].to_i(16)<<(i*8)
  }
  return res
end

# Returns false, if values do not validate
def decodePacket(hex,rssi=nil)
  res = {'station'=>getLEint(hex,0,2),
         'packetID'=>getLEint(hex,2,2),
         'timestamp'=>Time.now,
         'hex'=>String.new(hex)
        }
  return false unless $STATIONDEFS[res['station']]
  res['room']=$STATIONDEFS[res['station']]['name']
  res['rssi']=rssi if rssi
  pos = 2
  allvalid = true
  $STATIONDEFS[res['station']]['measures'].each{|mdef|
    res[mdef[0]] = Array.new(mdef[1]){|p|
      err("Packet too short! Packet #{hex} Definition #{$STATIONDEFS[res['station']]}") if (hex.size<pos*2+4)
      val = getLEint(hex,pos+=2,2)
      if $CONVERTERS[mdef[0]]
        val = $CONVERTERS[mdef[0]].call(val)        
      end
      if $VALIDATORS[mdef[0]]
        allvalid &&= $VALIDATORS[mdef[0]].call(val)
        # log("after validating #{mdef[0]}: allvalid=#{allvalid}")
      end
      val                               
    }
  }
  if allvalid
    return res
  else
    return false
  end
end

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


get '/loraconcentrator/packetin/:hex' do
  hex = params['hex']
  rssi = params['rssi']
  if checkSum(hex)
    # log("Got OK packet #{hex}")
    p = decodePacket(hex,rssi)
    if p
      j = toJSON(p)
      log(p.inspect)
      # log(j)
      storePacket(p)
      mqtt(p)
      return "OK"
    else
      log("Bogous values, packet rejected")
      return "Checksum ok, values out of range (double transmission error)"
    end
  else
    log("Rejected packet for wrong checksum: #{hex}")
    return "Wrong checksum"
  end
end


