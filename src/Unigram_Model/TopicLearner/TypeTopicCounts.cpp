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
 * TypeTopicCounts.cpp
 *
 *  Created on: 24 Mar, 2009
 *      
 */

#include <algorithm>

#include "TypeTopicCounts.h"
#include "DocumentReader.h"
#include "DocumentWriter.h"
#include "TopicLearner/Dirichlet.h"
#include "Context.h"

//#include "MemcClient.h"

bool cnt_cmp(packed_t i, packed_t j);
bool cnt_cmp_ttc(cnt_topic_t i, cnt_topic_t j);

/************************** Initialization routines ************************/
/**
 * Construct an empty Word_Topic Counts table
 */
TypeTopicCounts::TypeTopicCounts() {
    num_words = num_topics = 0;
    tokens_per_topic = NULL;
    wt = NULL;
    word_mutexes = NULL;
    topic_stats = NULL;
    top_topics = NULL;
}

/**
 * Constructs a Word Topic Counts table that will
 * have num_words_ unique words and each word can
 * be assigned a maximum of num_topics_ topics
 */
TypeTopicCounts::TypeTopicCounts(word_t num_words_, topic_t num_topics_) {
    num_words = num_words_;
    wt = new topicCounts*[num_words];
    for (word_t k = 0; k < num_words; k++) {
        wt[k] = new topicCounts();
    }
    word_mutexes = new word_mutex_t[num_words];
    for (word_t i = 0; i < num_words; i++) {
        word_mutexes[i] = word_mutex_t();
    }
    init(num_topics_);
}

TypeTopicCounts::~TypeTopicCounts() {

    if (word_mutexes) {
        for (word_t i = 0; i < num_words; i++) {
            word_mutexes[i].~word_mutex_t();
        }
        delete[] word_mutexes;
        word_mutexes = NULL;
    }

    if (wt) {
        for (word_t i = 0; i < num_words; i++) {
            delete wt[i];
            wt[i] = NULL;
        }
        delete[] wt;
        wt = NULL;
    }
    destroy();
}

void TypeTopicCounts::init(topic_t num_topics_) {
    num_topics = num_topics_;
    tokens_per_topic = new atomic<topic_t> [num_topics];
    for (topic_t k = 0; k < num_topics; k++)
        tokens_per_topic[k] = 0;

    topic_stats = new TopKList*[num_topics];
    for (int i = 0; i < num_topics; i++)
        topic_stats[i] = new TopKList(min(num_words, NUM_WORDS_PER_TOP));

    top_topics = new TopKList(num_topics);
}

void TypeTopicCounts::destroy() {
    if (tokens_per_topic) {
        delete[] tokens_per_topic;
        tokens_per_topic = NULL;
    }
    if (top_topics) {
        delete top_topics;
        top_topics = NULL;
    }

    if (topic_stats) {
        for (int i = 0; i < num_topics; i++)
            delete topic_stats[i];
        delete[] topic_stats;
        topic_stats = NULL;
    }
}

pair<int, float> TypeTopicCounts::estimate_fit(string fname,
        WordIndexDictionary* dict) {
    pair<int, float> retVal(0, 0.0f);
    word_t k = 0;
    DocumentReader doc_rdr(fname);
    TypeTopicCounts tmp_ttc;
    tmp_ttc.num_topics = Context::get_instance().get_int("topics");
    int incoming_words = tmp_ttc.verify_header(doc_rdr);
    LDA::unigram_document* doc = new LDA::unigram_document;
    float max_mem = MEM_LOAD_PER * Context::get_instance().get_double(
            "maxmemory");
    int* lengths = new int[incoming_words];
    while (doc_rdr.read(doc) != -1) {
        if (k < incoming_words) {
            TopicCounts tc;
            tc.setLength(doc->cnt_topics_size());
            lengths[k++] = tc.origLength;
        }
    }
    float used_memory = 0.0f;
    WordIndexDictionary tmp_dict;
    tmp_dict.initialize_from_dict(dict, true);
    for (int i = 0; i < incoming_words; i++) {
        int prev_id = tmp_dict.get_prev_index(i);
        used_memory += lengths[prev_id] * sizeof(packed_t) / (float) (1 << 20);
        //		cout << "i:" << i << " prev_id:" << prev_id << " length:" << lengths[prev_id] << " used_mem:" << used_memory << "    ";
        if (used_memory >= max_mem)
            return retVal;
        ++retVal.first;
        retVal.second = used_memory;
    }
    delete[] lengths;
    return retVal;
}

