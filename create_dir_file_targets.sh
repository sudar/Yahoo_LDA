#!/bin/sh

find . -maxdepth 1 -type d | egrep -v "svn|^.$"
