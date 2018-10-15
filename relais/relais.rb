# coding: utf-8
require 'serialport'
require 'net/http'

$SERVERURL = "http://www.tech-lab.ch/loraconcentrator/packetin/"
$LOGFILE = "relais.log"

class Relais
  def initialize(port=nil)
    @packets=[]
    @ports = port ? [port] : Array.new(8){|i| "/dev/ttyUSB#{i}"}.reverse
    @ports.each {|dev|
      if File.exist?(dev)
        @sp = SerialPort.new(dev, 115200)
        break if @sp
      end
    }
    unless @sp      
      err("Could not open Serial Port! Tried #{@ports.inspect}")
    end
  end

  
  def err(what)
    log(what)
    raise what
  end
  
  def log(what)
    # TODO: Add Mutex for this block!
    log = File.open($LOGFILE,"a")
    what = Time.now.to_s+","+what
    log.puts(what)
    puts what
    log.close
  end


  def reset()
    # TODO: Do some serial port magic to hardware reset ESP32
    # Fallback: Send some reset code for software reset
  end

  def setChannel(freq, rate, spread)
    msg = "SET #{freq} #{rate} #{spread}\r\n"
    @sp.puts(msg)
    log("Setting channel: #{msg}")    
  end

  def getChannel()
    @sp.puts("GET\r\n")
    line = @sp.gets
    unless line
      log("Getting channel: no respones from ESP32")
    end
    res = line.split(" ").map{|e| e.to_i}
    log("Getting channel: #{res.inspect}")
    return res
  end

  # Packet: First Byte is length, then contents of said length
  # TODO: NEW: Packet now directly hex-Code, starting with
  # "-->", ending with "\r\n"
  def checkForPacket()
    c = @sp.getc
    return false unless c
    log("About to receive a packet of size #{c.ord}")
    hex = ""
    c.ord.times{|i|
      b = nil
      # Wait for next byte
      timeout = 1000
      while (!(b=@sp.getc))
        sleep(0.001)
        timeout-=1
        if (timeout==0)
          log("Timeout receiving packet")
          return false
        end
      end
      # Append to hexString
      hex += "%02x" % (b.ord)
    }
    log("Received packet #{hex}")
    @packets.push(Time.now.to_s+","+hex)
    return hex
  end

  def getPackets()
    return @packets.join("\r\n")
  end

  # Waits for a packet and forwards it to the server
  def loop()
    while true
      hex = checkForPacket()
      if hex
        # HTTP acces
        url = "#{$SERVERURL}#{hex}"
        res = Net::HTTP.get(url)
        log("#{url} => #{res}")
      end
      sleep(0.01)
    end
  end
end

r = Relais.new()

Thread.new() {
  r.loop()
}


# REST Interface
require 'sinatra'

get "/packets" do
  return r.getPackets()
end

get "/channel" do
  return r.getChannel()
end


# Set Channel
# TODO: Parameterübergabe
put "/channel/:setting" do |setting|
   # parse setting, call method
  return "Not implemented yet"
end



# Routes may also utilize query parameters:

get '/posts' do
  # matches "GET /posts?title=foo&author=bar"
  title = params['title']
  author = params['author']
  # uses title and author variables; query is optional to the /posts route
end
