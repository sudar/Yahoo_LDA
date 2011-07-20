/*******************************************************************************
    Copyright (c) 2011 Yahoo! Inc. All rights reserved.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License. See accompanying LICENSE file.

    The Initial Developer of the Original Code is Shravan Narayanamurthy.
******************************************************************************/
/*
 * Merge_Dictionaries.cpp
 *
 *  Created on: 31-Jan-2011
 *      
 */

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "WordIndexDictionary.h"
#include "TopicLearner/TypeTopicCounts.h"
#include "TopicLearner/Synchronizer_Helper.h"
#include "TopicLearner/DM_Client.h"

using namespace std;

DEFINE_int32(clientid,-1,"The id that this client should use and means that this is now a part of the multi-machine setup");
DEFINE_string(servers,"specify","The set of all memcached servers that are storing the state. E.g. 192.168.0.1, 192.168.0.3:44, 200.132.12.34");
DEFINE_string(outputprefix,"lda", "A prefix that will be used with all files output by the program");
DEFINE_string(globaldictionary,"specify","The global dictionary; topic counts of parts of which have to be retrieved");
DEFINE_int32(topics,100,"The number of topics to be used by LDA.")
;

int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    string
            usage(
                    "\n\n This program takes a global dictionary dump and retrieves topic counts from global table\n"
                        " for this client's part of words and dumps those counts to disk.\n"
                        " The dumps of all clients taken together represents the global table\n");

    string
            cmd_usage(
                    (string) argv[0]
                            + " --globaldictionary=<dump of global dictionary> --clientid=<client id>\n"
                                " --servers=<servers hosting the topic counts> [--outputprefix=<outputprefix>]");

    usage += cmd_usage;

    google::SetUsageMessage(usage);
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (strcmp(argv[i], "--help") == 0) {
            google::ShowUsageWithFlagsRestrict(argv[0], "Merge_Topic");
            exit(0);
        }
    }

    google::ParseCommandLineFlags(&argc, &argv, true);

    google::SetCommandLineOptionWithMode("minloglevel", "1",
            google::SET_FLAG_IF_DEFAULT);
    google::SetCommandLineOptionWithMode("stderrthreshold", "1",
            google::SET_FLAG_IF_DEFAULT);

    const char* pwd = google::StringFromEnv("PWD", "/tmp");
    google::SetCommandLineOptionWithMode("log_dir", pwd,
            google::SET_FLAG_IF_DEFAULT);

    LOG(WARNING)
            << "----------------------------------------------------------------------";
    LOG(WARNING) << "Log files are being stored at " << pwd
            << "/formatter.*";
    LOG(WARNING)
            << "----------------------------------------------------------------------";

    string flagsInp = google::CommandlineFlagsIntoString();

    LOG(INFO) << flagsInp << endl;

    LOG_IF(FATAL,
            google::GetCommandLineFlagInfoOrDie("clientid").is_default
            || google::GetCommandLineFlagInfoOrDie("servers").is_default)<<"You need to specify this client's id and the servers storing the global table";

    LOG_IF(FATAL,google::GetCommandLineFlagInfoOrDie("globaldictionary").is_default)
    << "You need to specify the global dictionary to be used for retrieving the global topic counts";

    LOG(WARNING) << "Initializing part " << FLAGS_clientid
    << " of global Word-Topic counts table from memcached";

    WordIndexDictionary* global_dict = new WordIndexDictionary();
    string global_dict_dump = FLAGS_globaldictionary;

    LOG(WARNING) << "Initializing global dictionary from " << global_dict_dump;
    global_dict->initialize_from_dump(global_dict_dump);
    LOG(WARNING) << "global dictionary Initialized";

    int num_words = global_dict->get_num_words();
    Synchronizer_Helper* sync_helper = NULL;
    DM_Client client(num_words,FLAGS_servers,*sync_helper);
    int num_words_per_client = num_words / client.get_num_servers();
    int from = FLAGS_clientid * num_words_per_client;
    int to = (FLAGS_clientid < client.get_num_servers() - 1) ? from
    + num_words_per_client : num_words;
    TypeTopicCounts* ttc = new TypeTopicCounts(to - from, FLAGS_topics);

    for(int word_id=from; word_id<to;word_id++) {
        string word = global_dict->get_word(word_id);
        string counts;
        client.get(word,counts);
        ttc->initialize_from_string(word_id-from,counts);
    }

    string ttc_dump = FLAGS_outputprefix+".ttc.dump";
    LOG(WARNING) << "Part-" << FLAGS_clientid << " of global Word-Topic counts table initialized";
    LOG(WARNING) << "Saving it to " << ttc_dump;
    ttc->dump(ttc_dump);

    delete ttc;
    delete global_dict;
}
