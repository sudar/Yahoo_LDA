#input: number of seconds to sleep
wait_for_all(){
  echo "Synch directory: " $2 1>&2;
  #Write that I am done
  echo ${mapred_task_partition} " done." | hadoop dfs -put - $2/${mapred_task_partition};

  #Find how many are done
  echo "Num of map tasks: " ${mapred_map_tasks} 1>&2;
  num_clients_done=`hadoop dfs -ls $2 | wc -l`;
  num_clients_done=`expr ${num_clients_done} - 1`;
  hadoop dfs -ls $2 1>&2;
  echo "Num of clients done: " ${num_clients_done} 1>&2;

  #Sleep till all are done
  while [ ${num_clients_done} -lt ${mapred_map_tasks} ]; do
    echo "Sleeping" 1>&2;
    sleep $1;
    num_clients_done=`hadoop dfs -ls $2 | wc -l`;
    num_clients_done=`expr ${num_clients_done} - 1`;
    echo "Num of clients done: " ${num_clients_done} 1>&2;
  done;
  echo "All clients done!" 1>&2;
}

#inp1: Exit code
#inp2: Identifier
check_exit_status(){
  if [ "$1" -ne "0" ]; then
    echo "$2 returned an error code of $1" 1>&2;
    exit $1;
  else
    echo "$2 done" 1>&2;
  fi
}

get_num_mappers(){
  name=`expr $1 : "\(.*\)\..*"`;
  ext=`expr $1 : "$name\.\(.*\)"`;
  #echo $name $ext 1>&2;
  if [ $ext ] && [ $ext == "gz" ]; then echo 1; return 1;
  else num=`echo "($2 / $3) + 1" | bc`; echo $num; return $num;
  fi;
}
