#!/bin/bash

testpath=$(pwd)/tests/typecheck
for testfile in $testpath/good*.bminor
do
    
	if $(pwd)/bminor -typecheck $testfile > $testfile.out
	then
		echo "$testfile success (as expected)"
	else
		echo "$testfile failure (INCORRECT)"
	fi
done

for testfile in $testpath/bad*.bminor
do
	if $(pwd)/bminor -typecheck $testfile > $testfile.out
	then
		echo "$testfile success (INCORRECT)"
	else
		echo "$testfile failure (as expected)"
	fi
done
