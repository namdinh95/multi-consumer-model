 # Name: Nam Dinh
 # Email: nvd130030@utdallas.edu
 # Course: CS 3376.502
 
CXX = g++
CXXFLAGS =
CPPFLAGS = -Wno-write-strings -Wall -g -I$(HOME)/include
LDFLAGS = -L$(HOME)/lib
LDLIBS = -lcdk -lcurses -lboost_system -lboost_thread-mt


#
# PROJECTNAME is a descriptive name used for the backup target
# This should not contain spaces or special characters

PROJECTNAME = program6

EXECFILE = program6

OBJS = program6.o


#
# We override the Implicit rule for .oc so that we 
# automatically create dependency files
#
#    n.o is made automatically from n.cc or n.C with a command of the form:
#	 $(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@
#


all: $(EXECFILE)

clean:
	rm -f $(OBJS) $(EXECFILE) *.P *~ \#*


backup:
	@mkdir -p ~/backups; chmod 700 ~/backups
	@$(eval CURDIRNAME := $(shell basename `pwd`))
	@$(eval MKBKUPNAME := ~/backups/$(PROJECTNAME)-$(shell date +'%Y.%m.%d-%H:%M:%S').tar.gz)
	@echo
	@echo Writing Backup file to: $(MKBKUPNAME)
	@echo
	@-tar zcfv $(MKBKUPNAME) ../$(CURDIRNAME)
	@chmod 600 $(MKBKUPNAME)
	@echo
	@echo Done!


#%o:%c
#	$(CC) $(CPPFLAGS) -M -MF $*P $<
#	$(CC) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

#%o:%cc
#	$(CXX) $(CPPFLAGS) -M -MF $*P $<
#	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@


$(EXECFILE): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

#
# Keep Intermediary lex file
#.PRECIOUS: scan.c

#
# This includes any dependency files that were
# created by the preprocessor

#-include $(OBJS:%.o=%.P)

