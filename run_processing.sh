#! /bin/bash
#
#  1: run number
#  2: first DUT id
#  3: last DUT id
#  4: settings file
#  5: output folder
#  6: input data folder
#  7: number of planes in the telescope
#  8: config file
#  9: chip type (0: pALPIDE(fs) , 1: pALPIDEss)
# 10: alignment method (0 common alignment, 1: run-by-run alignment)
# 11: Processing type: DEBUG (all temporary output is kept), REPROCESS
# 12: Extra busy time
# 13: 0: decide from the data if it's noise or data run, 1: force it to be treated as data, 2: force it to be treated as noise
# TODO assign names to the variables

if [ "${11}" == "DEBUG" ]; then
  redirect=" /dev/stdout"
else
  redirect=" /dev/null"
fi

sed -i '/'$1'/d' $5/../analysis.log
echo -n -e "Processing run" $1 "\n" >> $5/../analysis.log
nativeFolder=$6
if [ -n $SLURM_JOB_ID ]; then
    echo $SLURM_JOB_ID > $5/slurm_job_id.txt
fi
if (( $9 == 1)); then
  if [ "${11}" == "DEBUG" ] && [ -a `printf $5/run"%06d".raw $1` ]; then
    rm -r `printf $5/run"%06d".raw $1`
  fi
  $EUDAQ/bin/Converter.exe -s -t native `printf $6/run"%06d".raw $1` -o `printf $5/run"%06d".raw $1`
  nativeFolder=$5
  echo "DUT(s) are set to be a pALPIDEss" >> $5/analysis.log
elif (( $9 == 0)); then
  echo "DUT(s) are set to be a pALPIDEfs" >> $5/analysis.log
fi
$EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --option NativePath=$nativeFolder --config=$8 -csv $4 converter $1 > $redirect 2>&1
name=`printf $5/lcio/run"%06d"-converter.slcio $1`
nEvent=`lcio_event_counter $name`
echo "Run contains" $nEvent "good events" > $5/analysis.log
re='^[-+]?[0-9]*\.?[0-9]+$'
if ! [[ $nEvent =~ $re ]] ; then
  echo "Converter failed in run" $1 >> $5/../analysis.log
  echo "Converter failed" >> $5/analysis.log
  if [ -f `printf run"%06d"-maskedPixels_*.txt $1` ]; then
    rm `printf run"%06d"-maskedPixels_*.txt $1`
  fi
  if [ "${11}" != "DEBUG" ]; then
    rm -r $5/lcio/ 
    if [ -f `printf *"%06d"* ${input[0]}` ]; then
      rm `printf *"%06d"* ${input[0]}`
    fi
  fi
  exit 0
fi
if (($nEvent < 10000)); then
  echo "Too few events in run" $1 >> $5/../analysis.log
  echo "Too few events" >> $5/analysis.log
  if [ "${11}" != "DEBUG" ]; then
    rm -r $5/lcio
    if [ -f `printf *"%06d"* ${input[0]}` ]; then
      rm `printf *"%06d"* ${input[0]}`
    fi
  fi
  rm `printf run"%06d"-maskedPixels_*.txt $1`
  exit 0
fi
mv `printf run"%06d"-maskedPixels_*.txt $1` $5/database/
cd $5/logs/
converterName=`printf converter-"%06d".zip $1`
unzip $converterName > /dev/null 2>&1 
converterLogName=`printf converter-"%06d".log $1`
place=`cat $converterLogName | sed -n -e "s/^.*Place of telescope://p" | bc -l`
if (($place == -100)); then
  place=`cat $converterLogName | sed -n -e "s/^.*Place of telescope from config file://p" | bc -l`
