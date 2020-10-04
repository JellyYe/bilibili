#!/bin/bash

A=3

function say_num()
{
	echo "$1"
	return $?
}

say_hello(){
	echo "hi mm"
	echo "hi shuaige"
}

for i in  a b c d
do
	echo "$i"
done

value=9
case $value in 
	10)
		echo "10"
		;;
	9)
		echo "*****9****"
		;;
	8)  
		echo "8"
		;;
	*)
		echo "no support"
		;;
esac 

if [ $A -lt 4 ] ; then
	echo "less than"
fi

if [ $A -gt 2 ] 
then
	echo "great than"
fi

#if test $A -eq 3
if test $A -eq 3 ; then
       echo "equal"
fi

say_hello

say_num 4

