#!/bin/bash

. ./functions.sh

#Main starts
model=$1;
flags=$2;
topics=$3;
iters=$4;
trained_data=$5;
maxmem=$6;

echo "model=$model flags=\"$flags\" topics=$topics iters=$iters trained_data=$trained_data maxmem=$maxmem" 1>&2

JHOME=`pwd`;
LDALIBS=$JHOME"/LDALibs";
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$LDALIBS/lib;
PORT=25342;
JAVA="${JAVA_HOME+${JAVA_HOME}/bin/}java";
MY_INP_DIR="${mapred_input_dir}/../${mapred_task_partition}";
synch_dir=${mapred_output_dir}"/temporary/synchronize";
hostnames_dir=${mapred_output_dir}"/temporary/hostnames";
HADOOP_CMD="${HADOOP_HOME+${HADOOP_HOME}/bin/}hadoop";
cp $LDALIBS/Tokenizer.class .;

#if [ $maxmem ]; then
#  ${HADOOP_CMD} dfs -copyToLocal ${trained_data}/global/lda.dict.dump training.dict.dump;
#  ${HADOOP_CMD} dfs -copyToLocal ${trained_data}/global/topic_counts topic_counts;
#  dumps=`ls -alh topic_counts | wc -l`;
#  dumps=`expr $dumps - 3`;
#  $LDALIBS/learntopics -teststream --dumpdir=topic_counts --numdumps=$dumps --dictionary=training.dict.dump --maxmemory=$maxmem --topics=$topics --burnin=$iters;
#  rm training.dict.dump;
#  rm -rf topic_counts;
#  cd $JHOME
#  `${HADOOP_CMD} dfs -put out ${mapred_output_dir}/${mapred_task_partition}`;
#  exit 0;
#fi;  

${HADOOP_CMD} dfs -touchz ${mapred_output_dir}/input/${mapred_task_partition};
${HADOOP_CMD} dfs -copyToLocal "${MY_INP_DIR}" out;
cd out;
lterr="";
if [ ${trained_data} ]; then
  ${HADOOP_CMD} dfs -copyToLocal ${trained_data}/global/topic_counts topic_counts;
  dumps=`ls -alh topic_counts | wc -l`;
  dumps=`expr $dumps - 4`;
  $LDALIBS/learntopics -test --topics=$topics --dumpprefix=topic_counts/lda --numdumps=$dumps;
  lterr=$?;
  rm -rf topic_counts;

  #wait_for_all 60 ${synch_dir}"/learntopics";

  cd $JHOME
  `${HADOOP_CMD} dfs -put out ${mapred_output_dir}/${mapred_task_partition}`;
  check_exit_status $lterr "learntopics";

  #wait_for_all 60 ${synch_dir}"/output";
  exit 0;
fi;

#Write <partition>:hostname to client-<partition>
echo ${mapred_task_partition}":"`hostname -i` | ${HADOOP_CMD} dfs -put - ${hostnames_dir}/client-${mapred_task_partition};

wait_for_all 60 ${synch_dir}"/clid_names";

#Parse the client ids & their corresponding hostnames into
#clientids & hostnames array
clid_names=`${HADOOP_CMD} dfs -cat ${hostnames_dir}/* | tr "\n" " "`;
for clid_name in ${clid_names}; do
  clientid=`expr "${clid_name}" : "\(.*\)\:.*"`; #client_id
  hostnames[${clientid}]=`expr "${clid_name}" : ".*\:\(.*\)"`; #hostname
done;


servers=${hostnames[0]}":$PORT";
client_ind=1;
while [ ${client_ind} -lt ${mapred_map_tasks} ]; do
  servers=$servers","${hostnames[${client_ind}]}":$PORT"
  client_ind=`expr ${client_ind} + 1`;
done;

echo "Servers Param: " $servers 1>&2;

