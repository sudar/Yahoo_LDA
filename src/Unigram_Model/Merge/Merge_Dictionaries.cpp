/*******************************************************************************
    Copyright (c) 2011, Yahoo! Inc.
    All rights reserved.

    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:

    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.

    * Neither the name of Yahoo! Inc. nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of Yahoo! Inc.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

using namespace std;

DEFINE_string(dumpprefix,"specify", "The dump of the dictionary to be used instead of creating afresh");
DEFINE_int32(dictionaries,-1,"The number of dictionaries present");
DEFINE_string(outputprefix,"lda", "A prefix that will be used with all files output by the program");

int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    string
            usage(
                    "\n\n This program takes a set of dictionary dumps and merges into one single dictionary\n"
                        "It is used to merge a set of local dictionaries found in the current directory in the multi-machine version\n\n");

    string
            cmd_usage(
                    (string) argv[0]
                            + " --dumpprefix=<dumpprefix> --dictionaries=<num of dictionaries> [--outputprefix=<outputprefix>]");

    usage += cmd_usage;

    google::SetUsageMessage(usage);
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (strcmp(argv[i], "--help") == 0) {
            google::ShowUsageWithFlagsRestrict(argv[0], "Merge_Dictionaries");
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

    if (google::GetCommandLineFlagInfoOrDie("dictionaries").is_default
            || google::GetCommandLineFlagInfoOrDie("dumpprefix").is_default)
        LOG(FATAL)
                << "You need to specify the number of dictionaries to merge"
                << " and the dictionary prefix via dumpprefix flag";

    WordIndexDictionary* dict = new WordIndexDictionary();
    string dict_dump = FLAGS_outputprefix + ".dict" + ".dump";

    LOG(WARNING) << "Initializing merge of " << FLAGS_dictionaries
            << " dictionaries using " << FLAGS_dumpprefix << " as prefix";
    dict->initialize_from_dumps(FLAGS_dumpprefix, FLAGS_dictionaries);
    LOG(WARNING) << "Dictionaries merged";

    LOG(WARNING) << "Num of unique words: " << dict->get_num_words();
    LOG(WARNING)
            << "Dumping global dictionary for later use by learnTopics into "
            << dict_dump << endl;
    dict->dump(dict_dump);
    LOG(WARNING) << "Finished dictionary dump" << endl;

    delete dict;
}
