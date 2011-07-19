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
