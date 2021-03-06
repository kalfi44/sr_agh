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

puts 'Waiting for examination orders. To exit press CTRL+C'
while true
  begin
    #sleep 10
    queues.each do |q|
      q.subscribe(block: false) do |delivery_info, properties, body|
      puts "Got test order from #{delivery_info.routing_key}: #{body}"
      message = "#{body} done"
      puts "Working on examination..."
      sleep 3
      exchange.publish(message, routing_key: "result.#{properties.reply_to}", correlation_id:  generate_uuid)
      puts "Sent #{delivery_info.routing_key}:#{message}"
    end
  end 
  rescue Interrupt => _
    channel.close
    connection.close

    exit(0)
  end
end