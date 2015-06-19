#! /bin/bash
#
#  1: run number
#  2: first DUT id
#  3: last DUT id
#  4: settings file (run list with the DUT information and beam conditions)
#  5: output folder
#  6: input data folder
#  7: number of planes in the telescope
#  8: config file
#  9: DUT type (0: pALPIDE(fs) , 1: pALPIDEss)
# 10: alignment method (0 common alignment, 1: run-by-run alignment)
# 11: Processing type: DEBUG (all temporary output is kept), REPROCESS
# 12: Extra busy time
# 13: 0: auto-detect - decide from the data if it's noise or data run, 1: force it to be treated as data, 2: force it to be treated as noise
runNumber=${1}
firstDUTid=${2}
lastDUTid=${3}
settingsFile=${4}
outputFolder=${5}
inputFolder=${6}
nTelescopePlanes=${7}
configFile=${8}
dutType=${9}
alignMethod=${10}
processingMode=${11}
extraBusyTime=${12}
dataType=${13}

# common arguments for all calls of jobsub.py:
commonOptions="--option DatabasePath=${outputFolder}/database --option HistogramPath=${outputFolder}/histogram --option LcioPath=${outputFolder}/lcio --option LogPath=${outputFolder}/logs  --config=${configFile} -csv ${settingsFile}"

if [ "${processingMode}" == "DEBUG" ]; then
  redirect=" /dev/stdout"
else
  redirect=" /dev/null"
fi

sed -i '/'${runNumber}'/d' ${outputFolder}/../analysis.log
echo -n -e "Processing run" ${runNumber} "\n" >> ${outputFolder}/../analysis.log
nativeFolder=${inputFolder}
if [ -n $SLURM_JOB_ID ]; then
    echo $SLURM_JOB_ID > ${outputFolder}/slurm_job_id.txt
fi
if (( ${dutType} == 1)); then
  if [ "${processingMode}" == "DEBUG" ] && [ -a `printf ${outputFolder}/run"%06d".raw ${runNumber}` ]; then
    rm -r `printf ${outputFolder}/run"%06d".raw ${runNumber}`
  fi
  $EUDAQ/bin/Converter.exe -s -t native `printf ${inputFolder}/run"%06d".raw ${runNumber}` -o `printf ${outputFolder}/run"%06d".raw ${runNumber}`
  nativeFolder=${outputFolder}
  echo "DUT(s) are set to be a pALPIDEss" >> ${outputFolder}/analysis.log
elif (( ${dutType} == 0)); then
  echo "DUT(s) are set to be a pALPIDEfs" >> ${outputFolder}/analysis.log
fi
$EUTELESCOPE/jobsub/jobsub.py ${commonOptions} --option NativePath=$nativeFolder converter ${runNumber} > $redirect 2>&1
name=`printf ${outputFolder}/lcio/run"%06d"-converter.slcio ${runNumber}`
nEvent=`lcio_event_counter $name`
echo "Run contains" $nEvent "good events" > ${outputFolder}/analysis.log
re='^[-+]?[0-9]*\.?[0-9]+$'
if ! [[ $nEvent =~ $re ]] ; then
  echo "Converter failed in run" ${runNumber} >> ${outputFolder}/../analysis.log
  echo "Converter failed" >> ${outputFolder}/analysis.log
  if [ -f `printf run"%06d"-maskedPixels_*.txt ${runNumber}` ]; then
    rm `printf run"%06d"-maskedPixels_*.txt ${runNumber}`
  fi
  if [ "${processingMode}" != "DEBUG" ]; then
    rm -r ${outputFolder}/lcio/
    if [ -f `printf *"%06d"* ${input[0]}` ]; then
      rm `printf *"%06d"* ${input[0]}`
    fi
  fi
  exit 0
