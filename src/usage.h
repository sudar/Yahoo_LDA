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
/**
 * \page usage Using Y!LDA
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">The main purpose of the
 * Y!LDA framework is to allow you to infer the set of constituents that
 * can be used to represent your documents as mixtures of the inferred
 * constituents. This is also called learning a model for you corpus.
 * This takes a significant amount of time.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Once a model has been
 * learnt, you can also use it to find out what topics a new document is
 * made up by using LDA in test mode. In this mode, you provide the
 * learnt model as a parameter and use that model to infer the topic
 * mixture for new documents. This is very fast compared to learning the
 * model. This can be done in two ways:</P>
 * <OL>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Batch mode: If you
 * 	have a bunch of new documents for which you want to infer the topic
 * 	mixture you use this mode.</P>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Streaming mode: If
 * 	you have documents which are to be streamed through the binary
 * 	generating the topic mixture instantly, then this mode is for you.</P>
 * </OL>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">The main difference
 * between the two modes is that, in batch mode, the model is loaded for
 * every batch of documents whereas in streaming mode, the model is
 * loaded once and you can keep streaming the documents. In batch mode,
 * you wait till all the documents in the batch are assigned topic
 * mixtures whereas in streaming mode the topic mixture is assigned on a
 * per document basis. In streaming mode, only a part of the model is
 * loaded depending on how much memory you allocate for storing the
 * model. However, we ensure that the model for the most important words
 * (words that occurr more frequently) are loaded before loading the
 * model for lesser frequent words till the allocated is memory is used
 * up. The model loading time can be significant if its big. Hence the
 * case for a streaming mode for those of you who do not want to bear
 * the cost of loading a big model every time you want to infer topic
 * mixtures on new documents.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Each of these is
 * described in detail with an illustration below in the next
 * subsection.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">The other dimension to
 * consider is the size of the data. If your corpus has a couple of
 * million documents and you are willing to wait for a couple of days
 * then all you need to know is the single machine setup where all the
 * data is on a single machine (usually multi-core) and you run LDA on
 * that machine.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">If your corpus is larger
 * than that, you have no choice but to resort to the multi-machine
 * setup. Here you split your data amongst multiple machines (possibly
 * each machine being a multi-core one) and run LDA on all those
 * machines. The Y!LDA framework currently provides a hadoop based
 * distributed LDA mechanism that uses a custom implemented distributed
 * hash table. The framework provides hadoop-streaming scripts that do
 * the entire thing (splitting, training, testing) in a distributed
 * fashion on the hadoop cluster backed by the HDFS as the distributed
 * file system where your data is stored. If you do not have a hadoop
 * cluster at your disposal, we currently do not provide any scripts to
 * run things automatically. You will have to distribute the data
 * manually, copy the binary version, if its a homogenous setup, else
 * install and run the framework on all the nodes containing data and
 * finally execute binaries to merge the distributed parts of the model.
 * We will only detail the steps to achieve them. Perhaps, one can
 * contribute some scripts back so that this can be done automatically!</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <H2 CLASS="western">Single Machine Setup:</H2>
 * \ref single_machine_usage "Single Machine Usage"
 * <H2 CLASS="western">Multi-Machine Setup:</H2>
 * \ref multi_machine_usage "Multi Machine Usage"
 */
