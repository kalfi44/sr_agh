#!/usr/bin/env ruby
require 'bunny'

abort "Usage: #{$PROGRAM_NAME} [binding key]" if ARGV.empty?

def generate_uuid
  # very naive but good enough for code examples
  "#{rand}#{rand}#{rand}"
end

connection = Bunny.new
connection.start

channel = connection.create_channel
channel.prefetch(10)
exchange = channel.topic('topic_logs')

queues = []

ARGV.each do |severity|
  queue = channel.queue("#{severity}_techs")
  queue.bind(exchange, routing_key: "*.#{severity}")
  queues.push(queue)
end

puts ' [*] Waiting for logs. To exit press CTRL+C'

begin
  #sleep 10
  queues.each do |q|
    q.subscribe(block: true) do |delivery_info, properties, body|
    puts " [x] #{delivery_info.routing_key}:#{body}"
    message = "done"
    puts "#{properties.reply_to}"
    #sleep 3
    #channel.acknowledge(delivery_info.delivery_tag, true)
    exchange.publish(message, routing_key: "result.#{properties.reply_to}", correlation_id:  generate_uuid)
    puts " [x] Sent #{delivery_info.routing_key}:#{message}"
  end
end 
rescue Interrupt => _
  channel.close
  connection.close

  exit(0)
end