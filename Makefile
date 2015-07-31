CPPFLAGS=-g -D_REENTRANT -pthread
LDFLAGS=-g
LDLIBS=-L -lpthread -ljansson -lzlog
collector.exe: 
	    gcc $(CPPFLAGS) -o collector.exe sflow_elk/sflow_json_encoder.c sflow_elk/json_logstash.c rr_sflow_collector.c $(LDLIBS) 
clean:
		rm collector.exe collector.log
