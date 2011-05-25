#!/bin/sh

mv $2 $2.bkp;

cat $1 $2.bkp >$2

rm $2.bkp;
