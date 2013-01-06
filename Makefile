# Makefile for Vampire project

TARGETS = testdbapi testswserver testswclient testprotocol \
			vampire vampireserver vampireclient

all: vampire vampireserver vampireclient

# vampireserver:database service
vampireserver:dbapi.o net.o protocol.o engine.o server.o   \
			  queue.o event.o cmdline.o linkedlist.o       \
			  servlistmgr.o
	gcc -o $@ $^ -ltokyocabinet

# vampireclient:database remote command line
vampireclient: client.o net.o protocol.o remotedbapi.o     \
			   linkedlist.o servlistmgr.o clouddbapi.o cmdline.o            
	gcc -o $@ $^
	@printf '#==========================================\n'
	@printf '# vampire:C/S version\n'
	@printf '# execute ./vampireserver & ./vampireclient\n'
	@printf '#==========================================\n'

# vampire:local command line version
vampire: dbapi.o cmdline.o client.o
	gcc -o $@ $^ -ltokyocabinet
	@printf '#==========================================\n'
	@printf '# vampire:local command line version\n'
	@printf '# execute ./vampire\n'
	@printf '#==========================================\n'

test: dbapi.o testdbapi.o \
	  net.o testnetserver.o testnetclient.o \
	  protocol.o testprotocol.o \
	  linkedlist.o testlinkedlist.o
	gcc -o testdbapi dbapi.o testdbapi.o -ltokyocabinet
	./testdbapi
	gcc -o testswserver net.o testnetserver.o
	gcc -o testswclient net.o testnetclient.o
	./testswserver &
	sleep 1
	./testswclient
	killall testswserver
	gcc -o testprotocol protocol.o testprotocol.o
	./testprotocol

.c.o:
	gcc -c $<

clean:
	rm -rf *.o $(TARGETS) *.bak

