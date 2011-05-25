PWD=`pwd`; LD_PATH=$PWD"/lib"; for i in `find lib/* -maxdepth 0 -type d`; do LD_PATH=$LD_PATH:$PWD"/"$i; done; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LD_PATH
