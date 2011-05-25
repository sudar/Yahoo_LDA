OBJS=$(shell $(BINDIR)/create_obj_file_targets.sh)
DEPS=$(shell $(BINDIR)/create_dep_file_targets.sh)
DIRS=$(shell $(BINDIR)/create_dir_file_targets.sh)

all: compile

compile: current children

current: $(OBJS)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCDIRS) -I . -c $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCDIRS) -I . -c $<

children:
	for d in $(DIRS); do $(MAKE) -C $$d all; done

clean:
	-rm *.o *.d
	for d in $(DIRS); do $(MAKE) -C $$d clean; done

.PHONY: clean children
