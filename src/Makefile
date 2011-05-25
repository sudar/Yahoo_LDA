all: compile

compile: current document.pb.o children 

current: $(OBJS)

document.pb.o: commons/document.pb.cc commons/document.pb.h

commons/document.pb.cc: commons/document.pb.h

commons/document.pb.h: document.proto
	$(BINDIR)/protoc -I=. --cpp_out=commons document.proto

-include ../commons.mk
