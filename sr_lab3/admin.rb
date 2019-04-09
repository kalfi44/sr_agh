#!/usr/bin/env ruby
require 'bunny'

def generate_uuid
  # very naive but good enough for code examples
  "#{rand}#{rand}#{rand}"
end

connection = Bunny.new
connection.start

recv = ARGV[0]
info = ARGV[1]

channel = connection.create_channel
channel.prefetch(10)
exchange = channel.topic('topic_logs')

queues = []

queue = channel.queue("admin")
queue.bind(exchange, routing_key: "\#")

q1 = channel.queue("knee_techs")
q2 = channel.queue("hip_techs")
q3 = channel.queue("elbow_techs")


puts ' [*] Waiting for logs. To exit press CTRL+C'

begin
      queue.subscribe(block: true, manual_ack: false) do |delivery_info, properties, body|
      puts " [x] #{delivery_info.routing_key}:#{body}"
      #channel.acknowledge(delivery_info.delivery_tag, true)
      end 
rescue Interrupt => _
    channel.close
    connection.close

    exit(0)
end
