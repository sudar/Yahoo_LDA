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
 * constants.h
 *
 *  Created on: 24 Mar, 2009
 *      
 */
#ifndef CONSTANTS_H_
#define CONSTANTS_H_
#include "types.h"

//The Current Version of Code. Dump files and other
//configs will have to be consistent with this version
//for their use
static const double CURRENT_VERSION = 1.1;

//Num of tokens in flight which is specified for TBB
static const size_t NUM_SAMPLERS = 50000;

//Used to do upfront memory reservation for the updates vector
//which is the vehicle used to transport msgs from the sampler
//to the updater
static const size_t EXP_NUM_WORDS_PER_DOC = 1000;

static const size_t BITS_PER_BYTE = 8;

//Max size permitted for the protobuffer msg
//msgs are written as binary files in (size, msg)* format
static const size_int MAX_MSG_SIZE = 10000000;

//Interval for logging a status msg to INFO
static const int PRT_CNT_INTERVAL = 50000;

//Default value of alphabar
static const double ALPHA_SUM = 50.;

//Default value for beta
static const double BETA = 0.01;

//Default value for gamma
static const double GAMMA = 0.01;

//Default value for delta
static const double DELTA = 0.01;

//when alpha optimization is turned on, alphas
//are always >= MIN_ALPHA
static const double MIN_ALPHA = 0.01;

static const word_t NUM_WORDS_PER_TOP = 20;

static const word_t NUM_TOPICS_PER_DOC = 10;

static const word_t MAX_MSGS = 10;

//The number of Random Number Generators(RNGs)
//used to simulate one RNG per thread behaviour
//as TBB does not allow thread local state. We
//create NUM_RNGS RNGs and access one of them
//pseudo-randomly
static const uint16_t NUM_RNGS = 32;

//Used as the mask instead of mod operator
static const uint16_t RNG_MASK = (1 << 5) - 1;

//Max allowed memory size. If you try allocating
//more than this we will abort
static const uint64_t MAX_MEMORY_USAGE = ((uint64_t) 1) << 34;

//If you try allocating more than this much memory,
//you will be warned that there might be something
//going wrong. If you are using a very large data set
//this is expected
static const uint32_t WARN_MEMORY_SIZE = 1 << 31;

//Max number of attempts before we give up on
//acquiring a lock. This is relevant for the
//multi-machine code
static const uint16_t LOCK_MAX_TRIES = 60;

//This is the maximum number of attempts before
//we give up on waiting for all clients to init.
//This is relevant for the multi-machine code
static const uint16_t INIT_COM_MAX_TRIES = 60;

//Expiry time in secs for the lock set on memcached
//This is relevant for the multi-machine code
static const time_t LOCK_EXPIRY_TIME = 10; //in secs

//This is a multiplier used with the LOCK_EXPIRY_TIME
//when we try to reacquire a lock
//This is relevant for the multi-machine code
static const uint16_t REACQUIRE_BUFFER = 2;

