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
 * Local_Checkpointer.cpp
 *
 *  Created on: 07-Mar-2011
 *      
 */
#include "Context.h"
#include "Local_Checkpointer.h"
#include <fstream>
#include <sstream>

Local_Checkpointer::Local_Checkpointer() {
}

Local_Checkpointer::~Local_Checkpointer() {
}

void Local_Checkpointer::save_metadata(std::string& state) {
    std::string chkpt_file = Context::get_instance().get_string("chkpt_file");
    std::string new_chkpt_file = chkpt_file + ".new";
    std::ofstream chkpt_output(new_chkpt_file.c_str());
    chkpt_output << state;
    chkpt_output.flush();
    chkpt_output.close();
    std::string cmd = "mv " + new_chkpt_file + " " + chkpt_file;
    system(cmd.c_str());
}

std::string Local_Checkpointer::load_metadata() {
    std::string chkpt_file = Context::get_instance().get_string("chkpt_file");
    std::ifstream chkpt_input(chkpt_file.c_str());
    //std::string empty_str("");
    if (!chkpt_input.is_open())
        return "";
    std::stringbuf state;
    chkpt_input.get(state);
    return state.str();
}

void Local_Checkpointer::checkpoint() {
}