rm Tokenizer.class;
$LDALIBS/DM_Server $model ${mapred_task_partition} ${mapred_map_tasks} ${hostnames[${mapred_task_partition}]}":"$PORT --Ice.ThreadPool.Server.SizeMax=9 1>&2 &
if [ $? -ne 0 ]; then
  ps -aef | grep "DM_Server" 1>&2
  pkill DM_Server;
  sleep 10;
  $LDALIBS/DM_Server $model ${mapred_task_partition} ${mapred_map_tasks} ${hostnames[${mapred_task_partition}]}":"$PORT --Ice.ThreadPool.Server.SizeMax=9 1>&2 &
fi;

wait_for_all 60 ${synch_dir}"/ice";



#top -b -d 30 -n 240 -i 1>top.out 2>>top.out &
#Helps to report progress in case of large input
top -b -d 30 -n 2000 | egrep "DM_Server|learntopics"  1>&2 &
#top -b -d 10 -n 2000 | egrep "DM_Server"  1>&2 &

$LDALIBS/learntopics --model=$model --iter=$iters --topics=$topics --servers="$servers" --chkptdir="${MY_INP_DIR}" $flags 1>&2;
lterr=$?

wait_for_all 60 ${synch_dir}"/learntopics";

cd $JHOME
`${HADOOP_CMD} dfs -put out ${mapred_output_dir}/${mapred_task_partition}`;
check_exit_status $lterr "learntopics";

wait_for_all 60 ${synch_dir}"/output";

if [ $model -eq 2 ];
then
  pkill DM_Server
  pkill top;
  if [ "$?" -ne "0" ]; then exit 0; fi;
  exit 0;
fi;

if [ ${mapred_task_partition} -eq 0 ];
then
  mkdir -p $JHOME/global_dict;
  cd $JHOME/global_dict;
  i=0;
  while [ $i -lt ${mapred_map_tasks} ];
  do 
    ${HADOOP_CMD} dfs -copyToLocal ${mapred_output_dir}/$i/lda.dict.dump lda.dict.dump.$i;
    i=`expr $i + 1`;
  done;

  $LDALIBS/Merge_Dictionaries --dictionaries=${mapred_map_tasks} --dumpprefix=lda.dict.dump

  ${HADOOP_CMD} dfs -put lda.dict.dump ${mapred_output_dir}/global/lda.dict.dump;
  check_exit_status $? "dput lda.dict.dump";

  cp -L Merge_Dictionaries.WARNING mergedictionaries.log;
  check_exit_status $? "cp -L WARNING";

  ${HADOOP_CMD} dfs -put mergedictionaries.log ${mapred_output_dir}/global/mergedictionaries.log;
  check_exit_status $? "dput .log";

  cd $JHOME;
fi;

wait_for_all 60 ${synch_dir}"/global_dict";

mkdir -p $JHOME/global_top_cnts;
cd $JHOME/global_top_cnts;

${HADOOP_CMD} dfs -get ${mapred_output_dir}/global/lda.dict.dump lda.dict.dump.global

#create global word-topic counts table using memcached
#and global dictionary. Assign a portion of the global
#dictionary to each client and retrieve counts for these
#words and create a partial w-t-c table and dump it disk
$LDALIBS/Merge_Topic_Counts --topics=$topics --clientid=${mapred_task_partition} --servers="$servers" --globaldictionary="lda.dict.dump.global"

wait_for_all 60 ${synch_dir}"/merge_topcnts";

${HADOOP_CMD} dfs -put lda.ttc.dump ${mapred_output_dir}/global/topic_counts/lda.ttc.dump.${mapred_task_partition}
check_exit_status $? "dput lda.ttc.dump";

cp -L Merge_Topic_Counts.WARNING mergetopics.${mapred_task_partition}.log;
check_exit_status $? "cp -L WARNING";

${HADOOP_CMD} dfs -put mergetopics.${mapred_task_partition}.log ${mapred_output_dir}/global/mergetopics.${mapred_task_partition}.log;
check_exit_status $? "dput .log";

if [ ${mapred_task_partition} -eq 0 ];
then
  ${HADOOP_CMD} dfs -cp ${mapred_output_dir}/0/lda.par.dump ${mapred_output_dir}/global/topic_counts/lda.par.dump;
  check_exit_status $? "param dump";
fi;

pkill DM_Server
pkill top;
if [ "$?" -ne "0" ]; then exit 0; fi;
#Main ends
#########################################################################
