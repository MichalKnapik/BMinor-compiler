#!/bin/bash

scantestpath=$(pwd)/tests/scanner
for testfile in $scantestpath/good*.bminor
do
    
	if $(pwd)/bminor -scan $testfile > $testfile.out
	then
		echo "$testfile success (as expected)"
	else
		echo "$testfile failure (INCORRECT)"
	fi
done

for testfile in $scantestpath/bad*.bminor
do
	if $(pwd)/bminor -scan $testfile > $testfile.out
	then
		echo "$testfile success (INCORRECT)"
	else
		echo "$testfile failure (as expected)"
	fi
done
