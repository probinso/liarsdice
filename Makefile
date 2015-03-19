CC=gcc
CFLAGS=-g -Wall -I.
SHARE=handler.c player.c 
SERH= interface.c ssgame.c $(SHARE)
SERV= server.c $(SERH)
OBJS=${SERV:.c=.o}

CLIH= clientface.c ccgame.c $(SHARE)
CLIE= client.c $(CLIH)
CBJS=${CLIE:.c=.o}

default: server client

server: $(OBJS)
	$(CC) -o server $(CFLAGS) $(OBJS)

$(OBJS): types.h ${SERH:.c=.h}

client: $(CBJS)
	$(CC) -o client $(CFLAGS) $(CBJS)

$(CBJS): types.h ${CLIH:.c=.h}

archive: types.h ${SERH:.c=.h} ${CLIH:.c=.h} Makefile ${CLIE} ${SERV}
	tar -cvzf philiprobinson.tar.gz types.h ${SERH:.c=.h} ${CLIH:.c=.h} ${CLIE} ${SERV} Makefile 

clean:
	rm -f *.o *.core server client