pair<int, float> TypeTopicCounts::estimate_fit(string fname, float used_memory,
        int& incoming_words) {
    pair<int, float> retVal(0, used_memory);
    word_t k = 0;
    DocumentReader doc_rdr(fname);
    TypeTopicCounts tmp_ttc;
    tmp_ttc.num_topics = Context::get_instance().get_int("topics");
    incoming_words = tmp_ttc.verify_header(doc_rdr);
    LDA::unigram_document* doc = new LDA::unigram_document;
    float max_mem = MEM_LOAD_PER * Context::get_instance().get_double(
            "maxmemory");
    while (doc_rdr.read(doc) != -1) {
        if (k++ < incoming_words) {
            TopicCounts tc;
            tc.setLength(doc->cnt_topics_size());
            used_memory += tc.origLength * sizeof(packed_t) / (float) (1 << 20);
            if (used_memory >= max_mem)
                return retVal;
            ++retVal.first;
            retVal.second = used_memory;
        }
    }
    return retVal;
}

/**
 * Used to estimate the amount of memory being
 * used while initializing this structure in order
 * to warn if it exceeds WARN_MEMORY_SIZE
 * and fail if it exceeds MAX_MEMORY_USAGE
 */
void TypeTopicCounts::estimate_memoryn_warn(long num_elems) {
    uint64_t estUsage = num_elems * sizeof(packed_t);

    LOG_IF(FATAL,estUsage> MAX_MEMORY_USAGE)<< "Trying to allocate " << estUsage<< " bytes of memory will be fatal. Quitting";

    LOG_IF(WARNING,estUsage> WARN_MEMORY_SIZE) << "Warning: Trying to allocate " << estUsage << " bytes of memory";
}

/**
 * Reads documents using DocumentReader. For each document
 * and for each word encountered, it updates the topic counts
 * for that word based on the topic assignment.
 */
void TypeTopicCounts::initialize_from_docs(string wfname, string tfname) {
    DocumentReader wdoc_rdr(wfname);
    DocumentReader tdoc_rdr(tfname);
    LDA::unigram_document doc;

    //INIT_TC_SIZE: This is the initial number of elements
    //assigned per topicCounts vector.
    estimate_memoryn_warn(num_words * INIT_TC_SIZE);

    while (wdoc_rdr.read(&doc) != -1) {
        LDA::unigram_document tdoc;
        tdoc_rdr.read(&tdoc);
        doc.MergeFrom(tdoc);
        for (word_t k = 0; k < doc.body_size(); k++) {
            word_t word = doc.body(k);
            topic_t topic = doc.topic_assignment(k);
            ++tokens_per_topic[topic];
            topicCounts* topCnts = wt[word];

            if (!topCnts->findAndIncrement(topic))
                topCnts->addNewTopAftChk(topic);
        }
    }
}

int TypeTopicCounts::verify_header(DocumentReader& doc_rdr) {
    header* hdr = new header;
    doc_rdr.read(hdr);
    double incoming_ver = hdr->version();
    LOG_IF(FATAL,incoming_ver > CURRENT_VERSION)<< "Version mismatch. File Version: " << incoming_ver << " LDA Version: " << CURRENT_VERSION;
    int incoming_words = -1;
    LOG_IF(FATAL,!hdr->has_num_words()) << "Num of words not found in the dump file header. Fatal and quitting";
    incoming_words = hdr->num_words();
    int incoming_topics = -1;
    if(hdr->has_num_topics()) incoming_topics = hdr->num_topics();
    LOG_IF(FATAL,incoming_topics!=-1 && incoming_topics!=num_topics) << "Num of topics in the dump file header(" << incoming_topics << ") is different from that we have been initialized with(" << num_topics << ")";
    delete hdr;
    return incoming_words;
}

void TypeTopicCounts::initialize_from_string(word_t word, string& counts) {
    topicCounts* tc = wt[word];
    tc->assign(counts.size() / sizeof(cnt_topic_t));
    tc->init(counts);
    for (int i = 0; i < tc->length; i++) {
        cnt_topic_t top_cnt = tc->items[i];
        topic_t top = top_cnt.choose.top;
        cnt_t cnt = top_cnt.choose.cnt;
        tokens_per_topic[top] += cnt;
    }
}

/**
 * Reads the serialize dump (fname) in the protobuffere format
 * into memory. The dump is generated using dump() method.
 * The DocumentReader is used to read from the protobuf
 * format dump file. There will be num_words entries in
 * the dump for topicCounts and the last entry is the n(t).
 * So first we read num_words topicCounts & then n(t)
 */