fi
if (($nEvent < 10000)); then
  echo "Too few events in run" ${runNumber} >> ${outputFolder}/../analysis.log
  echo "Too few events" >> ${outputFolder}/analysis.log
  if [ "${processingMode}" != "DEBUG" ]; then
    rm -r ${outputFolder}/lcio
    if [ -f `printf *"%06d"* ${input[0]}` ]; then
      rm `printf *"%06d"* ${input[0]}`
    fi
  fi
  rm `printf run"%06d"-maskedPixels_*.txt ${runNumber}`
  exit 0
fi
mv `printf run"%06d"-maskedPixels_*.txt ${runNumber}` ${outputFolder}/database/
cd ${outputFolder}/logs/
converterName=`printf converter-"%06d".zip ${runNumber}`
unzip $converterName > /dev/null 2>&1
converterLogName=`printf converter-"%06d".log ${runNumber}`
place=`cat $converterLogName | sed -n -e "s/^.*Place of telescope://p" | bc -l`
if (($place == -100 && ${dataType}==0)); then
  echo "Whether it's data or noise is not specified in the rawdata. Please force it to be treated as noise or data." >> ${outputFolder}/analysis.log
  echo "In run" ${runNumber} "it's not specified in the rawdata whether it's data or noise. Please force it to be treated as noise or data." >> ${outputFolder}/../analysis.log
  exit 0
fi
rm *.log *.xml
cd - > /dev/null 2>&1
if (( ( ${dataType}==0 && $place > 100) || ${dataType}==2 )); then
  echo "Treated as noise run" >> ${outputFolder}/analysis.log
  for ((i=${firstDUTid};i<=${lastDUTid};i++)) do
    if [ -f ${outputFolder}/../settings_DUT$i.txt ]; then
      sed -i '/^'${runNumber}'/d' ${outputFolder}/../settings_DUT$i.txt
    fi
    if [ -f ${outputFolder}/settings_DUT$i.txt ]; then
      rm ${outputFolder}/settings_DUT$i.txt
    fi
  done
  $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} noise ${runNumber} > $redirect 2>&1
  for ((i=${firstDUTid};i<=${lastDUTid};i++)) do
    if ! [ -f ${outputFolder}/../settings_DUT$i.txt ]; then
      cat ${outputFolder}/settings_DUT$i.txt > ${outputFolder}/../settings_DUT$i.txt
    else
      tail -n 1 ${outputFolder}/settings_DUT$i.txt >> ${outputFolder}/../settings_DUT$i.txt
    fi
    sed -i 's/nan/0/g' ${outputFolder}/../settings_DUT$i.txt
  done
  cd ${outputFolder}/logs/
  unzip `printf converter-"%06d".zip ${runNumber}` > /dev/null 2>&1
  converterLog=`printf ${outputFolder}/logs/converter-"%06d".log ${runNumber}`
  thrFailed=0
  for ((i=${firstDUTid};i<=${lastDUTid};i++)) do
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
    echo "Threshold and noise calculation failed" >> ${outputFolder}/analysis.log
    echo "Threshold and noise calculation failed in run" ${runNumber} >> ${outputFolder}/../analysis.log
  else
    echo "Processing worked fine" >> ${outputFolder}/analysis.log
    echo "Processing worked fine for run" ${runNumber} >> ${outputFolder}/../analysis.log
  fi
  if [ "${processingMode}" != "DEBUG" ]; then
    rm -r ${outputFolder}/lcio ${outputFolder}/database
  fi
  #Quality checks for noise
  outputFolderQA=${outputFolder}/Plots/
  mkdir $outputFolderQA
  root -l -q -b qualityCheckNoise.C\(${runNumber},"\"${outputFolder}/histogram\"","\"$outputFolderQA\"",${nTelescopePlanes}\) > /dev/null 2>&1
  echo "QA written to" $outputFolderQA >> ${outputFolder}/analysis.log
