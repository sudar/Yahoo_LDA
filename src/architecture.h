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
 * \page architecture Y!LDA Architecture
 * \section intro Introduction
 * Please refer the Main Page for an introduction
 *
 * \section goals Goals
 * The approach to do topic modelling is to have a graphical model representing
 * the generative assumptions the user has about the corpus. A graphical model is a
 * probabilistic model representing the joint distribution of the random
 * variables involved with a graph denoting the conditional independence
 * assumptions amongst them. Solving the model is to infer the parameters of
 * the model by processing the actual data. Doing this inference is the
 * hardest part of the approach.
 *
 * \subsection new_models Adding new Models
 * There are a lot of variations on the basic LDA model and with each variation
 * the inferencing logic changes. The parameters, the sufficient statistics that
 * need to be maintained everything will be slightly different. One of the main
 * goals of this framework is to make the job of adding new models simpler.
 *
 * \subsection infrastructure Common Infrastructure
 * One task that is mostly common across multiple models is the infrastructure
 * needed to store documents, load them, create a pipeline that can optimally
 * utilize multi-core parallelism. In the framework we aim to standardize on
 * proven infrastructure that is known to provide efficient implementations so
 * that the model writer just worries about adding only the parts that are
 * relevant for doing the inference
 *
 * \subsection scalability Scalability
 * Another main aspect of this framework is to substantially increase the scale
 * of the state of the art by utilizing parallelism both multi-core and
 * multi-machine.
 *
 * \section components Main components of the System
 * Y!LDA uses the Gibbs sampling approach popularized by
 * <a href="http://dx.doi.org/10.1073%2Fpnas.0307752101">Collapsed Gibbs Sampling</a>.
 * There are four main components in this approach:
 * <OL>
 * <LI><B>Model:</B><BR/>
 * This encapsulates the parameters of the model and the sufficient
 * 				statistics that are necessary for inference
 * <LI><B>Model_Refiner:</B><BR/>
 * This encapsulates the logic needed for refining the initial
 * 				model which involves streaming the documents from disk, sampling
 * 				new topic assignments, updating the model, performing diagnostics
 * 				and optimization and writing the documents back to disk
 * <LI><B>Pipeline:</B><BR/>
 * As can be seen above, the refiner does a sequence of operations on
 * 				every document of the corpus. Some of them have to run serially but
 * 				some others can be run parallely. To enable exploiting multi-core
 * 				parallelism, the Pipeline is defined to be composed of a set of
 * 				operations called filters which can either be declared to be run
 * 				serially or parallely. The Pipeline comes with a scheduler that
 * 				schedules the threads available on the machine to run these filters
 * 				in an optimal fashion
 * <LI><B>Execution_Strategy:</B><BR/>
 * This encapsulates the strategy that decides what filters
 * 				a pipeline is composed of, how many times the documents are passed
 * 				through the pipeline.
 * </OL>
 * \subsection builder Builder Pattern
 * The Builder pattern fits very well for this approach. We implement a Model_Builder
 * that builds the last three components depending on what model is needed and what
 * mode the model is supposed to operate in.
 *
 * The Model_Builder creates an initial Model and creates the required Model_Refiner by
 * passing the Model(or the necessary components of the Model). It then creates a Pipeline
 * and an Execution_Strategy as per the mode of operation.
 *
 * The Director is pretty straightforward. It directs the given Model_Builder to create
 * the necessary components and executes the defined Execution_Strategy. This refines the
 * initial Model created by the builder into one that reflects parameters tuned to the
 * corpus on which the Model was refined on. Then the Model is stored on disk for testing.
 *
 * \section multi-machine Distributed Set Up
 * To cater to the Scalability goals, as detailed in
 * <a href="http://portal.acm.org/citation.cfm?id=1920931">An Architecture for Parallel Topic Models</a>,
 * the framework implements a
 * Distributed Memory based multi-machine setup that exploits multi-machine parallelism
 * to the fullest. The main idea being that the inferencing happens locally while the
 * state variables are kept up-to-date with a global copy that stored using a Distributed
 * HashTable. To come up with an efficient distributed set up is a difficult thing and
 * we definitely do not want people reinvent the wheel here. So the framework tries to
 * abstract the mechanism of distribution, the implementation of an efficient distributed
 * HashTable and the mechanism needed for Synchrnoization.
 *
 * \subsection distributed_map Distributed_Map
 * The framework implements a Distributed_Map interface using Ice as a very efficient
 * middleware. It essentially provides both a Server and Client implementation.
 * <OL>
 * <LI>
 * <B>DM_Server:</B><BR/>
 * The server essentially hosts a chunk of the distributed hash table and supports the
 * usual map operations. It also supports three special operations:
 * <UL>
 * <LI>Put: Which accumulates the values instead of replacing</LI>
 * <LI>waitForAll: Which is a barrier implementation using AMD</LI>
 * <LI>PutNGet: which is an asynchronous call that accumulates
 *              the passed value into the existing one and returns
 * 				the final value back to the caller through
 * 				a call back mechanism
 * </LI>
 * </UL>
 * </LI>
 * <LI>
 * <B>DM_Client:</B> <BR/>
 * A client that supports a single hash table view of the distributed system. The client
 * transparently supports a rate limited, sliding-window based Asynchronous Method Invocation
 * for the PutNGet which is a very useful operation to have for effective Synchronization.
 * Refer to the VLDB paper for more information.
 * </LI>
 * </OL>
 *
 * For most models, one need not worry about modifying the above. These only need to be
 * used most of the times without bothering much about their implementation.
 *
 * \subsection synchronizer Synchronizer
 * The framework provides a default implementation of the Synchronization strategy detailed
 * in [2]. The Synchronizer is run as a separate background thread apart from the main
 * threads that do the inferencing. The actual task of synchronization is left to the
 * implementation of a Synchronizer_Helper class. The Sychronizer only creates slots for
 * synchronization and asks the helper to synchronize in those slots. It also takes care
 * of running the Synchronization only till the inferencing is done.
 *
 * However, there is a strong assumption that the synchronization proceeds in a linear
 * fashion. That is the structures being synchronized are linear and can be synchronized
 * one after the other. This is implicit in the Synchronizer's creation of slots.
 *
 * \subsection synchronizer_helper Synchronizer_Helper
 * Every model has to only provide the Synchronizer_Helper implementation which spills
 * the logic for synchronizing the model's relevant structures, maintains copies of them
 * where needed and provides the callback function for the AMI putNGet.
 *
 * \section default_impl Default Implementations provided
 * The framework provides default implementations for the Pipeline interface and the
 * Execution_Stratgey interface.
 * <OL>
 * <LI><B>TBB_Pipeline:</B><BR/>
 * This implementation uses Intel's Threading Building Blocks for
 * 					providing the Pipeline interface.
 * <LI><B>Training_Execution_Strategy:</B><BR/>
 * The default implementation of Execution_Strategy for
 * LDA training. Assembles the following pipeline for data flow:
 * \image html data_flow.png
 * \image latex data_flow.eps
 * <LI><B>Synchronized_Training_Execution_Strategy:</B><BR/>
 * The default implementation of
 * 					Execution_Strategy that extends Training_Execution_Strategy and
 * 					adds Synchronization capability
 * <LI><B>Testing_Execution_Strategy:</B><BR/>
 * The default implementation of Execution_Strategy for
 * 					LDA testing.
 *
 * \section unigram Unigram Model
 * The framework also provides the Unigram_Model implementations of the various common
 * interfaces. This is the basic LDA model with the bag of words assumption. Please
 * take a look at how the various interfaces are implemented. The main implementation
 * needed is for Model & Model_Refiner. Additionally, it implements efficient sparse
 * data structures to store the sufficient statistics.
 *
 * \section new_model Adding a new Model
 * Please use the Unigram_Model implementation as an example to implement new models
 *
 * \section chkpt Checkpoints
 * The framework also provide checkpointing functionality for the multi-machine setup
 * in order to provide failure recovery. This is implemented by an external object
 * that knows how to do three things: a. Serialize metadata to disk b. load previously
 * serialized metadata on request c. Serialize the datastructures to disk
 *
 * An appropriate checkpointer is passed as an argument while creating an Execution_Strategy
 * The strategy uses checkpointers to checkpoint at regular intervals. At startup, it also
 * checks if any checkpoints are available and if so, it starts up from that checkpoint.
 *
 * Different checkpointers are needed for different setups. For ex., the framework uses
 * the Local Checkpointer when running in single machine mode which only involves writing
 * the iteration number as metadata. All other data needed for restart is already being
 * serialized. However, for the multi-machine setup, a different mechanism is needed and
 * a Hadoop Checkpointer is implemented.
 *
 * This is an ongoing effort and we will add more stuff both to the code and documentation.
 * We definitely need your help & contribution in making this better.
 *
 * Here is an initial set of TODOs:
 *
 * \todo Add unit tests to make the code more robust
 * \todo Add more code documentation for the Unigram_Model components
 * \todo Implement fancier models in later versions
 * \todo Implement extensions to the LDA model in later versions
 *
 * These are in no particular order and we might re-prioritize later. Please mail me if
 * you are interested in contributing
 *
 * We shall use the git pull request (fork + pull model) for collaborative development.
 */