bool TypeTopicCounts::initialize_from_dump(string fname,
        WordIndexDictionary* local_dict, WordIndexDictionary* global_dict,
        size_t offset) {
    word_t k = 0;
    DocumentReader doc_rdr(fname);
    int incoming_words = verify_header(doc_rdr);
    LDA::unigram_document* doc = new LDA::unigram_document;

    //Print Memory Warning
    int pmw = incoming_words / 10 - 1;
    long totUsage = 0;
    int num_inited = 0;
    bool cont_init = true;
    while (doc_rdr.read(doc) != -1) {
        if (k < incoming_words) {
            int local_word_id = k;
            if (global_dict != NULL && local_dict != NULL) {
                string word = global_dict->get_word(k + offset);
                local_word_id = local_dict->get_index(word);
                if (local_word_id == -1) {
                    ++k;
                    continue;
                }
            } else
                local_word_id += offset;
            if (local_word_id >= num_words) {
                ++k;
                cont_init = false;
                break;
            }
            topicCounts* topCnts = wt[local_word_id];
            //The topicCounts structure uses a block allocate
            //methodology to avoid heap fragmentation. But dump
            //just dumps the actual number of entries. So first
            //block align the number of entries and then assign
            //memory as per the aligned length
            topCnts->setLength(doc->cnt_topics_size());

            totUsage += topCnts->origLength;
            if (pmw != 0 && k % pmw == 0)
                estimate_memoryn_warn(totUsage);

            //Allocate memory as per the aligned length
            topCnts->assign(-1, false); // if setLen is false, length is ignored

            //Read in the the (topic,counts) pairs into the
            //topicCounts structure
            for (topic_t j = 0; j < topCnts->length; j++) {
                cnt_topic_t& curr_item = topCnts->items[j];
                curr_item.cnt_top = doc->cnt_topics(j);
                tokens_per_topic[curr_item.choose.top] += curr_item.choose.cnt;
            }
            ++num_inited;
            ++k;
        } else {
            for (topic_t t = 0; t < num_topics; t++) {
                tokens_per_topic[t] += doc->topic_counts(t);
            }
        }
    }

    delete doc;
    /*bool valid = validate();
     LOG_IF(WARNING,valid) << "The counts are consistent";
     LOG_IF(WARNING,num_inited==num_words) << "All the words have been initialized with their respective counts";
     LOG_IF(FATAL,!valid) << "Found inconsistent counts. This is fatal. Quitting";*/
    return cont_init;
}

void TypeTopicCounts::initialize_from_ttc(TypeTopicCounts *ttc) {
    for (word_t word = 0; word < num_words; word++) {
        topicCounts* topCnts = wt[word];
        topicCounts* orig_tc = ttc->wt[word];
        topCnts->assign(orig_tc->length);
        topCnts->init(orig_tc->items, orig_tc->length);
    }
    memcpy(tokens_per_topic, ttc->tokens_per_topic,
            num_topics * sizeof(atomic<topic_t> ));
}

/**
 * Use the DocumentWriter to dump the topicCounts and the
 * n(t) into a dump file (fname). Writes num_words topicCounts
 * vectors and then n(t)
 */
void TypeTopicCounts::dump(string fname) {
    DocumentWriter doc_wrtr(fname);
    header hdr;
    hdr.set_version(CURRENT_VERSION);
    hdr.set_num_words(num_words);
    hdr.set_num_topics(num_topics);
    doc_wrtr.write(hdr);
    LDA::unigram_document doc;
    for (word_t i = 0; i < num_words; i++) {
        topicCounts* topCnts = wt[i];
        for (topic_t j = 0; j < topCnts->length; j++) {
            cnt_topic_t ct = topCnts->items[j];
            doc.add_cnt_topics(ct.cnt_top);
        }
        LOG_IF(FATAL,!doc_wrtr.write(doc))<<"Couldn't write to the stream while dumping TypeTopicCounts. Quitting";

        doc.Clear();
    }
    for(topic_t j=0;j<num_topics;j++) {
        doc.add_topic_counts(tokens_per_topic[j]);
    }
    LOG_IF(FATAL,!doc_wrtr.write(doc)) << "Couldn't write to the stream. Quitting";

    doc.Clear();
}
/****************************** Initialization Ends **********************************/

/****************************** Getters **********************************************/

/**
 * The memory for topicCounts is assumed to have been allocated
 * This method then just copied the topicCounts vector from the
 * table into tc
 */