elif (( ( ${dataType}==0 && $place <= 100) || ${dataType}==1)); then
  echo "Treated as data run" >> ${outputFolder}/analysis.log
  if (( ${alignMethod} == 0)); then
    prealignFiles=`ls ${outputFolder}/../prealign_*.slcio`
    prealignExists=0
    for prealignFile in $prealignFiles
    do
      filename=`basename $prealignFile .slcio`
      runRange=${filename#prealign_*}
      first=${runRange%-*}
      if (($first<=${runNumber})); then
        last=${runRange#*-}
        if (($last>=${runNumber})); then
          if (($prealignExists==1)); then
            echo "More than one possible prealign files were found, please keep only one prealignment and one alignment file for each run" >> ${outputFolder}/analysis.log
            echo "More than one possible prealign files were found for run" ${runNumber} >> ${outputFolder}/../analysis.log
            if [ "${processingMode}" != "DEBUG" ]; then
              rm -r ${outputFolder}/lcio
              if [ -f `printf *"%06d"* ${input[0]}` ]; then
                rm `printf *"%06d"* ${input[0]}`
              fi
            fi
            exit 0
          fi
          firstFinal=$first
          lastFinal=$last
          cp $prealignFile `printf ${outputFolder}/database/run"%06d"-prealignment.slcio ${runNumber}`
          echo Copied $prealignFile >> ${outputFolder}/analysis.log
          prealignExists=1
        fi
      fi
    done
    error=`echo $?`
    if (($prealignExists == 0))
    then
      echo "Prealignment file doesn't exist" >> ${outputFolder}/analysis.log
      echo "Prealignment file doesn't exist for run" ${runNumber} >> ${outputFolder}/../analysis.log
      echo "Please create prealign file named prealign_FirstRunItIsToBeUsed-LastRunItIsToBeUsed.slcio or use run_pALPIDEfs_PS_7_wAlign to do alignment for all runs separately" >> ${outputFolder}/analysis.log
      if [ "${processingMode}" != "DEBUG" ]; then
        rm -r ${outputFolder}/lcio
        if [ -f `printf *"%06d"* ${input[0]}` ]; then
          rm `printf *"%06d"* ${input[0]}`
        fi
      fi
      exit 0
    fi
    alignFile=${outputFolder}/../align_$firstFinal-$lastFinal.slcio
    cp $alignFile `printf ${outputFolder}/database/run"%06d"-alignment.slcio ${runNumber}`
    error=`echo $?`
    if (($error > 0))
    then
      echo "Alignment file doesn't exist" >> ${outputFolder}/analysis.log
      echo "Alignment file doesn't exist for run" ${runNumber} >> ${outputFolder}/../analysis.log
      echo "Please create align file named align_FirstRunItIsToBeUsed-LastRunItIsToBeUsed.slcio or use run_pALPIDEfs_PS_7_wAlign to do alignment for all runs separately" >> ${outputFolder}/analysis.log
      if [ "${processingMode}" != "DEBUG" ]; then
        rm -r ${outputFolder}/lcio
        if [ -f `printf *"%06d"* ${input[0]}` ]; then
          rm `printf *"%06d"* ${input[0]}`
        fi
      fi
      exit 0
    fi
  fi
  if (( ${dutType} == 0 )); then
    $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} deadColumn ${runNumber} > $redirect 2>&1
  fi
  cd ${outputFolder}/logs
  unzip `printf deadColumn-"%06d".zip ${runNumber}` > /dev/null 2>&1
  deadColumnName=`printf deadColumn-"%06d".log ${runNumber}`
  averageHits=`awk '/Average number of hits per event:/{x=NR+'"${nTelescopePlanes}"';next}(NR<=x){print}' $deadColumnName | sed -n -e 's/^.*\[ MESSAGE5 \"deadColumn\"\] Layer [0-9]//p'`
  averageHitsArray=($averageHits)
  for ((i=0;i<${#averageHitsArray[@]};i++)) do
    if (( $(bc <<< "${averageHitsArray[i]} > 100") )); then
      excludedPlanes[${#excludedPlanes[@]}]=$i
    elif (( $(bc <<< "${averageHitsArray[i]} == 0") )); then
      emptyPlanes[${#emptyPlanes[@]}]=$i
    fi
  done
  echo "Excluded planes:" ${excludedPlanes[@]} >> ${outputFolder}/analysis.log
  echo "Empty planes: " ${emptyPlanes[@]} >> $5/analysis.log
  cd - > /dev/null 2>&1
  if ((${#excludedPlanes[@]}==0)); then
    excludedPlanes[0]=-1
  elif ((${#excludedPlanes[@]}>2)); then
    echo "More than 2 noise planes (planes" ${excludedPlanes[@]}"), exiting" >> ${outputFolder}/analysis.log
    echo "More than 2 noise planes (planes" ${excludedPlanes[@]}") in run" ${runNumber}", exiting" >> ${outputFolder}/../analysis.log
    if [ "${processingMode}" != "DEBUG" ]; then
      rm -r ${outputFolder}/lcio
      if [ -f `printf *"%06d"* ${input[0]}` ]; then
        rm `printf *"%06d"* ${input[0]}`
      fi
    fi
    exit 0
  fi
  if ((${#emptyPlanes[@]}==0)); then
    emptyPlanes[0]=-1
  elif ((${#emptyPlanes[@]}>1)); then
    echo "More than 1 completely empty plane, strange, exiting" >> $5/analysis.log
    echo "More than 1 completely empty plane in run" $1", strange, exiting" >> $5/../analysis.log
  fi
  if (( ${alignMethod} == 1 )); then
    maffpALPIDEfs=0.0001
    maffpALPIDEss=0.001
    $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} --option MaxAllowedFiringFreq=$maffpALPIDEfs --option MaxAllowedFiringFreqpALPIDEss=$maffpALPIDEss hotpixel ${runNumber} > $redirect 2>&1
    $EUTELESCOPE/jobsub/jobsub.py --option ExcludedPlanes="" ${commonOptions} --option LCIOInputFiles=${outputFolder}/lcio/run@RunNumber@-converter.slcio clustering ${runNumber} > $redirect 2>&1
    cd ${outputFolder}/logs/
    clusteringName=`printf clustering-"%06d".zip ${runNumber}`
    unzip $clusteringName > /dev/null 2>&1
    clusteringLogName=`printf clustering-"%06d".log ${runNumber}`
    tooNoisy="contains more than 4096 cluster"
    if grep -q "$tooNoisy" "$clusteringLogName"; then
      echo At least one plane had more than 4096 clusters in one event, too noisy exiting > ${outputFolder}/analysis.log
      echo At least one plane is too noisy run ${runNumber} > ${outputFolder}/../analysis.log
      if [ "${processingMode}" != "DEBUG" ]; then
        rm -r ${outputFolder}/lcio
        if [ -f `printf *"%06d"* ${input[0]}` ]; then
          rm `printf *"%06d"* ${input[0]}`
        fi
      fi
      exit 0
    fi
    rm *.log *.xml
    cd - > /dev/null 2>&1
    $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} hitmaker ${runNumber} > $redirect 2>&1
    $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} prealign ${runNumber} > $redirect 2>&1
    ./run_align_7 ${runNumber} ${settingsFile} ${outputFolder} ${configFile} ${emptyPlanes[0]} > $redirect 2>&1
    error=`echo $?`
    if (($error > 0))
    then
      #Quality checks if alignment failed
      outputFolderQA=${outputFolder}/Plots/
      mkdir $outputFolderQA
      mkdir $outputFolderQA/important
      mkdir $outputFolderQA/others
      if (( ${dutType} == 0)); then
        root -l -q -b qualityCheckfs.C\(${runNumber},${firstDUTid},${lastDUTid},"\"${outputFolder}/histogram\"","\"$outputFolderQA\"",${nTelescopePlanes}\) > /dev/null 2>&1
      elif (( ${dutType} == 1)); then
        root -l -q -b qualityCheckss.C\(${runNumber},${firstDUTid},${lastDUTid},"\"${outputFolder}/histogram\"","\"$outputFolderQA\"",${nTelescopePlanes}\) > /dev/null 2>&1
      fi
      echo "QA written to" $outputFolderQA >> ${outputFolder}/analysis.log
      if [ "${processingMode}" != "DEBUG" ]; then
        rm -r ${outputFolder}/lcio
        if [ -f `printf *"%06d"* ${input[0]}` ]; then
          rm `printf *"%06d"* ${input[0]}`
        fi
      fi
      exit 0
    fi
  fi
  maffpALPIDEfs=0.001
  maffpALPIDEss=1
  $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} --option MaxAllowedFiringFreq=$maffpALPIDEfs --option MaxAllowedFiringFreqpALPIDEss=$maffpALPIDEss  hotpixel ${runNumber} > $redirect 2>&1
  $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} --option LCIOInputFiles=${outputFolder}/lcio/run@RunNumber@-converter.slcio --option ExcludedPlanes="${excludedPlanes[0]} ${excludedPlanes[1]}" clustering ${runNumber} > $redirect 2>&1
  cd ${outputFolder}/logs/
  clusteringName=`printf clustering-"%06d".zip ${runNumber}`
  unzip $clusteringName > /dev/null 2>&1
  clusteringLogName=`printf clustering-"%06d".log ${runNumber}`
  tooNoisy="contains more than 4096 cluster"
  if grep -q "$tooNoisy" "$clusteringLogName"; then
    echo At least one plane had more than 4096 clusters in one event, too noisy exiting > ${outputFolder}/analysis.log
    echo At least one plane is too noisy run ${runNumber} > ${outputFolder}/../analysis.log
    if [ "${processingMode}" != "DEBUG" ]; then
      rm -r ${outputFolder}/lcio
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
  minTimeStamp=`echo "$minTimeStamp+${extraBusyTime}" | bc -l`
  if (( ${extraBusyTime}==0)); then
    minTimeStamp=0
  fi
  rm *.log *.xml
  cd - > /dev/null 2>&1
  $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} hitmaker ${runNumber} > $redirect 2>&1

  for ((i=${firstDUTid};i<=${lastDUTid};i++)) do
    isExcluded=0
    for ((j=0;j<${#excludedPlanes[@]};j++)) do
      if ((${excludedPlanes[j]}==$i)); then
        isExcluded=1
        break
      fi
    done
    if (($isExcluded==1)); then
      continue
    fi
    if ((${emptyPlanes[0]}==$i)); then
      continue
    fi
    $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} --option dutID="$i" fitter ${runNumber} > $redirect 2>&1
    cd ${outputFolder}/logs/
    unzip `printf fitter-"%06d".zip ${runNumber}` > /dev/null 2>&1
    fitterName=`printf fitter-"%06d".log ${runNumber}`
    nTrack=`cat $fitterName |  sed -n -e "s/^.*Total number of reconstructed tracks//p" | bc -l`
    rm *.log *.xml
    cd - > /dev/null 2>&1
    if (($nTrack<100))
    then
      echo "Too few tracks found in DUT" $i", moving to next DUT" >> ${outputFolder}/analysis.log
      echo "Process for DUT" $i "in run" ${runNumber} "exited with fitting error" >> ${outputFolder}/../analysis.log
      continue
    else
      echo $nTrack "tracks used in DUT" $i >> ${outputFolder}/analysis.log
    fi
    if [ -f ${outputFolder}/../settings_DUT$i.txt ]; then
      sed -i '/^'${runNumber}'/d' ${outputFolder}/../settings_DUT$i.txt
    fi
    if [ -f ${outputFolder}/settings_DUT$i.txt ]; then
     rm ${outputFolder}/settings_DUT$i.txt
    fi
    echo "Using events with a timestamp larger than" $minTimeStamp >> ${outputFolder}/analysis.log
    $EUTELESCOPE/jobsub/jobsub.py ${commonOptions} --option dutID="$i" --option MinTimeStamp=$minTimeStamp analysis ${runNumber} > $redirect 2>&1
    if ! [ -f ${outputFolder}/../settings_DUT$i.txt ]; then
      cat ${outputFolder}/settings_DUT$i.txt > ${outputFolder}/../settings_DUT$i.txt
    else
      tail -n 1 ${outputFolder}/settings_DUT$i.txt >> ${outputFolder}/../settings_DUT$i.txt
    fi
    sed -i 's/nan/0/g' ${outputFolder}/../settings_DUT$i.txt
    cd ${outputFolder}/logs/
    unzip `printf analysis-"%06d".zip ${runNumber}` > /dev/null 2>&1
    analysisName=`printf analysis-"%06d".log ${runNumber}`
    if (( ${dutType} == 0)); then
      efficiencies=`awk '/Overall efficiency of pALPIDEfs sectors/{x=NR+4;next}(NR<=x){print}' $analysisName | sed -n -e 's/^.*\[ MESSAGE4 \"Analysis\"\] //p'`
      rm *.log *.xml
      cd - > /dev/null 2>&1
      echo "Efficiencies of the four sectors in DUT" $i":" >> ${outputFolder}/analysis.log
      effArray=($efficiencies)
      for ((j=1;j<=10;j=j+3)) do
        echo ${effArray[j-1]} >> ${outputFolder}/analysis.log
      done
    elif (( ${dutType} == 1)); then
      echo "Efficiencies of the four sectors in DUT" $i":" >> ${outputFolder}/analysis.log
      effArray=($efficiencies)
      cat $analysisName | grep Sector | sed -n -e 's/^.*\[ MESSAGE4 \"Analysis\"\] //p' >> ${outputFolder}/analysis.log
      rm *.log *.xml
      cd - > /dev/null 2>&1
    fi
    mv `printf ${outputFolder}/logs/analysis-"%06d".zip ${runNumber}` `printf ${outputFolder}/logs/analysis-"%06d"_DUT$i.zip ${runNumber}`
    echo "Processing of DUT" $i "exited without errors for run" ${runNumber} >> ${outputFolder}/../analysis.log
    echo "Processing of DUT" $i "exited without errors" >> ${outputFolder}/analysis.log
  done
  if [ "${processingMode}" != "DEBUG" ]; then
    find ${outputFolder}/lcio -type f -not -name '*track*' | xargs rm # To keep only the output of the fitter
#    rm -r ${outputFolder}/lcio ${outputFolder}/database # To delete all intermediate steps
  fi
  #Quality checks
  outputFolderQA=${outputFolder}/Plots/
  mkdir $outputFolderQA
  mkdir $outputFolderQA/important
  mkdir $outputFolderQA/others
  if (( ${dutType} == 0)); then
    root -l -q -b qualityCheckfs.C\(${runNumber},${firstDUTid},${lastDUTid},"\"${outputFolder}/histogram\"","\"$outputFolderQA\"",${nTelescopePlanes}\) > /dev/null 2>&1
  elif (( ${dutType} == 1)); then
    root -l -q -b qualityCheckss.C\(${runNumber},${firstDUTid},${lastDUTid},"\"${outputFolder}/histogram\"","\"$outputFolderQA\"",${nTelescopePlanes}\) > /dev/null 2>&1
  fi
  echo "QA written to" $outputFolderQA >> ${outputFolder}/analysis.log
else
  echo -n -e "Not able to decide if it's noise or data. Please use one of the following settings in dataProcessing: \n  0: decide from the data if it's noise or data run \n  1: force it to be treated as data, \n  2: force it to be treated as noise \n"
fi
