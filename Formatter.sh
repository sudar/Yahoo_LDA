#!/bin/sh

. ./functions.sh

#Main starts
synch_dir=${mapred_output_dir}"/temporary/synchronize";
hostnames_dir=${mapred_output_dir}"/temporary/hostnames";
HADOOP_CMD="${HADOOP_HOME+${HADOOP_HOME}/bin/}hadoop";
model=$1;
flags=$2;
trained_data=$3;

echo "model=$model flags=\"$flags\" trained_data=\"${trained_data}\"" 1>&2;

JHOME=`pwd`;
LDALIBS=$JHOME"/LDALibs";
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$LDALIBS/lib;
PORT=25342;
JAVA="${JAVA_HOME+${JAVA_HOME}/bin/}java";

mkdir -p $JHOME/out;
cd $JHOME/out;
cp $LDALIBS/Tokenizer.class .;

if [ $model -eq 1 ]; then
  if [ ${trained_data} ]; then
    ${HADOOP_CMD} dfs -copyToLocal ${trained_data}/global/lda.dict.dump training.dict.dump;
    $JAVA Tokenizer | $LDALIBS/formatter --model=$model --dumpfile=training.dict.dump
    rm training.dict.dump;
  else
    $JAVA Tokenizer | $LDALIBS/formatter --model=$model
  fi;
else
  $LDALIBS/formatter --model=$model
fi;
check_exit_status $? "Formatter";
${HADOOP_CMD} dfs -touchz ${mapred_output_dir}/input/${mapred_task_partition};
cd $JHOME
`${HADOOP_CMD} dfs -put out ${mapred_output_dir}/${mapred_task_partition}`;