fi
rm *.log *.xml
cd - > /dev/null 2>&1
if (( ( ${13}==0 && $place > 100) || ${13}==2 )); then
  echo "Treated as noise run" >> $5/analysis.log
  for ((i=$2;i<=$3;i++)) do
    if [ -f $5/../settings_DUT$i.txt ]; then
      sed -i '/^'$1'/d' $5/../settings_DUT$i.txt
    fi
    if [ -f $5/settings_DUT$i.txt ]; then
      rm $5/settings_DUT$i.txt
    fi
  done
  $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --config=$8 -csv $4 noise $1 > $redirect 2>&1
  for ((i=$2;i<=$3;i++)) do
    if ! [ -f $5/../settings_DUT$i.txt ]; then
      cat $5/settings_DUT$i.txt > $5/../settings_DUT$i.txt
    else
      tail -n 1 $5/settings_DUT$i.txt >> $5/../settings_DUT$i.txt
    fi
    sed -i 's/nan/0/g' $5/../settings_DUT$i.txt
  done
  cd $5/logs/
  unzip `printf converter-"%06d".zip $1` > /dev/null 2>&1 
  converterLog=`printf $5/logs/converter-"%06d".log $1`
  thrFailed=0
  for ((i=$2;i<=$3;i++)) do
    thresholds=`awk '{a[++i]=$0;}/Firmware version on layer '"$i"' is/{for(j=NR-4;j<NR;j++)print a[j];}' $converterLog | sed -n -e 's/^.*\[ VERBOSE \"UNKOWN\"\] //p'`
    thrArray=($thresholds)
    for ((j=0;j<16;j++)) do
      if ! [[ ${thrArray[j]} =~ $re ]] ; then
        thrFailed=1
        break
      fi
      if (( $(bc <<< "${thrArray[j]} <= 0 || ${thrArray[j]} > 100") )); then
        thrFailed=1
        break
      fi
    done
  done
  rm *.log *.xml
  cd - > /dev/null 2>&1
  if (( $thrFailed == 1)); then
    echo "Threshold and noise calculation failed" >> $5/analysis.log
    echo "Threshold and noise calculation failed in run" $1 >> $5/../analysis.log
  else
    echo "Processing worked fine" >> $5/analysis.log
    echo "Processing worked fine for run" $1 >> $5/../analysis.log
  fi
  if [ "${11}" != "DEBUG" ]; then
    rm -r $5/lcio $5/database
  fi
  #Quality checks for noise
  outputFolder=$5/Plots/
  mkdir $outputFolder
  root -l -q -b qualityCheckNoise.C\($1,"\"$5/histogram\"","\"$outputFolder\"",$7\) > /dev/null 2>&1
  echo "QA written to" $outputFolder  >> $5/analysis.log
