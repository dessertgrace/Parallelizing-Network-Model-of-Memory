ENDLIST =

COMPFLAGS=-std=c++11 -Wall -Wextra -pedantic -DTRACE_ON -I../include

#CXXFLAGS = $(COMPFLAGS) -O0 -g -no-pie -pg
CXXFLAGS =  $(COMPFLAGS) -O3 -pg -no-pie -DNDEBUG 

#-Werror

#-DNS_THREADED -- implemented but no significant performance  gain

VPATH =  ../lib ../include

LDFLAGS = -pthread -pg -no-pie
LDPATH = -L ../lib
LDLIBS = -lutil

EXECUTABLES = \
	ns \
	columns \
	mat \
	$(ENDLIST)

all: $(EXECUTABLES)

COMMON_OBJECTS = \
	$(ENDLIST)

NS_OBJECTS = \
	NsConnection.o \
	NsGlobals.o \
	NsLayer.o \
	NsMain.o \
	NsSystem.o \
	NsTract.o \
	NsUnit.o \
	$(COMMON_OBJECTS) \
	$(ENDLIST)

COLUMNS_OBJECTS = \
	columns.o \
	$(ENDLIST)

MAT_OBJECTS = \
	mat.o \
	$(ENDLIST)

OBJECTS = \
	$(COMMON_OBJECTS) \
        $(NS_OBJECTS) \
        $(COLUMNS_OBJECTS) \
        $(MAT_OBJECTS) \
	$(ENDLIST)

ns: 	$(NS_OBJECTS) $(LDLIBS)
	$(CXX) $(LDFLAGS) $(NS_OBJECTS) $(LDPATH) $(LDLIBS) -o $@

columns:$(COLUMNS_OBJECTS) $(LDLIBS)
	$(CXX) $(LDFLAGS) $(COLUMNS_OBJECTS) $(LDPATH) $(LDLIBS) -o $@

mat:	$(MAT_OBJECTS) $(LDLIBS)
	$(CXX) $(LDFLAGS) $(MAT_OBJECTS) $(LDPATH) $(LDLIBS) -o $@

DEPS = $(subst .o,.d,$(OBJECTS))

clean:
	/bin/rm -f $(DEPS) $(OBJECTS)

veryclean: clean
	/bin/rm -f $(EXECUTABLES)

# Automated dependency management

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $*.cc -o $*.o
	$(CXX) -MM $(CXXFLAGS) $*.cc > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

-include $(OBJECTS:.o=.d)
