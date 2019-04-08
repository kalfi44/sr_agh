require 'socket'
require 'ipaddr'
MULTICAST_ADDR = "224.1.2.3" 
PORT = 10000
ip =  IPAddr.new(MULTICAST_ADDR).hton + IPAddr.new("0.0.0.0").hton
sock = UDPSocket.new
sock.setsockopt(Socket::IPPROTO_IP, Socket::IP_ADD_MEMBERSHIP, ip)
sock.setsockopt(Socket::SOL_SOCKET, Socket::SO_REUSEADDR, 1)
sock.bind(MULTICAST_ADDR, PORT)
loop do
  msg, info = sock.recvfrom(256)
  res = msg.match(/\b\w+\b/)
  puts "MSG: #{msg} from #{info[2]} (#{info[3]})/#{info[1]} len #{msg.size}" 
end
