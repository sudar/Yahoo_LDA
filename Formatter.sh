#!/bin/sh

. ./functions.sh

#Main starts
synch_dir=${mapred_output_dir}"/temporary/synchronize";
hostnames_dir=${mapred_output_dir}"/temporary/hostnames";
model=$1;
flags=$2;

echo "model=$model flags=\"$flags\"" 1>&2;

JHOME=`pwd`;
LDALIBS=$JHOME"/LDALibs";
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$LDALIBS/lib;
PORT=25342;
JAVA="/grid/0/gs/java/jdk/bin/java";

mkdir -p $JHOME/out;
cd $JHOME/out;
cp $LDALIBS/Tokenizer.class .;

if [ $model -eq 1 ]; then
  $JAVA Tokenizer | $LDALIBS/formatter --model=$model
else
  $LDALIBS/formatter --model=$model
fi;
check_exit_status $? "Formatter";
hadoop dfs -touchz ${mapred_output_dir}/input/${mapred_task_partition};
cd $JHOME
`hadoop dfs -put out ${mapred_output_dir}/${mapred_task_partition}`;
