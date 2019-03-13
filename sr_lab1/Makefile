all:
	gcc client.c -o client

cli1:
	./client Adam 8080 127.0.0.1 8080 1

cli2:   
	./client Robert 8081 127.0.0.1 8080 0

cli3:   
	./client Krzysztof 8082 127.0.0.1 8081 0

cli4:   
	./client Anna 8083 127.0.0.1 8081 0

cli5:   
	./client Justyna 8084 127.0.0.1 8083 0
	
log:
	ruby logger.rb
