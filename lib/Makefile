ENDLIST =

#CXXFLAGS = -std=c++11 -Wall -DTRACE_ON -I../include -O0  -g -pg -no-pie -DUTIL_THREADED 
CXXFLAGS = -std=c++11 -Wall -DTRACE_ON -I../include -O3 -pg -no-pie -DNDEBUG -DUTIL_THREADED 
CFLAGS = -I../include -O3 -DNDEBUG

ARFLAGS = rvU
LDFLAGS = -pg -no-pie


LIBUTIL = libutil.a
LIBS = $(LIBUTIL)

LIBUTIL_OBJECTS = \
	$(LIBUTIL)(Trace.o) \
	$(LIBUTIL)(Props.o) \
	$(LIBUTIL)(Sched.o) \
	$(LIBUTIL)(Util.o) \
	$(LIBUTIL)(format.o) \
	$(LIBUTIL)(tinyexpr.o) \
	$(ENDLIST)

LIBOBJECTS = \
	$(LIBUTIL_OBJECTS) \
	$(ENDLIST)

LDFLAGS = -pthread -pg
LDPATH = -L ../lib
LDLIBS = -lutil

all: $(LIBS)

$(LIBUTIL): $(LIBUTIL_OBJECTS)

DEPS = $(patsubst $(LIBUTIL)(%.o),%.d,$(LIBOBJECTS))

clean:
	/bin/rm -f *.d $(LIBS)

veryclean: clean

# Automated dependency management

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $*.cc -o $*.o
	$(CXX) -MM $(CXXFLAGS) $*.cc > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

#-include $(DEPS)
