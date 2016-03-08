#!/bin/sh

for i in `seq 1 8`;
do
	echo "Test: $i"
	python test.py ~/Documents/IST/Presentation/Testes/KMeans/test$i.csv Plots/test$i
done
