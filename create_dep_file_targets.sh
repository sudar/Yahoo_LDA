#!/bin/sh

for i in `find . -maxdepth 1 -iname "*.c[cp]*"`; 
do 
	name=`expr "$i" : "\(.*\)\.c[cp]*"`; 
	echo $name.d; 
done;