topic_t TypeTopicCounts::get_counts(word_t word, topicCounts* tc) {
    topicCounts* topCnts = wt[word];
    word_mutex_t::scoped_lock lock(*get_lock(word), false);
    tc->length = topCnts->length;
    if (topCnts->length != 0)
        memcpy(tc->items, topCnts->items, tc->length * sizeof(cnt_topic_t));
    return tc->length;
}

/**
 * The memory for tc is assumed to be allocated. This method just
 * copies the n(t) into tc
 */
topic_t TypeTopicCounts::get_counts(atomic<topic_t>* tc) {
    memcpy(tc, tokens_per_topic, num_topics * sizeof(topic_t));
    return num_topics;
}

/**
 * Get the TopicStats per topic & the hot/top NUM_TOP_TOPICS
 * topics. Used to print the topic stats for the top topics
 */
pair<TopKList**, TopKList*> TypeTopicCounts::get_topic_stats() {
    return pair<TopKList**, TopKList*> (topic_stats, top_topics);
}

/**
 * If you want to lock the structure externally, you can get
 * the lock that controls access to the word by this method
 * Method: word_mutex_t::scoped_lock lock(*get_lock(word), false);
 */
word_mutex_t* TypeTopicCounts::get_lock(word_t word) {
    return word_mutexes + word;
}

/**
 * The number of unique words for which the topic counts are
 * maintained
 */
word_t TypeTopicCounts::get_num_words() {
    return num_words;
}

/**
 * The number of topics being learnt
 */
topic_t TypeTopicCounts::get_num_topics() {
    return num_topics;
}

bool TypeTopicCounts::validate() {
    int* total_counts = new int[num_topics];
    memset(total_counts, 0, num_topics * sizeof(int));
    for (word_t w = 0; w < num_words; w++) {
        topicCounts* topCnts = wt[w];
        int length = topCnts->length;
        for (int i = 0; i < length; i++) {
            cnt_topic_t& ct = topCnts->items[i];
            total_counts[ct.choose.top] += ct.choose.cnt;
        }
    }
    for (int i = 0; i < num_topics; i++)
        if (total_counts[i] != tokens_per_topic[i]) {
            delete[] total_counts;
            return false;
        }
    delete[] total_counts;
    return true;
}

/****************************** Getters End **********************************/

/****************************** Setters **************************************/

/**
 * Replace the counts for this word with those in tc
 */
void TypeTopicCounts::replace(word_t word, topicCounts& tc) {
    word_mutex_t::scoped_lock lock(*get_lock(word), true);
    topicCounts* topCnts = wt[word];
    topCnts->replace(tc);
}

/**
 * Update the counts for word using the delta map. The map
 * contains the topic to count deltas. A lock is obtained
 * on the word and is delegated to the relevant topicCounts
 * Used by the background synchronizers
 */
void TypeTopicCounts::upd_count(word_t word, mapped_vec delta, string dbg) {
    word_mutex_t::scoped_lock lock(*get_lock(word), true);
    topicCounts* topCnts = wt[word];
    topCnts->upd_count(delta, tokens_per_topic);
    //LOG_IF(FATAL,topCnts->QUIT) << dbg << topCnts->print();
}

/**
 * Main update used by the updater filter. Decrements the old_topic
 * counts by 1 and increments the new_topic counts by 1 for word.
 * Acquire a lock. Find the old and new topic locations and update
 * them using the topicCounts vector's methods. However the decrement
 * and increment methods take an index and do fast updates. So there
 * is some pointer arithmetic going on here to find the index into the
 * vector using the position pointers. Also note that the basic structure
 * used has the topic and count packed in a 64 bit integer. So whenever
 * we want to refer to the topic or count individually we need to multiply
 * or divide by 2
 */
void TypeTopicCounts::upd_count(word_t word, topic_t old_topic,
        topic_t new_topic, bool ignore_old_topic) {
    word_mutex_t::scoped_lock lock(*get_lock(word), true);
    topicCounts* topCnts = wt[word];
    if (ignore_old_topic) {
        if (!topCnts->findAndIncrement(new_topic))
            topCnts->addNewTopAftChk(new_topic);
        ++tokens_per_topic[new_topic];
        return;
    }
    if (old_topic == new_topic)
        return;
    topic_t *oldTop = NULL;
    topic_t *newTop = NULL;
    topCnts->findOldnNew(old_topic, new_topic, &oldTop, &newTop);

    if (oldTop == NULL) {
        LOG(FATAL) << "Old Topic " << old_topic << " Not Found for word "
                << word << endl << topCnts->print();
        InvalidOldTopicExc ivotExc(word, old_topic);
        throw ivotExc;
    } else {
        //When a decrement happens it might alter the position of newTop found prev
        topCnts->decrement((oldTop - (topic_t*) (topCnts->items)) / 2, &newTop);
    }

    --tokens_per_topic[old_topic]; //atomic update

    if (newTop == NULL)
        topCnts->addNewTop(new_topic);
    else
        topCnts->increment((newTop - (topic_t*) (topCnts->items)) / 2);

    ++tokens_per_topic[new_topic]; //atomic update
}

