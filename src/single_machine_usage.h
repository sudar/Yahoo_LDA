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
/** \page single_machine_usage Single Machine Setup
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">First step is to set
 * LD_LIBRARY_PATH. This can be done by sourcing the script
 * setLibVars.sh</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Assume Y!LDA is installed
 * in $LDA_HOME.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">cd $LDA_HOME</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">source ./setLibVars.sh</P>
 * <H3 CLASS="western">    Basic Usage: 
 * </H3>
 * <OL>
 * <LI>\ref learning_model</LI>
 * <LI>\ref using_model</LI>
 * <LI>\ref generated_output</LI>
 * <LI>\ref customizations</LI>
 * </OL>
 * \section learning_model Learning a Model
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"> The process of learning
 * a model has four steps:</P>
 * <OL>
 *	<LI>\ref tokenize_format </LI>
 * 	<LI>\ref learntopics </LI>
 * 	<LI>\ref word_mix </LI>
 * 	<LI>\ref topic_mix </LI>
 * </OL>
 * \subsection tokenize_format Tokenization and Formatting
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"> The tokenizer converts 
 * text into tokens that undergone some basic normalizations.
 * Most likely you want to write your own. A Java
 * version is provided for convenience more than anything else. This is
 * a simple java class that tokenizes the file by splitting the stream
 * around non-character[^a-zA-Z] boundaries into word tokens. So, it
 * ignores numbers &amp; punctuation currently. The raw text corpus is
 * assumed to have the following format. Its a single file containing
 * documents on every line. Each document has the format:</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"> doc-id&lt;space&gt;aux-id&lt;space&gt;word1[&lt;space&gt;word2]*</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">The tokenizer just writes
 * the tokens appended to the <STRONG>doc-id</STRONG> &amp; <STRONG>aux-id</STRONG>
 * to stdout.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"> The 'formatter' converts
 * the text into GoogleProtocolBuffer messages. This is done to ensure
 * that we have a very small file footprint on disk. It takes the
 * tokenized corpus supplied as input and formats it into the internal
 * format needed for learntopics. The primary output is the documents in
 * the internal format. The program by default removes stop words
 * supplied statically in the src/commons/constants.h file. It then
 * creates the dictionary from the remaining words.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">The following is an
 * illustration of this step. It uses the taining set that is available
 * with the package (ut_out/ydir_1k.txt)</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <CODE>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">$
 * cd $LDA_HOME/ut_out</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">$
 * cp ../Tokenizer.java .</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">$
 * javac Tokenizer.java</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">$
 * ls -al ydir_1k.txt</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm 1818848 2011-04-17 17:34 ydir_1k.txt 
 * </P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">$
 * cat ydir_1k.txt | java Tokenizer | ../formatter</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:44.967370 19605 Controller.cpp:83]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:44.967788 19605 Controller.cpp:84] Log files are being stored
 * at /home/shravanm/workspace/LDA_Refactored/ut_out/unigram/formatter.*
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:44.967808 19605 Controller.cpp:85]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:44.968111 19605 Controller.cpp:91] Assuming that corpus is
 * being piped through stdin. Reading from stdin... 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:45.652430 19605 Controller.cpp:107] Formatting Complete.
 * Formatted document stored in lda.wor. You have used lda as the output
 * prefix. Make sure you use the same as the input prefix for
 * learntopics 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:45.652497 19605 Controller.cpp:113] Dumping dictionary for
 * later use by learntopics into lda.dict.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:45.674201 19605 Controller.cpp:117] Finished dictionary dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:45.674236 19605 Controller.cpp:118] Formatting done 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:45.674253 19605 Controller.cpp:122] Total number of unique
 * words found: 17208 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:45.674270 19605 Controller.cpp:123] Total num of docs found:
 * 900 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:35:45.674288 19605 Controller.cpp:124] Total num of tokens found:
 * 182525</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">$
 * ls -1 lda*</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm 204655 2011-04-17 17:35 lda.dict.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm 397983 2011-04-17 17:35 lda.wor 
 * </P>
 * </CODE>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * \subsection learntopics Learning the topic mixtures
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"> 'learntopics' learns a topic model from a corpus which
 * has been formatted using 'formatter'. The input to this are the files
 * generated by 'formatter'. So you need to run this in the same
 * directory that contains these files. The output from this step
 * contains two types of files: binary files that are used by other
 * modes of operation like batch and stream mode testing and text files
 * which are human readable and give a sense of what has happened. The
 * generated output will be explained in detail in the next sections.
 * But the primary outputs are the topic assignments to the documents in
 * the corpus and the word mixtures that represent each topic that has
 * been learnt.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Continuing with the
 * illustration of step 1 we do the following to learn the model:
 * Assuming that PWD=$LDA_HOME/ut_out</P>
 * <CODE>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">$
 * ../learntopics --topics=100 --iter=500</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Log
 * file created at: 2011/04/17 17:44:04 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Running
 * on machine: offerenjoy 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Log
 * line format: [IWEF]mmdd hh:mm:ss.uuuuuu threadid file:line] msg 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.082424 19628 Controller.cpp:68]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.082855 19628 Controller.cpp:69] Log files are being stored
 * at
 * /home/shravanm/workspace/LDA_Refactored/ut_out/unigram/learntopics 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.082871 19628 Controller.cpp:70]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.083418 19628 Controller.cpp:81] You have chosen single
 * machine training mode 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.083976 19628 Unigram_Model_Training_Builder.cpp:43]
 * Initializing Dictionary from lda.dict.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.137420 19628 Unigram_Model_Training_Builder.cpp:45]
 * Dictionary Initialized 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.174685 19628 Unigram_Model_Trainer.cpp:24] Initializing
 * Word-Topic counts table from docs lda.wor, lda.top using 17208 words
 * &amp; 100 topics. 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.250243 19628 Unigram_Model_Trainer.cpp:26] Initialized
 * Word-Topic counts table 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.250313 19628 Unigram_Model_Trainer.cpp:29] Initializing
 * Alpha vector from Alpha_bar = 50 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.250356 19628 Unigram_Model_Trainer.cpp:31] Alpha vector
 * initialized 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.250375 19628 Unigram_Model_Trainer.cpp:34] Initializing Beta
 * Parameter from specified Beta = 0.01 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.250395 19628 Unigram_Model_Trainer.cpp:37] Beta param
 * initialized 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.251711 19628 Training_Execution_Strategy.cpp:35] Starting
 * Parallel training Pipeline 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.590452 19628 Training_Execution_Strategy.cpp:53] Iteration 1
 * done. Took 0.00564301 mins 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.700546 19628 Unigram_Model.cpp:92] Average num of topics
 * assigned per word = 4.17091 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.700599 19628 Training_Execution_Strategy.cpp:58] &gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;
 * Log-Likelihood (model, doc, total): -1.47974e+06 , -815572 ,
 * -2.29531e+06 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.703927 19628 Unigram_Model_Trainer.cpp:478] Restarting IO 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.992218 19628 Training_Execution_Strategy.cpp:53] Iteration 2
 * done. Took 0.00477359 mins 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:04.995560 19628 Unigram_Model_Trainer.cpp:478] Restarting IO 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:05.261360 19628 Training_Execution_Strategy.cpp:53] Iteration 3
 * done. Took 0.00439862 mins 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:05.264669 19628 Unigram_Model_Trainer.cpp:478] Restarting IO 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:44:05.516330 19628 Training_Execution_Strategy.cpp:53] Iteration 4
 * done. Took 0.00416395 mins 
 * </P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">...</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">...</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.021239 19628 Unigram_Model_Trainer.cpp:478] Restarting IO 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.242962 19628 Training_Execution_Strategy.cpp:53] Iteration
 * 498 done. Took 0.00366346 mins 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.246363 19628 Unigram_Model_Trainer.cpp:478] Restarting IO 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.469266 19628 Training_Execution_Strategy.cpp:53] Iteration
 * 499 done. Took 0.00368553 mins 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.472687 19628 Unigram_Model_Trainer.cpp:478] Restarting IO 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.513748 19628 Training_Execution_Strategy.cpp:53] Iteration
 * 500 done. Took 0.000653982 mins 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.616693 19628 Unigram_Model.cpp:92] Average num of topics
 * assigned per word = 1.85268 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.616768 19628 Training_Execution_Strategy.cpp:58] &gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;
 * Log-Likelihood (model, doc, total): -1.04167e+06 , -425400 ,
 * -1.46707e+06 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.620058 19628 Unigram_Model_Trainer.cpp:478] Restarting IO 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.622022 19628 Training_Execution_Strategy.cpp:66] &gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;
 * Check Pointing at iteration: 500 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.625076 19628 Training_Execution_Strategy.cpp:72] Parallel
 * training Pipeline done 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.625128 19628 Controller.cpp:128] Model has been learnt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.627476 19628 Unigram_Model.cpp:105] Saving model for test
 * pipeline in lda.ttc.dump and lda.par.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.695773 19628 Unigram_Model.cpp:108] Model saved 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.695843 19628 Unigram_Model.cpp:117] Writing top words
 * identified per topic into lda.topToWor.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.701156 19628 Unigram_Model.cpp:139] Word statistics per
 * topic written 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.701257 19628 Unigram_Model_Training_Builder.cpp:126] Saving
 * document to topic-proportions in lda.docToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.701279 19628 Unigram_Model_Training_Builder.cpp:127] Saving
 * word to topic assignment in lda.worToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.911658 19628 Unigram_Model_Training_Builder.cpp:193]
 * Document to topic-proportions saved in lda.docToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 17:45:59.911726 19628 Unigram_Model_Training_Builder.cpp:194] Word to
 * topic assignment saved in lda.worToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">$
 * ls -al lda*</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm       4 2011-04-17 17:45 lda.chk 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  204655 2011-04-17 17:35 lda.dict.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  168223 2011-04-17 17:45 lda.docToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm     816 2011-04-17 17:45 lda.par.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  230304 2011-04-17 17:45 lda.top 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm   38415 2011-04-17 17:45 lda.topToWor.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  267631 2011-04-17 17:45 lda.ttc.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  397983 2011-04-17 17:35 lda.wor 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm 2213071 2011-04-17 17:45 lda.worToTop.txt 
 * </P>
 * </CODE>
 * \subsection word_mix Viewing the word mixtures for each topic
 * <CODE>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">$
 * cat  lda.topToWor.txt</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Topic
 * 0: (center,0.171509) (diving,0.110265) (scuba,0.0857668)
 * (equipment,0.0686183) (mark,0.0637188) (olympic,0.0465703)
 * (rescue,0.0441205) (aquatic,0.039221) (ymca,0.039221)
 * (family,0.0367712) (ruiz,0.0343214) (dive,0.0318716)
 * (sports,0.0294219) (divers,0.0294219) (safety,0.0294219)
 * (minnesota,0.0294219) (orlando,0.0294219) (advanced,0.0294219)
 * (training,0.0269721) (international,0.0245223) 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Topic
 * 1: (beach,0.138171) (resort,0.0760789) (florida,0.0714219)
 * (world,0.062108) (center,0.0543465) (experience,0.0527942)
 * (vacation,0.0481372) (fl,0.0465849) (located,0.0450326)
 * (offers,0.0450326) (holiday,0.0403757) (south,0.0403757)
 * (rates,0.0403757) (beautiful,0.0388233) (location,0.0341664)
 * (sea,0.0341664) (activities,0.0341664) (resorts,0.0326141)
 * (accommodations,0.0326141) (perfect,0.0326141) 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">...</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Topic
 * 25: (surf,0.381466) (surfing,0.0727078) (surfboards,0.0573127)
 * (surfboard,0.0547468) (board,0.0530363) (beach,0.0367858)
 * (shop,0.0367858) (malibu,0.0350753) (longboard,0.0325094)
 * (boards,0.0325094) (gear,0.0299435) (bags,0.022246) (travel,0.022246)
 * (racks,0.0213907) (wetsuits,0.0205354) (clothing,0.0205354)
 * (california,0.0196801) (lessons,0.0188248) (ocean,0.016259)
 * (surfers,0.0154037) 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">...</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Topic
 * 35: (credit,0.101053) (card,0.0893533) (money,0.0680813)
 * (orders,0.0670177) (make,0.060636) (mail,0.0574452) (cards,0.0563816)
 * (valley,0.055318) (paypal,0.0521272) (accept,0.0510636)
 * (secure,0.0468092) (purchase,0.0468092) (time,0.0372368)
 * (apple,0.0351095) (checks,0.0351095) (email,0.0351095)
 * (address,0.0287279) (due,0.0276643) (special,0.0255371)
 * (people,0.0234099)</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">...</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Topic
 * 43: (racquet,0.12544) (racquets,0.107418) (wilson,0.101651)
 * (head,0.094442) (tennis,0.0908377) (prince,0.0821871)
 * (shoes,0.0648861) (babolat,0.0367719) (pro,0.0367719) (bags,0.030284)
 * (string,0.0266796) (men,0.0259588) (tour,0.0237961) (mp,0.0223544)
 * (rackets,0.0223544) (ncode,0.0223544) (grips,0.0223544)
 * (women,0.0223544) (dunlop,0.0216335) (nike,0.0194709)</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">...</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Topic
 * 81: (gear,0.151097) (water,0.0828022) (swim,0.0786631)
 * (floatation,0.0621068) (html,0.0600373) (pro,0.0579677)
 * (exercise,0.0538286) (mask,0.0496896) (swimming,0.04762)
 * (aqua,0.0414114) (watergearwg,0.0372724) (aquajoggeraj,0.0372724)
 * (kids,0.0352028) (goggles,0.0331333) (zoggszoggs,0.0310637)
 * (fins,0.0310637) (hydro,0.0289942) (snorkel,0.0269247)
 * (snorkeling,0.0269247) (caps,0.0269247) 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">...</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Topic
 * 96: (horse,0.190855) (horses,0.126676) (riding,0.101343)
 * (training,0.0996538) (farm,0.0785425) (equestrian,0.0667202)
 * (dressage,0.0472978) (equine,0.0405421) (boarding,0.0380088)
 * (show,0.0337865) (stables,0.0270309) (lessons,0.0194308)
 * (michigan,0.0185864) (tack,0.0185864) (riders,0.0185864)
 * (ranch,0.016053) (farms,0.0152086) (pony,0.0152086)
 * (ponies,0.0143641) (sale,0.0135197)</P>
 * </CODE>
 * \subsection topic_mix Viewing the topic assignments
 * <CODE>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">$
 * cat lda.worToTop.txt</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">www.teddybears.com/ recreation/toys (teddy,36)
 * (bears,36) (enjoy,66) (teddy,36) (bears,36) (enjoy,66) (featuring,61)
 * (teddy,36) (bears,36) (teddy,36) (bear,36) (related,77)
 * (information,66) (everyday,38) (fun,44) (enjoy,66) (learn,2)
 * (enter,28) (love,61) (lord,51) (god,51) (heart,48) (soul,77)
 * (strength,80) (commandments,2) (give,13) (today,28) (hearts,61)
 * (impress,61) (house,80) (gates,60) (deuteronomy,36) (site,66)
 * (sponsored,77) (brown,48) (brehm,36) (bears,36) (teddy,36) (bear,36)
 * (artists,63) (teddy,36) (bears,36) (teddy,36) (bear,36)
 * (classifieds,61) (teddy,36) (bear,36) (clubs,77) (teddy,36) (bear,36)
 * (events,77) (teddy,36) (bear,36) (retailers,66) (teddy,36) (bear,36)
 * (magazines,61) (teddy,36) (bear,36) (books,66) (teddy,36) (bear,36)
 * (history,28) (web,66) (page,66) (design,16) (graphic,61)
 * (elements,45) (embedded,61) (html,81) (coding,48) (created,16)
 * (copyrighted,22) (kelly,6) (brown,48) (brehm,36) (rights,16)
 * (reserved,16) 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">www.bearsbythesea.com/      recreation/toys (teddy,36)
 * (bear,36) (store,20) (pismo,18) (beach,25) (california,25)
 * (specialize,56) (muffy,14) (store,20) (complete,20) (collections,87)
 * (checkout,91) (web,66) (site,66) (muffy,14) (muffy,14)
 * (interested,66) (information,56) (price,3) (guides,77) (forums,28)
 * (newsletters,78) (follow,56) (information,66) (link,78)
 * (interested,66) (purchasing,4) (items,4) (follow,56) (online,20)
 * (store,20) (link,78) (online,20) (store,20) (information,66) 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">www.the-toybox.com  recreation/toys (party,0)
 * (supplies,38) (wiggles,56) (licensed,19) (characters,83) (jay,0)
 * (jay,0) (jet,83) (cabbage,49) (patch,49) (play,61) (tents,59)
 * (activity,91) (master,0) (roll,57) (building,90) (toy,83)
 * (building,90) (racing,50) (beanie,19) (babies,13) (personalized,19)
 * (toys,83) (requires,85) (minimum,12) (order,19) (enter,91)
 * (coupon,90) (code,90) (good,61) (coupons,90) (mail,56) (order,19)
 * (info,57) (order,13) (options,78) (return,4) (policy,9) (shipping,13)
 * (rates,1) (email,56) (wiggles,0) (party,19) (supplies,38) (toys,83)
 * (accessories,13) (jay,0) (jay,0) (jet,83) (plane,91) (party,19)
 * (supplies,90) (toys,83) (accessories,13) (toys,83) (party,19)
 * (supplies,90) (licensed,19) (characters,83) (toys,83) (sell,56)
 * (free,13) (internet,85) (premier,78) (shopping,83) (network,61)
 * (discount,13) (shopping,4) (internet,19) (click,78) (visit,66) 
 * </P>
 * </CODE>
 * \section using_model Using the Model
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">    Once the model has been
 * trained, you can provide the binary files as a parameter to
 * 'learntopics' to infer topic mixtures on new documents. The following
 * illustration  uses the test set that is available with the package
 * (ut_test/ydir_1k.tst.txt).</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"> As explained in the overview, 
 * there are two modes in which the above learnt model can be used:</P>
 * <OL>
 * 	<LI>\ref batch_mode</LI>
 * 	<LI>\ref stream_mode</LI>
 * </OL>
 * \subsection batch_mode Batch Mode
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Assuming that PWD=$LDA_HOME/ut_test.</P>
 * <CODE>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">First Tokenize and format
 * the test data.</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">$
 * cp ../ut_out/Tokenizer.class .</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">$
 * cat ydir_1k.tst.txt | java Tokenizer | ../formatter
 * --dumpfile=../ut_out/unigram/lda.dict.dump</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:41.834889 20929 Controller.cpp:83]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:41.835304 20929 Controller.cpp:84] Log files are being stored
 * at /home/shravanm/workspace/LDA_Refactored/ut_test/formatter.* 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:41.835325 20929 Controller.cpp:85]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:41.835626 20929 Controller.cpp:91] Assuming that corpus is
 * being piped through stdin. Reading from stdin... 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:41.835649 20929 Controller.cpp:97] Will use the dictionary dump
 * ../ut_out/unigram/lda.dict.dump to load the global dictionary. 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:41.836802 20929 Unigram_Test_Data_Formatter.cpp:14]
 * Initializing Dictionary from ../ut_out/unigram/lda.dict.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:41.900626 20929 Unigram_Test_Data_Formatter.cpp:16] Num of
 * unique words: 17208 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:42.145593 20929 Controller.cpp:107] Formatting Complete.
 * Formatted document stored in lda.wor. You have used lda as the output
 * prefix. Make sure you use the same as the input prefix for
 * learntopics 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:42.145661 20929 Controller.cpp:115] Induced local dictionary
 * being dumped to lda.dict.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:42.150445 20929 Controller.cpp:117] Finished dictionary dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:42.150481 20929 Controller.cpp:118] Formatting done 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:42.150507 20929 Controller.cpp:122] Total number of unique
 * words found: 3506 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:42.150533 20929 Controller.cpp:123] Total num of docs found:
 * 100 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:16:42.150559 20929 Controller.cpp:124] Total num of tokens found:
 * 16394 
 * </P>
 * </CODE>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Note the --dumpfile flag.
 * Here is where you provide the dictionary of words that the model
 * knows about. Only those words in the test data set are recognized and
 * the rest are ignored.</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <CODE>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">$
 * ../learntopics -test --dumpprefix=../ut_out/unigram/lda --topics=100</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:00.932081 20947 Controller.cpp:68]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:00.932518 20947 Controller.cpp:69] Log files are being stored
 * at /home/shravanm/workspace/LDA_Refactored/ut_test/learntopics.* 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:00.932539 20947 Controller.cpp:70]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:00.933087 20947 Controller.cpp:92] You have chosen single
 * machine testing mode 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:00.933671 20947 Unigram_Model_Training_Builder.cpp:43]
 * Initializing Dictionary from lda.dict.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:00.945226 20947 Unigram_Model_Training_Builder.cpp:45]
 * Dictionary Initialized 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:00.998955 20947 Unigram_Model_Tester.cpp:33] Initializing
 * Word-Topic counts table from dump ../ut_out/unigram/lda.ttc.dump
 * using 3506 words &amp; 100 topics. 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.029986 20947 Unigram_Model_Tester.cpp:51] Initialized
 * Word-Topic counts table 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.030045 20947 Unigram_Model_Tester.cpp:55] Initializing Alpha
 * vector from dumpfile ../ut_out/unigram/lda.par.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.030118 20947 Unigram_Model_Tester.cpp:57] Alpha vector
 * initialized 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.030144 20947 Unigram_Model_Tester.cpp:60] Initializing Beta
 * Parameter from specified Beta = 0.01 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.030186 20947 Unigram_Model_Tester.cpp:63] Beta param
 * initialized 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.036779 20947 Testing_Execution_Strategy.cpp:24] Starting
 * Parallel testing Pipeline 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.329864 20947 Testing_Execution_Strategy.cpp:36] Iteration 0
 * done. Took 0.00488273 mins 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.354588 20947 Unigram_Model.cpp:92] Average num of topics
 * assigned per word = 3.37222 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.354617 20947 Testing_Execution_Strategy.cpp:41] &gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;
 * Log-Likelihood (model, doc, total): -1.93551e+06 , -57496.6 ,
 * -1.99301e+06 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.358059 20947 Testing_Execution_Strategy.cpp:49] Parallel
 * testing Pipeline done 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.360312 20947 Unigram_Model.cpp:105] Saving model for test
 * pipeline in lda.ttc.dump and lda.par.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.375068 20947 Unigram_Model.cpp:108] Model saved 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.375098 20947 Unigram_Model.cpp:117] Writing top words
 * identified per topic into lda.topToWor.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.380234 20947 Unigram_Model.cpp:139] Word statistics per
 * topic written 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.380290 20947 Unigram_Model_Training_Builder.cpp:126] Saving
 * document to topic-proportions in lda.docToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.380311 20947 Unigram_Model_Training_Builder.cpp:127] Saving
 * word to topic assignment in lda.worToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.401484 20947 Unigram_Model_Training_Builder.cpp:193]
 * Document to topic-proportions saved in lda.docToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 22:22:01.401512 20947 Unigram_Model_Training_Builder.cpp:194] Word to
 * topic assignment saved in lda.worToTop.txt 
 * </P>
 * </CODE>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Note the following flags:</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">1. test: This indicates the
 * batch test mode</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">2. dumpprefix: This flag
 * gives the prefix for the binary files storing the model in our case,
 * ../ut_out/lda. 'lda' is the default prefix used by the framework when
 * none is specified. If you have specified a different prefix here is
 * the place to use it. This will add different suffixes to fetch the
 * various binary files to fetch the model saved and use them to infer
 * the topic mixture for your new documents.</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">3. topics: This flag
 * indicates the number of topics that your trained model contained. The
 * same number should be used for your new documents.</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <CODE>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">$
 * ls -al lda*</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  40059 2011-04-17 22:16 lda.dict.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm 204655 2011-04-17 22:16 lda.dict.dump.global 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  18947 2011-04-17 22:22 lda.docToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm    816 2011-04-17 22:22 lda.par.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  62380 2011-04-17 22:22 lda.top 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  38389 2011-04-17 22:22 lda.topToWor.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  83799 2011-04-17 22:22 lda.ttc.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm  36374 2011-04-17 22:16 lda.wor 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">-rw-r--r--
 * 1 shravanm shravanm 200546 2011-04-17 22:22 lda.worToTop.txt 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * </CODE>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">This is pretty much the same
 * output that you see when you learnt the model but for your new
 * documents.</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * \subsection stream_mode Streaming Mode
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm"> Again assuming that PWD=$LDA_HOME/ut_test</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">In this there is no need for
 * formatting the documents but you need to provide the dictionary as an
 * additional parameter. You also need to specify the maximum memory in
 * MBs that you can allocate to store the model. You can tokenize and
 * directly pipe the raw text documents through 'learntopics'. 
 * </P>
 * <CODE>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">$
 * java Tokenizer | ../learntopics -teststream
 * --dumpprefix=../ut_out/unigram/lda --topics=100
 * --dictionary=../ut_out/unigram/lda.dict.dump --maxmem=128</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.342099 21004 Controller.cpp:68]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.342605 21004 Controller.cpp:69] Log files are being stored
 * at /home/shravanm/workspace/LDA_Refactored/ut_test/learntopics.* 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.342627 21004 Controller.cpp:70]
 * ----------------------------------------------------------------------
 * 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.343189 21004 Controller.cpp:92] You have chosen single
 * machine testing mode 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.343723 21004 Unigram_Model_Streaming_Builder.cpp:20]
 * Initializing global dictionary from ../ut_out/unigram/lda.dict.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.398543 21004 Unigram_Model_Streaming_Builder.cpp:23]
 * Dictionary initialized and has 17208 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.398643 21004 Unigram_Model_Streaming_Builder.cpp:49]
 * Estimating the words that will fit in 128 MB 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.486726 21004 Unigram_Model_Streaming_Builder.cpp:52] 17208
 * will fit in 1.05881 MB of memory 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.486809 21004 Unigram_Model_Streaming_Builder.cpp:53]
 * Initializing Local Dictionary from ../ut_out/unigram/lda.dict.dump
 * with 17208 words. 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.562101 21004 Unigram_Model_Streaming_Builder.cpp:82] Local
 * Dictionary Initialized. Size: 34416 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.565176 21004 Unigram_Model_Streamer.cpp:27] Initializing
 * Word-Topic counts table from dump ../ut_out/unigram/lda.ttc.dump
 * using 17208 words &amp; 100 topics. 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.608408 21004 Unigram_Model_Streamer.cpp:45] Initialized
 * Word-Topic counts table 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.608469 21004 Unigram_Model_Streamer.cpp:49] Initializing
 * Alpha vector from dumpfile ../ut_out/unigram/lda.par.dump 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.608543 21004 Unigram_Model_Streamer.cpp:51] Alpha vector
 * initialized 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.608571 21004 Unigram_Model_Streamer.cpp:54] Initializing
 * Beta Parameter from specified Beta = 0.01 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.608608 21004 Unigram_Model_Streamer.cpp:57] Beta param
 * initialized 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:24.615538 21004 Testing_Execution_Strategy.cpp:24] Starting
 * Parallel testing Pipeline 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">www.sauritchsurfboards.com/
 * recreation/sports/aquatic_sports watch out jeremy sherwin is here
 * over the past six months you may have noticed this guy in every surf
 * magazine published jeremy is finally getting his run more.. copyright
 * surfboards 2004 all rights reserved june 6 2004 new launches it s new
 * and improved site you can now order custom surfboards online more
 * improvements to come.. top selling models middot rocket fish middot
 * speed egg middot classic middot squash 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">www.sauritchsurfboards.com/ recreation/sports/aquatic_sports        (watch,83)
 * (past,86) (months,77) (noticed,15) (guy,93) (surf,35) (magazine,86)
 * (published,92) (finally,49) (run,21) (copyright,62) (surfboards,27)
 * (rights,90) (reserved,59) (june,63) (launches,26) (improved,40)
 * (site,26) (order,72) (custom,36) (surfboards,11) (online,68)
 * (improvements,67) (top,29) (selling,82) (models,30) (middot,62)
 * (rocket,23) (fish,67) (middot,35) (speed,29) (egg,2) (middot,22)
 * (classic,58) (middot,69) (squash,67) 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">www.semente.pt
 * recreation/sports/aquatic_sports por desde de 1999 para este site com
 * o para browsers 4 ou superior de prefer ecirc ncia o internet
 * explorer aqui gr aacute tis este site foi e pela think pink
 * multimedia 2000 surfboards all rights reserved 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">www.semente.pt      recreation/sports/aquatic_sports        (por,83)
 * (de,86) (para,77) (site,15) (para,93) (browsers,35) (superior,86)
 * (de,92) (prefer,49) (internet,21) (explorer,62) (aqui,27) (gr,90)
 * (aacute,59) (site,63) (pink,26) (multimedia,40) (surfboards,26)
 * (rights,72) (reserved,36)</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:38.769309 21004 Testing_Execution_Strategy.cpp:36] Iteration 0
 * done. Took 0.235894 mins 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:38.875191 21004 Unigram_Model.cpp:92] Average num of topics
 * assigned per word = 1.85268 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:38.875252 21004 Testing_Execution_Strategy.cpp:41] &gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;
 * Log-Likelihood (model, doc, total): -2.52084e+06 , -194.288 ,
 * -2.52103e+06 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">W0417
 * 23:06:38.875350 21004 Testing_Execution_Strategy.cpp:49] Parallel
 * testing Pipeline done 
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * </CODE>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Note the flags used:</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">1. teststream: Indicates the
 * streaming test mode</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">2. dictionary: The
 * dictionary of words that the model knows about. Only those words in
 * the test data set are recognized and the rest are ignored.</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">3. maxmemory: In MB. Denotes
 * the amount of memory you allocate to store the model.</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">The rest are same as batch
 * mode.</P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <P ALIGN=LEFT STYLE="margin-bottom: 0cm">Here there is no other
 * output that you need to look for. The topic assignments are dumped
 * back to stdout. The word mixtures for the topics are the same as that
 * in the model.</P>
 * \section generated_output Output Generated
 *              <OL>
 *                      <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.wor: This file
 *                      generated by 'formatter' is the document in the protobuffer format
 *                      with words replaced by their indices</P>
 *                      <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.top: This file
 *                      generated by 'learntopics' contains the current topic assignments
 *                      for all the documents in the protobuffer format</P>
 *                      <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.dict.dump: This
 *                      file is a binary dump of the dictionary generated by the
 *                      'formatter' program. This will be implicitly used by the
 *                      'learntopics' program</P>
 *                      <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.ttc.dump: This
 *                      file generated by 'learntopics' is the binary dump of the
 *                      word-topic counts table. This is the state at the end of all the
 *                      iterations and essentially represents all the training that has
 *                      happened through all the iterations. It is very important as this
 *                      is a necessary input to the test pipeline.</P>
 *                      <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.par.dump: This
 *                      file  generated by 'learntopics' is the binary dump of the
 *                      parameters specified by the user which might be modified by an
 *                      optimization step</P>
 *                      <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.chk: This file
 *                      generated by 'learntopics' is a checkpoint keeping the current
 *                      iteration and some metadata</P>
 *                      <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.*.txt: These are
 *                      for human consumption and there are three of these generated by
 *                      'learntopics':</P>
 *                      <OL>
 *                              <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.topToWor.txt –
 *                              The word mixtures for each topics</P>
 *                              <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.worToTop.txt –
 *                              The topic assignments for every document on a per word basis</P>
 *                              <LI><P ALIGN=LEFT STYLE="margin-bottom: 0cm">lda.docToTop.txt –
 *                              The topic proportions for every document</P>
 *                      </OL>
 * </OL>
 * \section customizations Customization
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">In its simplest form, all
 * the parameters to 'formatter' and 'learntopics' have sensible default
 * values which practically allows them to be used without any
 * arguments. However, if you need to customize your setup then the
 * following options are available:</P>
 * <UL>
 *      <LI><P ALIGN=JUSTIFY>I/O Customization: By default all files output
 *      by the formatter, which are also the input to the learnTopcis
 *      program and all files output by the learntopics program use the
 *      prefix <STRONG>lda</STRONG>. This can be easily customized using the
 *      <STRONG>outputprefix</STRONG> &amp; <STRONG>inputprefix</STRONG>
 *      flags respectively. But while this customization is used, one needs
 *      to be careful about keeping the <STRONG>outputprefix</STRONG> &amp;
 *      <STRONG>inputprefix</STRONG> same. 
 *      </P>
 *      <LI><P ALIGN=JUSTIFY>Parameter Customization: The weights of the
 *      Dirichlet Conjugates for both topics(alpha) &amp; words(beta) can be
 *      changed by the <STRONG>alpha</STRONG> &amp; <STRONG>beta</STRONG>
 *      flags. 
 *      </P>
 *      <LI><P>Diagnostics &amp; Optimization: By default, the code performs
 *      alpha optimization every 25 iterations after burn in and also prints
 *      the log likelihood every 25 iterations. These are customizable using
 *      <STRONG>optimizestats</STRONG>, <STRONG>printloglikelihood</STRONG>
 *      &amp; <STRONG>burnin</STRONG> flags. 
 *      </P>
 *      <LI><P>Initialization Options: By default, we do random initialization
 *      for the topic assignments in the first iteration. However, we can be a
 *      bit smarter about this. Instead of random assignments, we start out
 *      with no topic assignments and an empty word-topic counts tables.
 *      The sampling depends entirely on the smoothing mass for the first few
 *      documents and subsequent documents use the counts table built so far.
 *      This is similar in style to sequential monte carlo. You can use the
 *      \'online\' flag to signal this. We have seen that this leads to faster
 *      convergence.
 * </UL>
 */
