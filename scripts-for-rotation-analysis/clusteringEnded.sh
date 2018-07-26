#/bin/bash.sh
#this script checks, whether the clustering has ended probably for all runs between the first and second argument given. Set the output folder.
outputFolder="OUTPUTFOLDER"

firstrun="$1"
lastrun="$2"

if [ "$firstrun" == "" ];then
	echo "Please specify the first run you would want to check as the first argument. Exiting."
	exit 0
fi

if [ "$lastrun" == "" ];then
	echo "Please specify the last run you would want to check as the second argument. Exiting."
	exit 0
fi

for (( runs=$firstrun; runs<=$lastrun; runs++ ))
do
	runnumber="$(printf "%06d" $runs)"
	#echo $runnumber
	zipfile="$outputFolder/run$runnumber/logs/clustering-$runnumber.zip"
	file="clustering-$runnumber.log"

	lastTwolines=$(unzip -c $zipfile $file | tail -n2)
	#echo $lastTwolines

	shortenedLastTwolines="$(echo $lastTwolines | head -c 72)"
	#echo $shortenedLastTwolines

	compareMessage='[ MESSAGE4 "Clustering_pALPIDEfs"] Maximum of the time stamp histo is at'
	#echo $compareMessage

	if [ "$compareMessage" != "$shortenedLastTwolines" ]; then
		echo "The clustering of run $runs was not finished properly."
	fi
done