/**
 * Clears all the topic statistics
 */
void TypeTopicCounts::clear_stats() {
    for (int i = 0; i < num_topics; i++)
        topic_stats[i]->clear();
    top_topics->clear();
}

void TypeTopicCounts::estimate_alphas(double* alphas, double& alpha_sum) {
    long num_tokens = 0;
    for (topic_t t = 0; t < num_topics; t++)
        num_tokens += tokens_per_topic[t];
    alpha_sum = 50;
    for (topic_t t = 0; t < num_topics; t++) {
        alphas[t] = (tokens_per_topic[t] * alpha_sum) / num_tokens;
    }
}
/******************************** Setters End ***********************************/

/************************** Test & Debug Functions **********************************/

/**
 * Initialize the structure using explicit topicCounts for word.
 * Used in testing. Refer TypeTopicCountsTest.cpp
 */
void TypeTopicCounts::initialize(topicCounts* wtc, atomic<topic_t>* tc,
        word_t word) {
    topicCounts* topCnts = wt[word];
    topCnts->assign(wtc->length);
    memcpy(topCnts->items, wtc->items, topCnts->length * sizeof(cnt_topic_t));
    memcpy(tokens_per_topic, tc, num_topics * sizeof(atomic<topic_t> ));
}

/**
 * Initialize using and array of topicCounts.
 * Used in testing. Refer TypeTopicCountsTest.cpp
 */
void TypeTopicCounts::initialize(topicCounts** wtc, atomic<topic_t>* tc) {
    for (word_t i = 0; i < num_words; i++) {
        initialize(wtc[i], tc, i);
    }
}

bool TypeTopicCounts::equal(const TypeTopicCounts& expected) {
    if (num_words != expected.num_words || num_topics != expected.num_topics) {
        LOG(WARNING) << "Act num words: " << num_words << "Exp num words: "
                << expected.num_words;
        LOG(WARNING) << "Act num topics: " << num_topics
                << "Exp num topics: " << expected.num_topics;
        return false;
    }
    for (int i = 0; i < num_words; i++) {
        if (!wt[i]->equal(*expected.wt[i])) {
            LOG(WARNING) << "Topic Counts for word " << i
                    << " do not match";
            cout << "Actual" << endl;
            cout << wt[i]->print();
            cout << "Expected" << endl;
            cout << expected.wt[i]->print();
            return false;
        }
    }
    return true;
}

string TypeTopicCounts::print(word_t word) {
    stringstream ss;
    string str;
    ss << "Topic Counts:" << endl;
    topicCounts* topCnts = wt[word];
    if (topCnts->length != 0)
        ss << word << ": " << wt[word]->print() << endl;
    ss << "Tokens Per Top:" << endl;
    for (int i = 0; i < num_topics; i++)
        ss << "(" << i << "," << tokens_per_topic[i] << ") ";
    ss << endl;
    return ss.str();
}

/**
 * Dump the structure to log(INFO)
 */
void TypeTopicCounts::print() {
    LOG(INFO) << "Word-Topic Counts";
    int nnz = 0;
    stringstream ss;
    for (word_t i = 0; i < num_words; i++) {
        topicCounts* topCnts = wt[i];
        if (topCnts->length == 0)
            continue;
        ++nnz;
        ss << i;
        for (topic_t j = 0; j < topCnts->length; j++) {
            cnt_topic_t ct = topCnts->items[j];
            ss << " (" << ct.choose.cnt << "," << ct.choose.top << ")";
        }
        if (nnz % 25 == 0) {
            LOG(INFO) << ss.str();
            ss.str("");
        } else
            ss << "\t";
    }
    LOG(INFO) << ss.str();
    ss.str("");
    LOG(INFO) << "Topic Counts";

    for (topic_t i = 0; i < num_topics; i++) {
        if (tokens_per_topic[i] == 0)
            continue;
        ss << "(" << i << "," << tokens_per_topic[i] << "), ";
    }
    LOG(INFO) << ss.str();
    ss.str("");
}
/************************** Test & Debug Functions End **********************************/
