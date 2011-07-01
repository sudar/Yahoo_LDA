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
/** \page multi_machine_usage Multi-Machine Setup
 * <P>Please take a look at \ref single_machine_usage "Single Machine Usage" for information on running
 * individual commands. Here we give you ways to run those individual commands on multiple machines.
 * So, we are not repeating the details on the individual commands.</P>
 * <OL>
 *         <LI><P STYLE="margin-bottom: 0cm"><B>Using Hadoop</B></P>
 *         <OL>
 *                 <LI><P STYLE="margin-bottom: 0cm">Organize your corpus on HDFS:</P>
 *                 <OL>
 *                         <LI><P STYLE="margin-bottom: 0cm">Run <CODE>splitter.sh
 *                         &quot;queue&quot; &quot;orig-corpus&quot; &quot;organized-corpus&quot;
 *                         &quot;#chunks&quot;</CODE></P>
 *                         <P STYLE="margin-bottom: 0cm">The splitter program is a simple
 *                         map-reduce streaming script that splits your original corpus into
 *                         #chunks gzip files. This enables Y!LDA to process one chunk on one
 *                         machine. We advise you to use one full machine to run one instance
 *                         of Y!LDA as the memory requirements may be large depending on your
 *                         corpus size. A very large corpus of the order of 10 to 20 Million
 *                         moderately sized documents can need anywhere from 5 to 6 GB of
 *                         memory. You can reduce this by using more machines though. For
 *                         ex., if you have a corpus of 1.5 Million documents you might want
 *                         to split it across 8 machines using:</P>
 *                         <P STYLE="margin-bottom: 0cm"><CODE>splitter.sh
 *                         &quot;queue&quot; &quot;orig-corpus&quot; &quot;organized-corpus&quot;
 *                         8</CODE></P>
 *                 </OL>
 *                 <LI><P STYLE="margin-bottom: 0cm">Run Y!LDA on the organized
 *                 corpus:</P>
 *                 <OL>
 *                         <LI><P STYLE="margin-bottom: 0cm">Assuming you have a homogenous
 *                         setup, install Y!LDA on one machine.</P>
 *                         <LI><P STYLE="margin-bottom: 0cm">Run make jar to create
 *                         LDALibs.jar file with all the required libraries and binaries</P>
 *                         <LI><P STYLE="margin-bottom: 0cm">Copy LDALibs.jar to HDFS</P>
 *			   <LI><P STYLE="margin-bottom: 0cm">Copy Formatter.sh LDA.sh functions.sh runLDA.sh to gateway</P>
 *                         <LI><P STYLE="margin-bottom: 0cm">Figure out the max memory
 *                         allowed per map task for your cluster and use the same in the
 *                         script via the maxmem parameter. This can be done by looking at
 *                         any job conf (job.xml) and searching the value of
 *                         &quot;mapred.cluster.max.map.memory.mb&quot; property.</P>
 *                         <LI><P STYLE="margin-bottom: 0cm">Run <CODE>runLDA.sh
 *                         1 &quot;flags&quot; [train|test] &quot;queue&quot;
 *                         &quot;organized-corpus&quot; &quot;output-dir&quot; &quot;max-mem&quot;
 *                         &quot;#topics&quot; &quot;#iters&quot;
 *                         &quot;full_hdfs_path_of_LDALibs.jar&quot; [&quot;training-output&quot;]
 *                         </CODE><BR/><BR/>
 *			   For train ex. <CODE>runLDA.sh
 *                         1 &quot;&quot; train default
 *                         &quot;/user/me/organized-corpus&quot; &quot;/user/me/lda-output&quot; 6144 100 100
 *                         &quot;LDALibs.jar&quot;
 *                         </CODE><BR/><BR/>
 *			   For test ex. <CODE>runLDA.sh
 *                         1 &quot;&quot; test default
 *                         &quot;/user/me/organized-corpus&quot; &quot;/user/me/lda-output&quot; 6144 100 100
 *                         &quot;LDALibs.jar&quot; &quot;/user/me/lda-output&quot;
 *                         </CODE>
 *                         </P>
 *                         <LI><P STYLE="margin-bottom: 0cm">This
 *                         starts a map-only script on each machine. The script starts the
 *                         DM_Server on all the machines. Then Y!LDA is run on each machine.
 *                         The input is one chunk of the corpus.</P>
 *                         <LI><P STYLE="margin-bottom: 0cm">For
 *                         testing, use the test flag and provide directory storing the
 *                         training output.</P>
 *                 </OL>
 *                 <LI><P STYLE="margin-bottom: 0cm">Output
 *                 generated</P>
 *                 <OL>
 *                         <LI><P STYLE="margin-bottom: 0cm">Creates &lt;#chunks&gt; folders
 *                         in &lt;output-dir&gt; one for each client. 
 *                         </P>
 *                         <LI><P STYLE="margin-bottom: 0cm">Each of these directories hold
 *                         the same output as the single machine case but from different
 *                         clients. 
 *                         </P>
 *                         <LI><P STYLE="margin-bottom: 0cm">&lt;output-dir&gt;/&lt;client-id&gt;/learntopics.WARNING
 *                         contains the output written to stderr by client &lt;client-id&gt;</P>
 *                         <LI><P STYLE="margin-bottom: 0cm">&lt;output-dir&gt;/&lt;client-id&gt;/lda.docToTop.txt
 *                         contains the topic proportions assigned to the documents in the
 *                         portion of the corpus alloted to client &lt;client-id&gt;</P>
 *                         <LI><P STYLE="margin-bottom: 0cm">&lt;output-dir&gt;/&lt;client-id&gt;/lda.topToWor.txt
 *                         contains the salient words learned for each topic. This remains
 *                         almost same across clients. So you can pick one of these as the
 *                         salient words per topic for the full corpus.</P>
 *                         <LI><P STYLE="margin-bottom: 0cm">&lt;output-dir&gt;/&lt;client-id&gt;/lda.ttc.dump
 *                         contains the actual model. Even this like the salient words is
 *                         almost same across clients and any one can be used as the model
 *                         for the full corpus.</P>
 *                         <LI><P STYLE="margin-bottom: 0cm">&lt;output-dir&gt;/global
 *                         contains the dump of the global dictionary and the partitioned
 *                         gobal topic counts table. These are generated in the training
 *                         phase and are critical for the test option to work.</P>
 *                 </OL>
 *                 <LI><P STYLE="margin-bottom: 0cm">Viewing
 *                 progress</P>
 *                 <OL>
 *                         <LI><P STYLE="margin-bottom: 0cm">The
 *                         stderr output of the code will be redirected into hadoop logs. So
 *                         you can check the task logs from the tracking URL displayed in the
 *                         output of runLDA.sh to see what is happening</P>
 *                 </OL>
 *                 <LI><P STYLE="margin-bottom: 0cm">Failure Recovery</P>
 *                 <P> We provide a check-pointing mechanism to handle recovery from failures.
 *                 The current scheme works in local mode and for distributed mode using Hadoop.
 *                 The reason for this being that the distributed check-pointing uses the hdfs to store
 *                 the check-points. The following is the process: </P>
 *                 <OL>
 *                 <LI>The formatter task is run on the inputs and the formatted input is stored in a temporary location.
 *                 <LI>The learntopics task is run using the temporary location as an input and the specified output as the
 *                 output directory. Care is taken to start the same number of mappers for both the formatter and
 *                 learntopics tasks. The input is a dummy directory structure with dummy directories equal to the
 *                 number of mappers.
 *                 <LI>Each learntopics task copies its portion of the formatted input by dfs copy_to_local the
 *                 folder corresponding to its mapred_task_partition.
 *                 <LI>Runs learntopics with the temporary directory containing the formatted input
 *                 as a check-point directory. So all information needed to start learntopics from
 *                 the previous check-pointed iteration is available locally and any progress made
 *                 is written back to the temporary input directory.
 *                 </OL>
 *                 <P>This mechanism is utilized by the scripts to detect failure cases
 *                 and attempt to re-run the task again from the previous checkpoint. As learntopics is
 *                 designed to check if check-point metadata is available in the working directory and
 *                 use it to start-off from there a separate restart option is obviated. </P>
 *                 <P>As a by product one gets the facility of doing incremental runs, that is,
 *                 to run say 100 iterations, check the output and run the next 100 iterations if needed.
 *                 The scripts detect this condition and ask you if you want to start-off from where you left
 *                 or restart from the beginning.</P>
 *                 <P>The scripts are designed in such a fashion that these
 *                 happen transparently to the user. This is information for developers and for cases
 *                 where the recovery mechanism could not handle the failure in the specified number of
 *                 attempts. Check the stderr logs to see what the reason for failure is. Most times it is due
 *                 to wrong usage which results in unrecoverable aborts. If you think its because of a flaky
 *                 cluster, then try increasing the number of attempts. If nothing works and you think there
 *                 is a bug in the code please let us know.</P>
 *         </OL>
 * </OL>
 * <OL START=2>
 *      <LI><P STYLE="margin-bottom: 0cm"><B>Using SSH - Assume you have
 *      'm' machines</B></P>
 *      <OL>
 *              <LI><P STYLE="margin-bottom: 0cm">If you have a homogenous set up,
 *              install Y!LDA on one machine, run make jar and copy LDALibs.jar to
 *              all the other machines in the set up. Else install Y!LDA on all
 *              machines.</P>
 *              <LI><P STYLE="margin-bottom: 0cm">Split the corpus into 'm' parts
 *              and distribute them to the 'm' machines</P>
 *              <LI><P STYLE="margin-bottom: 0cm">Run formatter on each split of
 *              the corpus on every machine.</P>
 *              <LI><P STYLE="margin-bottom: 0cm">Run the Distributed_Map Server on
 *              each machine as a background process using nohup:</P>
 *              <P STYLE="margin-bottom: 0cm"><CODE>nohup
 *              ./DM_Server &lt;model&gt; &lt;server_id&gt; &lt;num_clients&gt;
 *              &lt;host:port&gt; --Ice.ThreadPool.Server.SizeMax=9 &amp;</CODE></P>
 *              <OL>
 *                      <LI><P STYLE="margin-bottom: 0cm">model: an integer that
 *                      represents the model. Set it to 1 for Unigram_Model</P>
 *                      <LI><P STYLE="margin-bottom: 0cm">server_id: a number that denotes
 *                      the index of this server in the list of servers that is provided
 *                      to 'learntopics'. If server1 has h:p, 10.1.1.1:10000 &amp; is
 *                      assigned id 0, server2 has h:p, 10.1.1.2:10000 &amp; is assigned
 *                      id 1, the list of servers that is provided to 'learntopics' has to
 *                      be 10.1.1.1:10000, 10.1.1.2:10000 and not the other way around.</P>
 *                      <LI><P STYLE="margin-bottom: 0cm">num_clients: a number that
 *                      denotes the number of clients that will access the Distributed
 *                      Map. This is usually equal to 'm'. This is used to provide a
 *                      barrier implementation</P>
 *                      <LI><P STYLE="margin-bottom: 0cm">host:port- the port and ip
 *                      address on which the server must listen on</P>
 *              </OL>
 *              <LI><P STYLE="margin-bottom: 0cm">Run Y!LDA on the corpus:</P>
 *              <OL>
 *                      <LI><P STYLE="margin-bottom: 0cm">On every machine run</P>
 *			<P><CODE>learntopics
 *                      --topics=&lt;#topics&gt; --iter=&lt;#iter&gt;
 *                      --servers=&lt;list-of-servers&gt; --chkptdir=&quot;/tmp&quot;
 *                      --chkptinterval=10000</CODE></P>
 *                      <OL>
 *                              <LI><P STYLE="margin-bottom: 0cm">&lt;list-of-servers&gt;: The
 *                              comma separated list of ip:port numbers of the servers involved
 *                              in the set-up. The index of the ip:port numbers should be as per
 *                              the server_id parameter used in starting the server</P>
 *                              <LI><P STYLE="margin-bottom: 0cm">chkptdir &amp; chkptinterval:
 *                              These are currently used only with the Hadoop set-up. Set
 *                              chkptdir to something dummy. In order that the checkpointing code
 *                              does not execute, set the chkptinterval to a very large value or
 *                              some number greater than the number of iterations</P>
 *                      </OL>
 *                      <LI><P STYLE="margin-bottom: 0cm">Create Global Dictionary - Run
 *                      the following on server with id 0.  Assuming learntopics was run
 *                      in the folder /tmp/corpus</P>
 *                      <OL>
 *                              <LI><P STYLE="margin-bottom: 0cm"> <CODE>mkdir
 *                              -p /tmp/corpus/global_dict; cd /tmp/corpus/global_dict;</CODE></P>
 *                              <LI><P STYLE="margin-bottom: 0cm"> <CODE>scp
 *                              server_i:/tmp/corpus/lda.dict.dump lda.dict.dump.i </CODE>
 *                              where the variable 'i' is the same as the server_id.</P>
 *                              <LI><P STYLE="margin-bottom: 0cm">Merge Dictionaries<BR>
 *                              <CODE>Merge_Dictionaries
 *                              --dictionaries=m --dumpprefix=lda.dict.dump</CODE></P>
 *                              <LI><P STYLE="margin-bottom: 0cm"> <CODE>mkdir
 *                              -p ../global; mkdir -p ../global/topic_counts; cp lda.dict.dump
 *                              ../global/;</CODE> 
 *                              </P>
 *                      </OL>
 *                      <LI><P STYLE="margin-bottom: 0cm">Create a sharded Global
 *                      Word-Topic Counts dump - Run on every machine in the set-up</P>
 *                      <OL>
 *                              <LI><P STYLE="margin-bottom: 0cm"><CODE>mkdir
 *                              -p /tmp/corpus/global_top_cnts; cd /tmp/corpus/global_top_cnts;</CODE></P>
 *                              <LI><P STYLE="margin-bottom: 0cm"><CODE>scp
 *                              server_0:/tmp/corpus/global/lda.dict.dump lda.dict.dump.global</CODE></P>
 *                              <LI><P STYLE="margin-bottom: 0cm"><CODE>Merge_Topic_Counts
 *                              --topics=&lt;#topics&gt; --clientid=&lt;server-id&gt;
 *                              --servers=&lt;list-of-servers&gt;
 *                              --globaldictionary=&quot;lda.dict.dump.global&quot;</CODE></P>
 *                              <LI><P STYLE="margin-bottom: 0cm"><CODE>scp
 *                              lda.ttc.dump
 *                              server_0:/tmp/corpus/global/topic_counts/lda.ttc.dump.$server-id</CODE></P>
 *                      </OL>
 *                      <LI><P STYLE="margin-bottom: 0cm">Copy the parameters dump file to
 *                      global dump - Run on server_0</P>
 *                      <OL>
 *                              <LI><P STYLE="margin-bottom: 0cm"><CODE>cd
 *                              /tmp/corpus; cp lda.par.dump global/topic_counts/lda.par.dump</CODE></P>
 *                      </OL>
 *                      <LI><P STYLE="margin-bottom: 0cm">This completes training and the
 *                      model is available on server_0:/tmp/corpus/global</P>
 *              </OL>
 *              <LI><P STYLE="margin-bottom: 0cm">Running Y!LDA is test mode: Run
 *              on server_0. Assuming test corpus is in /tmp/test_corpus</P>
 *              <OL>
 *                      <LI><P STYLE="margin-bottom: 0cm"><CODE>cd
 *                      /tmp/test_corpus;</CODE> 
 *                      </P>
 *                      <LI><P STYLE="margin-bottom: 0cm"><CODE>cp
 *                      -r ../corpus/global .</CODE></P>
 *                      <LI><P STYLE="margin-bottom: 0cm"><CODE>learntopics
 *                      -teststream --dumpprefix=global/topic_counts/lda --numdumps=m
 *                      --dictionary=global/lda.dict.dump --maxmemory=2048
 *                      --topics=&lt;#topics&gt;</CODE></P>
 *                      <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">cat all your
 *                      documents, in the same format that 'formatter' expects, to the
 *                      above command's stdin</P>
 *              </OL>
 *      </OL>
 * </OL>
 */
