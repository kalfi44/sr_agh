#!/usr/bin/env ruby
require 'bunny'

def generate_uuid
  # very naive but good enough for code examples
  "#{rand}#{rand}#{rand}"
end

connection = Bunny.new
connection.start

channel = connection.create_channel
channel.prefetch(10)
exchange = channel.topic('topic_logs')
#severity = ARGV.shift || 'anonymous.info'
message = "AAAA"#ARGV.empty? ? '' : ARGV.join(' ')""

name = ARGV[0]

queue = channel.queue(name)

queue.bind(exchange, routing_key: "result.#{name}.\#")

begin
  queue.subscribe(block: false) do |delivery_info, _properties, body|
    puts " [x] #{delivery_info.routing_key}:#{body}"
  end
rescue Interrupt => _
  channel.close
  connection.close

  exit(0)
end

if ARGV.length > 1  
	severity = ARGV[1]
	patient = ARGV[2]


	message = "#{patient} - #{severity}"
	exchange.publish(message,
		routing_key: "#{name}.#{severity}",
		reply_to: name,
		correlation_id: generate_uuid)
	puts " [x] Sent #{name}:#{message}"
end

connection.close