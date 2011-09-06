#!/bin/bash
# input:
#   $1 : 1 for Unigram_Model
#	 If a new model is implemented this can be used to access it
#   $2 : Flags that need to be passed to learntopics
#   $3 : "train" or "test" or "teststream"
#   $4 : The queue you want to use
#   $5 : The hdfs directory that contains the input corpus
#   $6 : The hdfs directory that should contain the output
#   $7 : The max-memory to be used by the task. Also used
#         to limit the memory used to load topic counts for 
#         testing in streaming mode
#   $8 : The number of topics to be learnt (default:1000)
#   $9 : The number of iterations (default:1000)
#   $10: Full hdfs path of LDALibs.jar (default:hdfs://mithrilblue-nn1.blue.ygrid.yahoo.com/user/shravanm/LDALibs.jar)
#   $11: The number of machines to be used.
#   $12: Output obtained when LDA used in training mode.
#        To be used when test mode is used
# output:
#   Creates <#mappers> folders in <output-dir> one for each client.
#   Each of these directories hold the same output as the single machine
#   case but from different clients.
#   The <#mappers> created depends on hadoop.
#     Single file: <HDFS block size> chunks are created one per mapper
#     Multiple files: At least one chunk per file. If a file is more than
#       <HDFS block size> more chunks are produced.
#   If you want to process one file per mapper you need to gzip your
#   your input and provide the directory containing the gzip files as
#   <input-dir>

# check to see if a YLDA_HOME environment variable has been set and,
# if so, change into that directory first. Otherwise it assumes that
# YLDA_HOME is the local directory. This makes it so that YLDA does
# not have to be run from the YLDA root
if [ -z ${YLDA_HOME} ]; then
    YLDA_HOME=`pwd`;
fi;
cd ${YLDA_HOME}
. ${YLDA_HOME}/functions.sh

model=$1;
flags=$2;
mode=$3;
queue=$4;
input=$5;
output=$6;
maxmem=${7:-6144};
topics=${8:-1000};
iters=${9:-1000};
shift
#jar_file=${10:-hdfs://mithrilblue-nn1.blue.ygrid.yahoo.com/user/shravanm/LDALibs.jar};
jar_file=$9;
if [ ! ${jar_file} ]; then
  echo "You need to create your own LDALibs.jar with make jar,";
  echo "copy it to HDFS and provide its full hdfs path for the jar_file argument";
  exit 1;
fi;
shift
num_machines=$9;
if [ ! ${num_machines} ]; then
  echo "You need to specify the number of machines to be used";
  exit 1;
fi;
shift
trained_data=$9;
temp="temporary";
mr_ulimit=`echo "$maxmem * 1000 + 1000" | bc`;
user=`whoami`;
HADOOP_CMD="${HADOOP_HOME+${HADOOP_HOME}/bin/}hadoop";

formatter_output="${output}_0"
runformatter=1;
`${HADOOP_CMD} dfs -test -d "${formatter_output}"`
if [ $? -eq 0 ]; then
  #exists
  echo "A checkpointed directory exists. Do you want to start from this checkpoint?";
  read answer;
  if [ $answer == "yes" ]; then runformatter=0; fi
fi;

if [ $runformatter -eq 1 ]; then
  ${HADOOP_CMD} dfs -rmr "${formatter_output}";
  set -x;
  ${HADOOP_CMD} jar $HADOOP_HOME/hadoop-streaming.jar \
  -Dmapred.job.queue.name=$queue \
  -Dmapred.reduce.tasks.speculative.execution=false \
  -Dmapred.job.reduce.memory.mb=${maxmem} \
  -Dmapred.reduce.tasks=1 \
  -Dmapred.child.ulimit=${mr_ulimit} \
  -Dmapred.task.timeout=1800000 \
  -Dmapred.reduce.max.attempts=1 \
  -Dmapreduce.job.acl-view-job="shravanm,smola" \
  -input $input \
  -output "${formatter_output}" \
  -cacheArchive ${jar_file}#LDALibs \
  -mapper "/bin/cat" \
  -reducer "Formatter.sh $model \" \" ${trained_data}" \
  -file Formatter.sh \
  -file functions.sh \
  -numReduceTasks ${num_machines};
  exit_code=$?;
  set +x;
  if [ $exit_code -ne 0 ]; then
    echo "Unable to run Formatter on your corpus";
    exit $exit_code;
  else
    echo "Formatting complete. Formatted output stored at ${formatter_output}";
  fi;
  ${HADOOP_CMD} dfs -rmr -skipTrash ${formatter_output}/part-*
fi;

mapper="";
max_attempts=5;
map_max_attempts=1;
map_input="${formatter_output}/input";
if [ $mode == "train" ]; then
  mapper="LDA.sh $model \" $flags \" ${topics} ${iters}";
elif [ $mode == "test" ]; then
  mapper="LDA.sh $model \" $flags \" ${topics} ${iters} ${trained_data}";
  max_attemts=1;
  map_max_attempts=4;
#elif [ $mode == "teststream" ]; then
#  mapper="LDA.sh $model \" $flags \" ${topics} ${iters} ${trained_data} ${maxmem}";
#  max_attemts=1;
#  map_max_attempts=4;
#  map_input=$input;
fi;

echo $mapper;

cur_attempt=0;
exit_code=0;
while [ $cur_attempt -lt $max_attempts ];
do
  echo "######################################## Attempt: $cur_attempt";
  ${HADOOP_CMD} dfs -rmr $output;
  set -x;
  ${HADOOP_CMD} jar $HADOOP_HOME/hadoop-streaming.jar \
  -Dmapred.job.queue.name=$queue \
  -Dmapred.map.tasks.speculative.execution=false \
  -Dmapred.job.map.memory.mb=${maxmem} \
  -Dmapred.map.tasks=1 \
  -Dmapred.child.ulimit=${mr_ulimit} \
  -Dmapred.task.timeout=1800000 \
  -Dmapred.map.max.attempts=${map_max_attempts} \
  -Dmapreduce.job.acl-view-job="shravanm,smola" \
  -input "${map_input}" \
  -output $output \
  -cacheArchive ${jar_file}#LDALibs \
  -mapper "$mapper" \
  -file LDA.sh \
  -file functions.sh \
  -numReduceTasks 0;
  exit_code=$?;
  set +x;
  if [ $exit_code -eq 0 ]; then
    ${HADOOP_CMD} dfs -rmr -skipTrash $output/part-*
    if [ $mode == "test" ]; then
      echo "Topic Assignmnents for the test documents have been produced using the provided model";
    elif [ $mode == "train" ]; then
      echo "Topic model has been learnt for your corpus in $cur_attempts attempts";
    fi;
    exit 0;
  else
    if [ $mode == "test" ]; then
      echo "Unable to produce topic assignments to the test documents";
      exit $exit_code;
    elif [ $mode == "train" ]; then
      echo "Attempt $cur_attempt failed. Retrying";
      cur_attempt=`expr $cur_attempt + 1`;
    fi;
  fi;
done;

echo "Unable to learn topics for your corpus. Might be a bug or machine failure on the grid";
exit $exit_code;
