export LDAHOME=$(PWD)
export BINDIR=$(LDAHOME)/bin
export INCDIRS=-I $(LDAHOME)/include -I $(LDAHOME)/src -I $(LDAHOME)/src/commons -I $(LDAHOME)/src/Unigram_Model
export CXX=g++
export CXXFLAGS=$(OPTIM) $(OTHERS) $(DEBUG)
export DEBUG=-g
export OPTIM=-O0
export OTHERS=-MMD -MP
LIBS=-lprotobuf -ltbb -lcppunit -lpthread -lgflags -lglog -lIce -lIceUtil -lrt

all: compile link

compile: libraries children
	chmod +x learntopics formatter DM_Server Merge_Dictionaries Merge_Topic_Counts

children:
	$(MAKE) -C src all

link:
	$(CXX) $(CXXFLAGS) -Llib src/commons/*.o src/commons/TopicLearner/*.o src/Unigram_Model/Formatter/*.o src/Unigram_Model/TopicLearner/*.o src/commons/Server/DistributedMap.o -o learntopics $(LIBS)
	$(CXX) $(CXXFLAGS) -Llib src/commons/*.o src/commons/Formatter/*.o  src/Unigram_Model/Formatter/*.o -o formatter $(LIBS)
	$(CXX) $(CXXFLAGS) -Llib src/commons/*.o src/commons/Server/*.o src/Unigram_Model/Server/*.o src/Unigram_Model/TopicLearner/TopicCounts.o src/Unigram_Model/TopicLearner/eff_small_map.o -o DM_Server $(LIBS)
	$(CXX) $(CXXFLAGS) -Llib src/commons/*.o src/Unigram_Model/Merge/Merge_Dictionaries.o -o Merge_Dictionaries $(LIBS)
	$(CXX) $(CXXFLAGS) -Llib src/commons/*.o src/commons/Server/DistributedMap.o src/Unigram_Model/Merge/Merge_Topic_Counts.o src/Unigram_Model/TopicLearner/TopKList.o src/Unigram_Model/TopicLearner/TypeTopicCounts.o src/commons/TopicLearner/DM_Client.o src/Unigram_Model/TopicLearner/TopicCounts.o src/Unigram_Model/TopicLearner/eff_small_map.o -o Merge_Topic_Counts $(LIBS)

libraries: $(LDAHOME)/build/Ice-3.4.1/LICENSE $(LDAHOME)/lib/libIce.so

$(LDAHOME)/build/Ice-3.4.1/LICENSE: 
	./install.sh

jar: LDALibs.jar

LDALibs.jar: formatter learntopics DM_Server Merge_Dictionaries Merge_Topic_Counts Tokenizer.class
	jar cvf LDALibs.jar bin/ lib/ learntopics formatter DM_Server Merge_Dictionaries Merge_Topic_Counts Tokenizer.class

Tokenizer.class: Tokenizer.java
	javac Tokenizer.java

clean:
	-rm LDALibs.jar
	$(MAKE) -C src clean

distclean:
	-rm -rf bin/ config/ ICE_LICENSE include/ lib/ LICENSE man/ share/ slice/ build/ formatter learntopics DM_Server Merge_Dictionaries Merge_Topic_Counts LDALibs.jar dependencies/Ice-3.4.1.tar.gz

.PHONY: clean distclean children link
