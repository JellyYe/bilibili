#!/bin/bash


if [ $# -lt 2 ]  
then
	echo "uagea : cmd  arg1 arg2 ..."
	exit 1
else
	echo "hahahahahah"

fi


for person in $@
do

	case $person in
		marong)
			echo "it is a bitch"
			;;
		宝宝)
			echo "baobao is very hard"
			;;
		zhangdao)
			echo "baobao me too"
			;;
	
		*)
			echo "who are you "
		
			;;
	esac 
done


i=0
sum=0
while [ $i -le 100 ]
do
	sum=`expr $sum + $i`
	let "i=i+1"	

done
echo "sum = $sum"

path=/home/george
filelist=`ls $path`
for file in $filelist
do
	if [ -d $path/$file ] ; then
		echo "$file ---> dir"
	fi
	
	if [ -f $path/$file ] ; then
		echo "$file ---> regulor"
	fi
done






exit 0



#echo "PATH = $PATH"

echo "arg0 = $0"
echo "arg1 = $1"
echo "arg2 = $2"
echo "arg3 = $3"
echo "arg4 = $4"
echo "args num : $#"
echo "args list : $@"

ls /abx
echo "exit code $?"

ls /
echo "exit code $?"

NUM=250

echo "NUM = $NUM"
echo 'NUM = $NUM'
echo `NUM = $NUM`

curdir=`pwd`
echo "curdir = $curdir"

echo "hello world"

echo "pls input your name : "
read name

echo "name = $name"
