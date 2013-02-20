# Makefile for CS 360 example code

CXX=	g++ -g
EOBJS=	epoller.o handler.o Config.o ConfigTokenizer.o HTTPRequest.o HTTPResponse.o Tokenizer.o clientdata.o client.o
OBJS=	$(EOBJS)

LIBS=	

CCFLAGS= -g

all:		epoller


epoller:	$(EOBJS)
		$(CXX) -o web $(EOBJS) $(LIBS)


clean:
		rm -f $(OBJS) $(OBJS:.o=.d)

realclean:
		rm -f $(OBJS) $(OBJS:.o=.d) web


# These lines ensure that dependencies are handled automatically.
%.d:	%.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

include	$(OBJS:.o=.d)
