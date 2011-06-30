#!/bin/bash

buildlibrary ()
{
  cd $lib_build
  echo "Installing $1"
  tar -zxf $lib_src/$1.tar.gz
  cd $1
  case $1 in
   "tbb22_20090809oss")
     make;
     for i in `find build -name "lib*"`; do cp $i $prefix_dir/lib/; done;
     cp -r include/tbb $prefix_dir/include/;
     return
     ;;
   "boostinclude")
     mv boost $prefix_dir/include/;
     return
     ;;
   "mcpp-2.7.2")
     ./configure CFLAGS=-fPIC --enable-mcpplib --disable-shared --prefix=$prefix_dir
     ;;
   "bzip2-1.0.5")
     make
     make install PREFIX=$prefix_dir
     return
     ;;
   "Ice-3.4.1")
     cd cpp;
     make
     make install
     return
     ;;
   *)
     ./configure --prefix=$prefix_dir $2
     ;;
  esac
  make  
  #make -s check
  make install
}

# create local directories
rm -fr build
rm -fr lib
rm -fr man
rm -fr include
rm -fr bin
mkdir build
mkdir lib
mkdir bin
mkdir man
mkdir include

# set paths
prefix_dir=`pwd`
lib_src=$prefix_dir/dependencies
lib_build=$prefix_dir/build
lib_opts=''

mkdir /tmp/gflags		# fix a known build bug on OS X
buildlibrary gflags-1.2
rm -fr /tmp/gflags		# gflags wants to have /tmp/gflags but nobody else wants it ...
buildlibrary glog-0.3.0 --with-gflags=$prefix_dir
buildlibrary protobuf-2.2.0a
buildlibrary cppunit-1.12.1
buildlibrary tbb22_20090809oss
buildlibrary boostinclude
buildlibrary bzip2-1.0.5
buildlibrary mcpp-2.7.2

UNAME=`uname`
MACHINE_TYPE=`uname -m`
if [ $UNAME == "Linux" ];
then 
  lib_dir=
  if [ $MACHINE_TYPE == "x86_64" ];
  then
      lib_dir=lib64
      mv $prefix_dir/lib $prefix_dir/lib64
      buildlibrary Ice-3.4.1
      mv $prefix_dir/lib64 $prefix_dir/lib
  else
      lib_dir=lib
      buildlibrary Ice-3.4.1
  fi;
  if [ -d /usr/include/nptl ];
  then
      cp -r /usr/include/nptl/* $prefix_dir/include
      cp -r /usr/${lib_dir}/nptl/* $prefix_dir/lib
  fi;

fi;

cd $prefix_dir
#rm -fr $prefix_dir/build;

cp $prefix_dir/create_obj_file_targets.sh bin/
cp $prefix_dir/create_dep_file_targets.sh bin/
cp $prefix_dir/create_dir_file_targets.sh bin/

mkdir -p ut_out;
mkdir -p ut_test;
head -900 test/ydir_1k.txt >ut_out/ydir_1k.txt;
tail -100 test/ydir_1k.txt >ut_test/ydir_1k.tst.txt;
