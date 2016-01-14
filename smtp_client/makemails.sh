#!/bin/bash

#
# script creates emails to be processed by mail client
# see zadani.txt (in czech)
#
HOSTNAME='u2-0.ms.mff.cuni.cz'
PORTS="2526 2525 2527 2528 2529"

for i in $PORTS ; do
	mkdir $HOSTNAME@$i
	cd $HOSTNAME@$i
	for j in `seq 100` ; do
		FILE="info@test-$j.cz"
		echo "To: info@test$j.cz" > $FILE
		echo "From: odesilatel@spam.cz" >> $FILE
		echo "Subject: Test" >> $FILE
		echo "" >> $FILE
		for k in `seq 100` ; do
			perl -e 'print "a" x 75 ' >> $FILE
			echo "" >> $FILE
		done
	done
	cd ..
done
