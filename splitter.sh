#!/bin/sh
# This script can be used to split a large corpus into
# n-parts so that exactly n-machines are used on the
# grid.
# Inputs:
#   queue: The hadoop queue you want to use
#   input_dir: The dfs path where the corpus is located
#   output_dir: The dfs path where the split files are to be stored
#   numSplits: The number of machines you want to use to
#              process the corpus.
#
# Outputs:
#   <numSplits> .gz files obtained by slicing the corpus into
#   <numSplits> parts so that one file is processed by a single
#   mapper

queue=$1
input=$2;
output=$3;
numSplits=$4;

hadoop jar $HADOOP_HOME/hadoop-streaming.jar \
-Dmapred.job.queue.name=$queue \
-Dstream.map.output.field.separator=  \
-Dstream.reduce.output.field.separator=  \
-Dmapred.output.compress=true \
-Dmapred.output.compression.codec=org.apache.hadoop.io.compress.GzipCodec \
-input $input \
-output $output \
-mapper /bin/cat \
-reducer /bin/cat \
-numReduceTasks ${numSplits:-20}
