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
/** \mainpage Y!LDA Topic Modelling Framework
 * <H1 CLASS="western">What is Topic Modelling?</H1>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">
 * It is a Machine Learning technique to categorize data. If we have
 * to group the home pages of Singapore Airline, National University
 * of Singapore & Chijmes which is a restaurant in Singapore, we can
 * group them as all belonging to Singapore. Now if we have more pages
 * to group lets say, United Airlines, Australian National University
 * and a restaurant in Berkeley, then we can group the combined set of
 * pages in multiple ways: by country, by type of business and so on.
 * Choosing one of these different ways is hard because each one of
 * them has multiple roles to play depending on the context. In a
 * document that talks about nations strengths then the grouping by
 * nationality is good and in some document which talks about universties
 * its apt to use the grouping by type of business.
 * So the only alternative is to assign or tag each page with all the
 * categories it belongs to. So we tag the United Airlines page as
 * an airliner company in US and so on.</P>
 *
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">So whats the big
 * difference between grouping objects or clustering & Topic Models?
 * The following example clarifies the distinction:
 * Consider objects of different colors. Clustering them is to find
 * that there are 3 prototypical colors RGB & each object can be grouped
 * by what its primary color is. That is we group object by prototypes.
 * On the other hand with topic models we try to find the the composition
 * of RGB in the color of each object, that is we say that this color
 * is composed of 80% R, 9% G & 11% B. So topic models are definitely
 * richer in the sense that any color can be decomposed into the
 * prototypical colors but not all colors can be unambiguously grouped</P>
 *
 * <H1 CLASS="western">What is Y!LDA topic modelling framework?</H1>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Though conceptually it
 * sounds very good, to make it work on 100s of millions of pages,
 * with 1000s of topics to infer & no editorial data is a very hard
 * problem. The state of the art can only handle sizes that are 10
 * to 100 times smaller. We would also like the solution to scale
 * with the number of computers so that we can add more machines
 * and be able to solve bigger problems.</P>
 *
 * <P>One way of solving the problem of Topic Modelling is called
 * <a href="http://www.cs.princeton.edu/~blei/papers/BleiNgJordan2003.pdf">Latent Dirichlet Allocation</a>.
 * This is a statistical model which specifies a probabilistic procedure
 * to generate data. It defines a topic as a probability distribution over words.
 * Essentially think of topics as having a vocabulary of their own
 * with preference over words specified as a probability distribution.</P>
 *
 * <P>We have implemented a framework to solve the topic modelling
 * problem using LDA which can work at very large scale. Considerable
 * effort has also been spent on creating an architecture for the
 * framework which is flexible enough to allow the reuse of infrastructure
 * for the implementation fancier models and extension. One of the
 * main aims here is that scaling a new model should take minimal effort.
 * For more details please take a look at
 * <a href="http://portal.acm.org/citation.cfm?id=1920931">An Architecture for Parallel Topic Models</a>
 * </P>
 *
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <H1 CLASS="western">What does the framework provide?</H1>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">It provides a fast C++
 * implementation of the inferencing algorithm which can use both
 * multi-core parallelism and multi-machine parallelism using a hadoop
 * cluster. It can infer about a thousand topics on a million document
 * corpus while running for a thousand iterations on an eight core
 * machine in one day.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <H1 CLASS="western">What are the requirements?</H1>
 * <H2 CLASS="western">Hardware Requirements:</H2>
 * <P><B>Small Corpus of the order of thousands of documents:</B> Dual
 * Core machines with 4-8 GB of RAM might be sufficient. Of course you
 * can run the code on larger document sets but you will have to wait
 * longer or cut down on the number of iterations.</P>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>Large Corpus of the
 * order of millions of documents:</B>50 to 100 multi-Core (quad cores,
 * dual quad cores, etc) machines with 8 to 16 GB of RAM can give good
 * performance.</P>
 * <H2 CLASS="western">Software Requirements:</H2>
 * <P>The code has been mainly tested on the linux platform. If you want to
 * install on other platforms, since the source and libraries along with
 * source are provided, you can try compiling them and let us know if it
 * works.</P>
 * <H3 CLASS="western">Dependencies:</H3>
 * <P>The code has dependencies on a number of libraries. To facilitate
 * distribution we have hosted the libraries separately. The install script
 * shipped with the code fetches the sources for the associated libraries and
 * builds them on the developer's machine.
 * The following is the list: <BR>
 * <OL>
 * <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>Ice-3.4.1.tar.gz </B><BR>
 * An efficient inter process communication framework which is used for
 * the distributed storage of (topic, word) tables.</P>
 * <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>cppunit-1.12.1.tar.gz </B><BR>
 * C++ unit testing framework. We use this for unit tests.</P>
 * <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>mcpp-2.7.2.tar.gz </B><BR>
 * C++ preprocessor</P>
 * <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>boostinclude.tar.gz </B><BR>
 * Boost libraries (various datatypes)</P>
 * <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>gflags-1.2.tar.gz </B><BR>
 * Google's flag processing library (used for commandline options)</P>
 * <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>protobuf-2.2.0a.tar.gz </B><BR>
 * Protocol buffers (used for serializing data to disk and as internal
 * key data structure). Google's serialization library</P>
 * <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>bzip2-1.0.5.tar.gz </B><BR>
 * Data compression</P>
 * <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>glog-0.3.0.tar.gz </B><BR>
 * Logfile generation (Google's log library).</P>
 * <LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><B>tbb22_20090809oss.tar.gz </B><BR>
 * Intel Threading Building Blocks. Multithreaded processing
 * library. Much easier to use than pthreads. We use the pipeline
 * class.</P>
 * </OL>
 * All the libraries except Ice should install without
 * problems. With Ice, there are a lot dependencies involved and our
 * automated build script might not work in your set-up. If so please
 * install Ice manually and copy the required includes & libs into
 * the Yahoo_LDA directory</B></P>
 * <H1 CLASS="western">How do you install?</H1>
 * <OL>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Run make from the
 * 	directory where you have checked out the source.</P>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">This will first
 * 	install all the required libraries locally in the same directory. It
 * 	will then compile the source code to generate the binaries described
 * 	next.</P>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Please check for a
 * 	proper install before if you see compilation or linkage errors.
 * 	There may be issues with installation of Ice. Care has been taken
 * 	that the install happens on most well set-up machines but if it
 * 	fails, we recommend installing Ice manually and copying the include
 * 	files and the libraries to the include &amp; lib directories in the
 * 	Y!LDA install path. After this the compilation should go through
 * 	fine.</P>
 * </OL>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <H1 CLASS="western">What are the binaries that get installed and what
 * do you use them for?</H1>
 * <OL>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">formatter : Used to
 * 	format the raw text corpus into a binary format on which learntopics
 * 	can be run. This is a preprocessing step that allows one to run
 * 	learntopics many times on the same corpus. It also decreases the on
 * 	disk size of the raw text corpus.</P>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">learntopics: Used to
 * 	learn/infer the topics from the corpus and represent the documents
 * 	in the corpus as mixture of these topics.</P>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">DM_Server: This is
 * 	the server that implements a distributed hash table that is used to
 * 	store the global counts table while running the multi-machine
 * 	version of the code.</P>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Merge_Dictionaries:
 * 	This is used to build the global dictionary by merging the local
 * 	dictionaries</P>
 * 	<LI><P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Merge_Topic_Counts:
 * 	This is used to dump the global counts table to disk.</P>
 * </OL>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm"><BR>
 * </P>
 * <H1 CLASS="western">What are the scripts available and what do you do
 * with them?</H1>
 * <P ALIGN=JUSTIFY STYLE="margin-bottom: 0cm">Scripts are available for
 * the use of build system and the multi-machine setup with Hadoop. The
 * build system uses the bin/create*.sh scripts to find out files &amp;
 * directories. The multi-machine setup has the runLDA.sh, Formatter.sh
 * &amp; LDA.sh scripts that launch the hadoop-streaming job for doing
 * the distributed inference. Another script that helps you to organize
 * your corpus on hdfs is splitter.sh</P>
 * <P/>
 * <H1 CLASS="western">How do I use Y! LDA?</H1>
 * \ref usage
 * <P/>
 * <H1 CLASS="western">Where to find the developer documentation?</H1>
 * \ref architecture & the code documentation made available through
 * Doxygen
 * <P/>
 */
