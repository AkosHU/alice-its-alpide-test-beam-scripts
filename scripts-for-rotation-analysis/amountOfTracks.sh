#/bin/bash.sh
#this script looks at the analysis.log and extracts the amount of tracks and the amount of events and outputs the two for all runs between the range given as first and second parameter. It allows to compare the two. Set the output folder below.
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
	file="$outputFolder/run$runnumber/analysis.log"

	numberTracks=$(head -n12 $file | tail -n1)
	numberTracks="$(echo $numberTracks | head -c 10)" 
	numberTracks=$(echo ${numberTracks//[^0-9]/})

	numberEvents=$(head -n2 $file | tail -n1)
	numberEvents=$(echo ${numberEvents//[^0-9]/})

	echo "$runs: $numberTracks tracks in $numberEvents were found."
done