//List of stop words. These will be ignored in the input
static const std::string stopwords[] = { "a", "able", "about", "above",
        "according", "accordingly", "across", "actually", "after",
        "afterwards", "again", "against", "all", "allow", "allows", "almost",
        "alone", "along", "already", "also", "although", "always", "am",
        "among", "amongst", "an", "and", "another", "any", "anybody", "anyhow",
        "anyone", "anything", "anyway", "anyways", "anywhere", "apart",
        "appear", "appreciate", "appropriate", "are", "around", "as", "aside",
        "ask", "asking", "associated", "at", "available", "away", "awfully",
        "b", "be", "became", "because", "become", "becomes", "becoming",
        "been", "before", "beforehand", "behind", "being", "believe", "below",
        "beside", "besides", "best", "better", "between", "beyond", "both",
        "brief", "but", "by", "c", "came", "can", "cannot", "cant", "cause",
        "causes", "certain", "certainly", "changes", "clearly", "co", "com",
        "come", "comes", "concerning", "consequently", "consider",
        "considering", "contain", "containing", "contains", "corresponding",
        "could", "course", "currently", "d", "definitely", "described",
        "despite", "did", "different", "do", "does", "doing", "done", "down",
        "downwards", "during", "e", "each", "edu", "eg", "eight", "either",
        "else", "elsewhere", "enough", "entirely", "especially", "et", "etc",
        "even", "ever", "every", "everybody", "everyone", "everything",
        "everywhere", "ex", "exactly", "example", "except", "f", "far", "few",
        "fifth", "first", "five", "followed", "following", "follows", "for",
        "former", "formerly", "forth", "four", "from", "further",
        "furthermore", "g", "get", "gets", "getting", "given", "gives", "go",
        "goes", "going", "gone", "got", "gotten", "greetings", "h", "had",
        "happens", "hardly", "has", "have", "having", "he", "hello", "help",
        "hence", "her", "here", "hereafter", "hereby", "herein", "hereupon",
        "hers", "herself", "hi", "him", "himself", "his", "hither",
        "hopefully", "how", "howbeit", "however", "i", "ie", "if", "ignored",
        "immediate", "in", "inasmuch", "inc", "indeed", "indicate",
        "indicated", "indicates", "inner", "insofar", "instead", "into",
        "inward", "is", "it", "its", "itself", "j", "just", "k", "keep",
        "keeps", "kept", "know", "knows", "known", "l", "last", "lately",
        "later", "latter", "latterly", "least", "less", "lest", "let", "like",
        "liked", "likely", "little", "look", "looking", "looks", "ltd", "m",
        "mainly", "many", "may", "maybe", "me", "mean", "meanwhile", "merely",
        "might", "more", "moreover", "most", "mostly", "much", "must", "my",
        "myself", "n", "name", "namely", "nd", "near", "nearly", "necessary",
        "need", "needs", "neither", "never", "nevertheless", "new", "next",
        "nine", "no", "nobody", "non", "none", "noone", "nor", "normally",
        "not", "nothing", "novel", "now", "nowhere", "o", "obviously", "of",
        "off", "often", "oh", "ok", "okay", "old", "on", "once", "one", "ones",
        "only", "onto", "or", "other", "others", "otherwise", "ought", "our",
        "ours", "ourselves", "out", "outside", "over", "overall", "own", "p",
        "particular", "particularly", "per", "perhaps", "placed", "please",
        "plus", "possible", "presumably", "probably", "provides", "q", "que",
        "quite", "qv", "r", "rather", "rd", "re", "really", "reasonably",
        "regarding", "regardless", "regards", "relatively", "respectively",
        "right", "s", "said", "same", "saw", "say", "saying", "says", "second",
        "secondly", "see", "seeing", "seem", "seemed", "seeming", "seems",
        "seen", "self", "selves", "sensible", "sent", "serious", "seriously",
        "seven", "several", "shall", "she", "should", "since", "six", "so",
        "some", "somebody", "somehow", "someone", "something", "sometime",
        "sometimes", "somewhat", "somewhere", "soon", "sorry", "specified",
        "specify", "specifying", "still", "sub", "such", "sup", "sure", "t",
        "take", "taken", "tell", "tends", "th", "than", "thank", "thanks",
        "thanx", "that", "thats", "the", "their", "theirs", "them",
        "themselves", "then", "thence", "there", "thereafter", "thereby",
        "therefore", "therein", "theres", "thereupon", "these", "they",
        "think", "third", "this", "thorough", "thoroughly", "those", "though",
        "three", "through", "throughout", "thru", "thus", "to", "together",
        "too", "took", "toward", "towards", "tried", "tries", "truly", "try",
        "trying", "twice", "two", "u", "un", "under", "unfortunately",
        "unless", "unlikely", "until", "unto", "up", "upon", "us", "use",
        "used", "useful", "uses", "using", "usually", "uucp", "v", "value",
        "various", "very", "via", "viz", "vs", "w", "want", "wants", "was",
        "way", "we", "welcome", "well", "went", "were", "what", "whatever",
        "when", "whence", "whenever", "where", "whereafter", "whereas",
        "whereby", "wherein", "whereupon", "wherever", "whether", "which",
        "while", "whither", "who", "whoever", "whole", "whom", "whose", "why",
        "will", "willing", "wish", "with", "within", "without", "wonder",
        "would", "would", "x", "y", "yes", "yet", "you", "your", "yours",
        "yourself", "yourselves", "z", "zero" };

//The size of the above list
static const uint16_t NUM_STP_WRDS = sizeof(stopwords) / sizeof(stopwords[0]);

#endif /* CONSTANTS_H_ */