elif (( ( ${13}==0 && $place <= 100) || ${13}==1)); then
  echo "Treated as data run" >> $5/analysis.log
  if (( ${10} == 0)); then
    prealignFiles=`ls $5/../prealign_*.slcio`
    prealignExists=0
    for prealignFile in $prealignFiles
    do
      tmp=${prealignFile#*_}
      first=${tmp%-*}
      if (($first<=$1)); then
        tmp=${prealignFile#*-}
        last=${tmp%.*}
        if (($last>=$1)); then
          if (($prealignExists==1)); then
            echo "More than one possible prealign files were found, please keep only one prealignment and one alignment file for each run" >> $5/analysis.log
            echo "More than one possible prealign files were found for run" $1 >> $5/../analysis.log
            if [ "${11}" != "DEBUG" ]; then
              rm -r $5/lcio
              if [ -f `printf *"%06d"* ${input[0]}` ]; then
                rm `printf *"%06d"* ${input[0]}`
              fi
            fi
            exit 0
          fi
          firstFinal=$first
          lastFinal=$last
          cp $prealignFile `printf $5/database/run"%06d"-prealignment.slcio $1`
          echo Copied $prealignFile >> $5/analysis.log
          prealignExists=1
        fi
      fi
    done
    error=`echo $?`
    if (($prealignExists == 0))
    then
      echo "Prealignment file doesn't exist" >> $5/analysis.log
      echo "Prealignment file doesn't exist for run" $1 >> $5/../analysis.log
      echo "Please create prealign file named prealign_FirstRunItIsToBeUsed-LastRunItIsToBeUsed.slcio or use run_pALPIDEfs_PS_7_wAlign to do alignment for all runs separately" >> $5/analysis.log
      if [ "${11}" != "DEBUG" ]; then
        rm -r $5/lcio
        if [ -f `printf *"%06d"* ${input[0]}` ]; then
          rm `printf *"%06d"* ${input[0]}`
        fi
      fi
      exit 0
    fi
    alignFile=$5/../align_$firstFinal-$lastFinal.slcio
    cp $alignFile `printf $5/database/run"%06d"-alignment.slcio $1`
    error=`echo $?`
    if (($error > 0))
    then
      echo "Alignment file doesn't exist" >> $5/analysis.log
      echo "Alignment file doesn't exist for run" $1 >> $5/../analysis.log
      echo "Please create align file named align_FirstRunItIsToBeUsed-LastRunItIsToBeUsed.slcio or use run_pALPIDEfs_PS_7_wAlign to do alignment for all runs separately" >> $5/analysis.log
      if [ "${11}" != "DEBUG" ]; then
        rm -r $5/lcio
        if [ -f `printf *"%06d"* ${input[0]}` ]; then
          rm `printf *"%06d"* ${input[0]}`
        fi
      fi
      exit 0
    fi
  fi
  if (( $9 == 0 )); then
    $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --config=$8 -csv $4 deadColumn $1 > $redirect 2>&1
  fi
  cd $5/logs
  unzip `printf deadColumn-"%06d".zip $1` > /dev/null 2>&1 
  deadColumnName=`printf deadColumn-"%06d".log $1`
  averageHits=`awk '/Average number of hits per event:/{x=NR+'"$7"';next}(NR<=x){print}' $deadColumnName | sed -n -e 's/^.*\[ MESSAGE5 \"deadColumn\"\] Layer [0-9]//p'`
  averageHitsArray=($averageHits)
  for ((i=0;i<${#averageHitsArray[@]};i++)) do
    if (( $(bc <<< "${averageHitsArray[i]} > 100") )); then
      exludedPlanes[${#exludedPlanes[@]}]=$i
    fi
  done
  echo "Excluded planes:" ${exludedPlanes[@]} >> $5/analysis.log
  cd - > /dev/null 2>&1
  if ((${#exludedPlanes[@]}==0)); then
    exludedPlanes[0]=-1
  elif ((${#exludedPlanes[@]}>2)); then
    echo "More than 2 noise planes (planes" ${exludedPlanes[@]} "), exiting" >> $5/analysis.log
    echo "More than 2 noise planes (planes" ${exludedPlanes[@]} ") in run" $1", exiting" >> $5/../analysis.log
    if [ "${11}" != "DEBUG" ]; then
      rm -r $5/lcio
      if [ -f `printf *"%06d"* ${input[0]}` ]; then
        rm `printf *"%06d"* ${input[0]}`
      fi
    fi
    exit 0
  fi
  excludedPlanesTmp=${exludedPlanes[@]}
  if (( ${10} == 1 )); then
    maffpALPIDEfs=0.0001
    maffpALPIDEss=0.001
    $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --option MaxAllowedFiringFreq=$maffpALPIDEfs --option MaxAllowedFiringFreqpALPIDEss=$maffpALPIDEss --config=$8 -csv $4 hotpixel $1 > $redirect 2>&1
    $EUTELESCOPE/jobsub/jobsub.py --option ExcludedPlanes="" --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --option LCIOInputFiles=$5/lcio/run@RunNumber@-converter.slcio --config=$8 -csv $4 clustering $1 > $redirect 2>&1
    cd $5/logs/
    clusteringName=`printf clustering-"%06d".zip $1`
    unzip $clusteringName > /dev/null 2>&1 
    clusteringLogName=`printf clustering-"%06d".log $1`
    tooNoisy="contains more than 4096 cluster"
    if grep -q "$tooNoisy" "$clusteringLogName"; then
      echo At least one plane had more than 4096 clusters in one event, too noisy exiting > $5/analysis.log
      echo At least one plane is too noisy run $1 > $5/../analysis.log
      if [ "${11}" != "DEBUG" ]; then
        rm -r $5/lcio
        if [ -f `printf *"%06d"* ${input[0]}` ]; then
          rm `printf *"%06d"* ${input[0]}`
        fi
      fi
      exit 0
    fi
    rm *.log *.xml
    cd - > /dev/null 2>&1
    $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --config=$8 -csv $4 hitmaker $1 > $redirect 2>&1
    $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --config=$8 -csv $4 prealign $1 > $redirect 2>&1
    ./run_align_7 $1 $4 $5 $8 ${exludedPlanes[0]} ${exludedPlanes[1]} > $redirect 2>&1
    error=`echo $?`
    if (($error > 0))
    then
      #Quality checks if alignment failed
      outputFolder=$5/Plots/
      mkdir $outputFolder
      mkdir $outputFolder/important
      mkdir $outputFolder/others
      if (( $9 == 0)); then
        root -l -q -b qualityCheckfs.C\($1,$2,$3,"\"$5/histogram\"","\"$outputFolder\"",$7\) > /dev/null 2>&1
      elif (( $9 == 1)); then
        root -l -q -b qualityCheckss.C\($1,$2,$3,"\"$5/histogram\"","\"$outputFolder\"",$7\) > /dev/null 2>&1
      fi
      echo "QA written to" $outputFolder  >> $5/analysis.log
      if [ "${11}" != "DEBUG" ]; then
        rm -r $5/lcio
        if [ -f `printf *"%06d"* ${input[0]}` ]; then
          rm `printf *"%06d"* ${input[0]}`
        fi
      fi
      exit 0
    fi
  fi
  maffpALPIDEfs=0.001
  maffpALPIDEss=1
  $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --option MaxAllowedFiringFreq=$maffpALPIDEfs --option MaxAllowedFiringFreqpALPIDEss=$maffpALPIDEss --config=$8 -csv $4 hotpixel $1 > $redirect 2>&1
  $EUTELESCOPE/jobsub/jobsub.py --option ExcludedPlanes="${exludedPlanes[0]} ${exludedPlanes[1]}" --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --option LCIOInputFiles=$5/lcio/run@RunNumber@-converter.slcio --config=$8 -csv $4 clustering $1 > $redirect 2>&1
  cd $5/logs/
  clusteringName=`printf clustering-"%06d".zip $1`
  unzip $clusteringName > /dev/null 2>&1 
  clusteringLogName=`printf clustering-"%06d".log $1`
  tooNoisy="contains more than 4096 cluster"
  if grep -q "$tooNoisy" "$clusteringLogName"; then
    echo At least one plane had more than 4096 clusters in one event, too noisy exiting > $5/analysis.log
    echo At least one plane is too noisy run $1 > $5/../analysis.log
    if [ "${11}" != "DEBUG" ]; then
      rm -r $5/lcio
      if [ -f `printf *"%06d"* ${input[0]}` ]; then
        rm `printf *"%06d"* ${input[0]}`
      fi
    fi
    exit 0
  fi
  minTimeStamp=0
  minTimeStamp=`cat $clusteringLogName | grep pALPIDEfs | sed -n -e "s/^.*Maximum of the time stamp histo is at //p" | bc -l`
  if [[ ${9} -eq 1 ]] && [[ $minTimeStamp -lt 12000 ]]; then
      echo "Minimum timestamp was too low ("$minTimeStamp") overidding it with 12000"
      minTimeStamp=12000
  fi
  minTimeStamp=`echo "$minTimeStamp+${12}" | bc -l`
  if (( ${12}==0)); then
    minTimeStamp=0
  fi
  rm *.log *.xml
  cd - > /dev/null 2>&1
  $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --config=$8 -csv $4 hitmaker $1 > $redirect 2>&1

  for ((i=$2;i<=$3;i++)) do
    isExlcuded=0
    for ((j=0;j<${#exludedPlanes[@]};j++)) do
      if ((${exludedPlanes[j]}==$i)); then
        isExlcuded=1
        break
      fi
    done
    if (($isExlcuded==1)); then
      continue
    fi
    $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --option dutID="$i" --config=$8 -csv $4 fitter $1 > $redirect 2>&1
    cd $5/logs/
    unzip `printf fitter-"%06d".zip $1` > /dev/null 2>&1 
    fitterName=`printf fitter-"%06d".log $1`
    nTrack=`cat $fitterName |  sed -n -e "s/^.*Total number of reconstructed tracks//p" | bc -l`
    rm *.log *.xml
    cd - > /dev/null 2>&1
    if (($nTrack<100))
    then
      echo "Too few tracks found in DUT" $i", moving to next DUT" >> $5/analysis.log
      echo "Process for DUT" $i "in run" $1 "exited with fitting error" >> $5/../analysis.log
      continue
    else
      echo $nTrack "tracks used in DUT" $i >> $5/analysis.log
    fi
    if [ -f $5/../settings_DUT$i.txt ]; then
      sed -i '/^'$1'/d' $5/../settings_DUT$i.txt
    fi
    if [ -f $5/settings_DUT$i.txt ]; then
     rm $5/settings_DUT$i.txt
    fi
    echo "Using events with a timestamp larger than" $minTimeStamp >> $5/analysis.log
    $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=$5/database --option HistogramPath=$5/histogram --option LcioPath=$5/lcio --option LogPath=$5/logs --option dutID="$i" --option MinTimeStamp=$minTimeStamp --config=$8 -csv $4 analysis $1 > $redirect 2>&1
    if ! [ -f $5/../settings_DUT$i.txt ]; then
      cat $5/settings_DUT$i.txt > $5/../settings_DUT$i.txt
    else
      tail -n 1 $5/settings_DUT$i.txt >> $5/../settings_DUT$i.txt
    fi
    sed -i 's/nan/0/g' $5/../settings_DUT$i.txt
    cd $5/logs/
    unzip `printf analysis-"%06d".zip $1` > /dev/null 2>&1 
    analysisName=`printf analysis-"%06d".log $1`
    if (( $9 == 0)); then
      efficiencies=`awk '/Overall efficiency of pALPIDEfs sectors/{x=NR+4;next}(NR<=x){print}' $analysisName | sed -n -e 's/^.*\[ MESSAGE4 \"Analysis\"\] //p'`
      rm *.log *.xml
      cd - > /dev/null 2>&1
      echo "Efficiencies of the four sectors in DUT" $i":" >> $5/analysis.log
      effArray=($efficiencies)
      for ((j=1;j<=10;j=j+3)) do
        echo ${effArray[j-1]} >> $5/analysis.log
      done
    elif (( $9 == 1)); then
      grep "Overall efficiency of pAlpide: " $analysisName | sed -n -e 's/^.*\[ MESSAGE4 \"Analysis\"\] //p' >> $5/analysis.log
      rm *.log *.xml
      cd - > /dev/null 2>&1
    fi
    mv `printf $5/logs/analysis-"%06d".zip $1` `printf $5/logs/analysis-"%06d"_DUT$i.zip $1`
    echo "Processing of DUT" $i "exited without errors for run" $1 >> $5/../analysis.log
    echo "Processing of DUT" $i "exited without errors" >> $5/analysis.log
  done
  if [ "${11}" != "DEBUG" ]; then
    find $5/lcio -type f -not -name '*track*' | xargs rm # To keep only the output of the fitter
#    rm -r $5/lcio $5/database # To delete all intermediate steps
  fi
  #Quality checks
  outputFolder=$5/Plots/
  mkdir $outputFolder
  mkdir $outputFolder/important
  mkdir $outputFolder/others
  if (( $9 == 0)); then
    root -l -q -b qualityCheckfs.C\($1,$2,$3,"\"$5/histogram\"","\"$outputFolder\"",$7\) > /dev/null 2>&1
  elif (( $9 == 1)); then
    root -l -q -b qualityCheckss.C\($1,$2,$3,"\"$5/histogram\"","\"$outputFolder\"",$7\) > /dev/null 2>&1
  fi
  echo "QA written to" $outputFolder  >> $5/analysis.log
else
  echo -n -e "Not able to decide if it's noise or data. Please use one of the following settings in dataProcessing: \n  0: decide from the data if it's noise or data run \n  1: force it to be treated as data, \n  2: force it to be treated as noise \n"
fi
