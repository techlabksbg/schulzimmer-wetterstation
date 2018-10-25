# coding: utf-8
require 'serialport'
require 'net/http'
require 'io/wait'
require 'sinatra'

set :bind, '0.0.0.0'


$SERVER = "www.tech-lab.ch"
$PORT = 4568
$URL = "/loraconcentrator/packetin/"
$LOGFILE = "relais.log"

if ENV['USER']=="ivo"
  $SERVER = "localhost"
  $LOGFILE = "relais.log"
end


$SEMAPHORE = Mutex.new

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
    @rssi = nil
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


  def reset()
    ok = false
    begin
      @sp.dtr=0;
      sleep(0.01)
      @sp.dtr=1;
      ok = true
    rescue StandardError => e  
      log("Reset failed!")
      log(e.message)
    end
    return ok
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

  # Packet now directly hex-Code, starting with
  # "-->", ending with "\r\n"
  def checkForPacket()
    return false unless @sp.ready?()
    c = @sp.getc
    return false unless c
    # log("About to receive a packet of size #{c.ord}")
    # hex = ""
    line = c
    b = nil
    while (line.size==0 || line[-1]!="\n")
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
        line += b
    end
    log("[IN] #{line}")
    if (line[0..2]=="-->")
        hex = line[3..-3]
        @packets.push(hex)
        return hex
    else
      @rssi = line.scan(/rssi=(-\d+)/)[0]
      @rssi = "?rssi=#{@rssi[0]}" if @rssi
    end
    return false
  end

  def getPackets()
    return @packets.join("\r\n")
  end

  # Waits for a packet and forwards it to the server
  def loop()
    lastsuccess = Time.now
    while true
      hex = checkForPacket()
      if hex
        lastsuccess = Time.now
        begin
          # HTTP acces
          uri = URI("http://#{$SERVER}:#{$PORT}#{$URL}#{hex}#{@rssi}")
          res = Net::HTTP.get_response(uri)
          if (res.body=="OK")
            log("   [OK]  Sucessfully submitted to server")
          else
            log(" [WTF?] Server says #{res.body} #{res.inspect}")
          end
        rescue StandardError => e  
          log(e.message)
          # log(e.backtrace.inspect)          
        end
      end
      if (Time.now-lastsuccess>200) # Nichts gehoehrt waehrend 200 Sek?
        log("Timeout on LORA/ESP32, reset EPS32...")
        reset()
        lastsuccess = Time.now
      end
      sleep(0.01)
    end # while
  end
end

r = Relais.new(ARGV[0])

if ARGV[1]
  r.reset()
  exit
end

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

get "/reset" do
  return r.reset() ? "Reset OK" : "Reset failed :-("
end


# Set Channel
# TODO: Parameterübergabe
put "/channel/:setting" do |setting|
   # parse setting, call method
  return "Not implemented yet"
end


=begin
# Routes may also utilize query parameters:

get '/posts' do
  # matches "GET /posts?title=foo&author=bar"
  title = params['title']
  author = params['author']
  # uses title and author variables; query is optional to the /posts route
end

=end
