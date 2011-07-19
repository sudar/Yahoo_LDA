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
 * Controller.cpp
 *
 * Controller Script for the formatter.
 *
 * Instantiates the appropriate Formatter based on the
 * chosen model and formats the corpus using that.
 *
 *  Created on: 11-Jan-2011
 *      
 */

#include "FormatData_flags_define.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include <stdlib.h>
#include "Data_Formatter.h"
#include "Formatter/Unigram_Train_Data_Formatter.h"
#include "Formatter/Unigram_Test_Data_Formatter.h"
#include "Context.h"
#include "TopicLearner/Model.h"

using namespace std;

/**
 * Returns the formatter for the chosen model
 */
Data_Formatter* get_data_formatter() {
    Context& context = Context::get_instance();
    int model = context.get_int("model");
    string dumpfile = context.get_string("dumpfile");
    switch (model) {
    case Model::UNIGRAM:
        if (google::GetCommandLineFlagInfoOrDie("dumpfile").is_default)
            return new Unigram_Train_Data_Formatter();
        else
            return new Unigram_Test_Data_Formatter();
        break;

    default:
        LOG(FATAL) << "The model you seek has not been implemented";
    }
    return NULL;
}

/**
 * Deallocate resources held by the formatter
 */
void release_data_formatter(Data_Formatter* formatter) {
    delete formatter;
}

/**
 * Main
 */
int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    string
            usage(
                    "\n\nThis program takes the tokenized corpus supplied as input and formats it into the\n"
                        "protobuffers format needed for the actual LDA code.\n\n"
                        "It can optionally take a prefix to be used for the files output by this routine.\n\n"
                        "The primary output is the documents in the protobuffer format.\n\n"
                        "The program by default removes stop words supplied statically in constants.h file.\n"
                        "It then creates the dictionary from the remaining words.\n\n"
                        "In test mode you can use a previously induced dictionary to restrict\n"
                        "the corpus to only words that are known to the previously learnt topic model.\n\n"
                        "This also generates on stdout some statistics like the number of\n"
                        "unique words found and the total number of documents present.\n\n"
                        "Take a look at docs/html/index.html for more info.\n\n");
    string
            cmd_usage(
                    (string) argv[0]
                            + " --model=1 --corpus=<corpus> [--outputprefix=<outputprefix>] [--dumpfile=<dump_of_dictionary>]\n\n");
    usage += cmd_usage;
    google::SetUsageMessage(usage);
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (strcmp(argv[i], "--help") == 0) {
            google::ShowUsageWithFlagsRestrict(argv[0], "Format");
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

    LOG(INFO) << flagsInp;
    if (google::GetCommandLineFlagInfoOrDie("corpus").is_default) {
        LOG(WARNING)
                << "Assuming that corpus is being piped through stdin. Reading from stdin...";
        FLAGS_corpus = "";
    }
        else
        LOG(WARNING) << "Reading from " << FLAGS_corpus;

    LOG_IF(WARNING, !google::GetCommandLineFlagInfoOrDie("dumpfile").is_default)<<"Will use the dictionary dump " << FLAGS_dumpfile << " to load the global dictionary.";

    Context& context = Context::get_instance();
    string output_prefix = context.get_string("outputprefix");
    string output_w = output_prefix + ".wor";
    context.put_string("output_w",output_w);
    string corpus = context.get_string("corpus");
    LOG(INFO) << "Formatting " << corpus << " into " << output_w;
    Data_Formatter* formatter = get_data_formatter();
    formatter->format();
    LOG(WARNING) << "Formatting Complete. Formatted document stored in " << output_w
    << ". You have used " << output_prefix
    << " as the output prefix. Make sure you use the same as the input prefix for learnTopics"
    << endl;
    string dict_dump = output_prefix + ".dict" + ".dump";
    if(google::GetCommandLineFlagInfoOrDie("dumpfile").is_default)
    LOG(WARNING) << "Dumping dictionary for later use by learnTopics into " << dict_dump;
    else
    LOG(WARNING) << "Induced local dictionary being dumped to " << dict_dump;
    formatter->get_dictionary().dump(dict_dump);
    LOG(WARNING) << "Finished dictionary dump";
    LOG(WARNING) << "Formatting done";

    int num_words = formatter->get_dictionary().get_num_words();

    LOG(WARNING) << "Total number of unique words found: " << num_words;
    LOG(WARNING) << "Total num of docs found: " << formatter->get_num_docs();
    LOG(WARNING) << "Total num of tokens found: " << formatter->get_total_num_words();

    release_data_formatter(formatter);
}
