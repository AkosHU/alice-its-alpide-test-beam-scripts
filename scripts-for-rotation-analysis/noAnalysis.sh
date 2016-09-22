#/bin/bash.sh
#this script checks, whether enough tracks were found to carry out the analysis or not for all runs in between the first and the second argument given
outputFolder="ANALYSIS OUTPUT FOLDER"

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

	noFittingTooFewTracks=$(head -n12 $file | tail -n1)
	#echo $lastTwolines

	compareMessage='Too few tracks found in DUT 3, moving to next DUT'
	#echo $compareMessage

	if [ "$compareMessage" == "$noFittingTooFewTracks" ]; then
		echo "The analysis of run $runs was not carried out, since there were too few tracks in the fitting."
	fi
done
