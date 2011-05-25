/*****************************************************************************
     The contents of this file are subject to the Mozilla Public License
     Version 1.1 (the "License"); you may not use this file except in
     compliance with the License. You may obtain a copy of the License at
     http://www.mozilla.org/MPL/

     Software distributed under the License is distributed on an "AS IS"
     basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
     License for the specific language governing rights and limitations
     under the License.

     The Original Code is Copyright (C) by Yahoo! Research.

     The Initial Developer of the Original Code is Shravan Narayanamurthy.

     All Rights Reserved.
******************************************************************************/
/*
 * Hadoop_Checkpointer.cpp
 *
 *  Created on: 08-Mar-2011
 *      
 */

#include "Hadoop_Checkpointer.h"
#include "Context.h"
#include "glog/logging.h"

Hadoop_Checkpointer::Hadoop_Checkpointer() {
}

Hadoop_Checkpointer::~Hadoop_Checkpointer() {
}

void Hadoop_Checkpointer::checkpoint() {
    Context& context = Context::get_instance();
    using namespace std;
    string chkpt_dir = context.get_string("chkptdir");
    string chkpt_file = context.get_string("chkpt_file");
    string input_t = context.get_string("input_t");

    {
        //copy the topic assignments to chkpt_dir
        string dest_file(chkpt_dir + "/" + input_t);
        string top_cp_cmd(
                "hadoop dfs -put " + input_t + " " + dest_file + ".tmp");
        LOG(WARNING) << top_cp_cmd;
        system(top_cp_cmd.c_str());
        string rm_cmd("hadoop dfs -rmr " + dest_file);
        LOG(WARNING) << rm_cmd;
        system(rm_cmd.c_str());
        string mv_cmd("hadoop dfs -mv " + dest_file + ".tmp " + dest_file);
        LOG(WARNING) << mv_cmd;
        system(mv_cmd.c_str());
    }

    {
        //copy the metadata to chkpt_dir
        string dest_file(chkpt_dir + "/" + chkpt_file);
        string chk_cp_cmd(
                "hadoop dfs -put " + chkpt_file + " " + dest_file + ".tmp");
        system(chk_cp_cmd.c_str());
        string rm_cmd("hadoop dfs -rmr " + dest_file);
        LOG(WARNING) << rm_cmd;
        system(rm_cmd.c_str());
        string mv_cmd("hadoop dfs -mv " + dest_file + ".tmp " + dest_file);
        LOG(WARNING) << mv_cmd;
        system(mv_cmd.c_str());
    }

}
