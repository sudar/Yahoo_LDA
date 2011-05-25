#!/bin/sh
# input:
#   $1 : "train" or "test" or "teststream"
#   $2 : The queue you want to use
#   $3 : The hdfs directory that contains the input corpus
#   $4 : The hdfs directory that should contain the output
#   $5 : The max-memory to be used by the task. Also used
#         to limit the memory used to load topic counts for 
#         testing in streaming mode
#   $6 : The number of topics to be learnt (default:1000)
#   $7 : The number of iterations (default:1000)
#   $8 : Full hdfs path of LDALibs.jar (default:hdfs://mithrilblue-nn1.blue.ygrid.yahoo.com/user/shravanm/LDALibs.jar)
#   $9 : Output obtained when LDA used in training mode.
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

get_num_mappers(){
  name=`expr $1 : "\(.*\)\..*"`;
  ext=`expr $1 : "$name\.\(.*\)"`;
  #echo $name $ext 1>&2;
  if [ $ext ] && [ $ext == "gz" ]; then echo 1; return 1;
  else num=`echo "($2 / $3) + 1" | bc`; echo $num; return $num;
  fi;
}

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
trained_data=$9;
temp="temporary";
mr_ulimit=`echo "$maxmem * 1000 + 1000" | bc`;
user=`whoami`;

num_mappers=0;
block_size=134217728;
mkdir -p /tmp/$user;
hadoop dfs -ls $input >/tmp/$user/ls_out;
while read line; do
file=`echo $line | tr -s " " | cut -d' ' -f8`;
if [ $file ]; then
  size=`echo $line | tr -s " " | cut -d' ' -f5`;
  cur_num_mappers=`get_num_mappers $file $size $block_size`;
  num_mappers=`expr $num_mappers + $cur_num_mappers`;
fi;
done < /tmp/$user/ls_out;
echo "Num Mappers : $num_mappers";
if [ $num_mappers -gt 100 ]; then 
  echo "/**********************************************************/";
  echo "                    WARNING WARNING WARNING                 ";
  echo "          Will try to use $num_mappers number of nodes      ";
  echo " Please ensure your queue has enough slots (mostly 4 times) ";
  echo "/**********************************************************/";
fi;

runformatter=1;
`hadoop dfs -test -d "${output}_0"`
if [ $? -eq 0 ]; then
#exists
echo "A checkpointed directory exists. Do you want to start from this checkpoint?";
read answer;
if [ $answer == "yes" ]; then runformatter=0; fi
fi;

if [ $runformatter -eq 1 ]; then
  hadoop dfs -rmr "${output}_0";
  set -x;
  hadoop jar $HADOOP_HOME/hadoop-streaming.jar \
  -Dmapred.job.queue.name=$queue \
  -Dmapred.map.tasks.speculative.execution=false \
  -Dmapred.job.map.memory.mb=${maxmem} \
  -Dmapred.map.tasks=1 \
  -Dmapred.child.ulimit=${mr_ulimit} \
  -Dmapred.task.timeout=1800000 \
  -Dmapred.map.max.attempts=1 \
  -Dmapreduce.job.acl-view-job="shravanm,smola" \
  -input $input \
  -output "${output}_0" \
  -cacheArchive ${jar_file}#LDALibs \
  -mapper "Formatter.sh $model" \
  -file Formatter.sh \
  -file functions.sh \
  -numReduceTasks 0;
  exit_code=$?;
  set +x;
  if [ $exit_code -ne 0 ]; then
    echo "Unable to run Formatter on your corpus";
    exit $exit_code;
  else
    echo "Formatting complete. Formatted output stored at ${output}_0";
  fi;
  hadoop dfs -rmr -skipTrash $output/part-* $output/temporary
fi;

mapper="";
max_attempts=5;
if [ $mode == "train" ]; then
  mapper="LDA.sh $model \" $flags \" ${topics} ${iters}";
elif [ $mode == "test" ]; then
  mapper="LDA.sh $model \" $flags \" ${topics} ${iters} ${trained_data}";
  max_attemts=1;
elif [ $mode == "teststream" ]; then
  mapper="LDA.sh $model \" $flags \" ${topics} ${iters} ${trained_data} ${maxmem}";
  max_attemts=1;
fi;

echo $mapper;

cur_attempt=0;
exit_code=0;
while [ $cur_attempt -lt $max_attempts ];
do
  echo "######################################## Attempt: $cur_attempt";
  hadoop dfs -rmr $output;
  set -x;
  hadoop jar $HADOOP_HOME/hadoop-streaming.jar \
  -Dmapred.job.queue.name=$queue \
  -Dmapred.map.tasks.speculative.execution=false \
  -Dmapred.job.map.memory.mb=${maxmem} \
  -Dmapred.map.tasks=1 \
  -Dmapred.child.ulimit=${mr_ulimit} \
  -Dmapred.task.timeout=1800000 \
  -Dmapred.map.max.attempts=1 \
  -Dmapreduce.job.acl-view-job="shravanm,smola" \
  -input "${output}_0/input" \
  -output $output \
  -cacheArchive ${jar_file}#LDALibs \
  -mapper "$mapper" \
  -file LDA.sh \
  -file functions.sh \
  -numReduceTasks 0;
  exit_code=$?;
  set +x;
  if [ $exit_code -eq 0 ]; then
    hadoop dfs -rmr -skipTrash $output/part-* $output/temporary
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
