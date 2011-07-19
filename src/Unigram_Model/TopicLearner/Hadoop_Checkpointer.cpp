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
