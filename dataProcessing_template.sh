#! /bin/bash
#
# Optional parameters of the script:
# 1 : ALIGN, DEBUG or REPROCESS
# 2 : run_number, first_run-last_run
# 3 : step (e.g. converter)
# 4 : additional_option
#
# Examples how to run the script:
#    ./dataProcessing.sh 
#    ./dataProcessing.sh REPROCESS
#    ./dataProcessing.sh REPROCESS run_number
#    ./dataProcessing.sh REPROCESS first_run-last_run
#    ./dataProcessing.sh DEBUG run_number
#    ./dataProcessing.sh DEBUG run_number step additional_option 
#    ./dataProcessing.sh ALIGN "1 2 3 4" 1 10
#
rawDataFolder=<rawDataFolder>
outputFolder=<outputFolder>
settingsFile=<settingsFile>
configFile=<configFile>
withAlign=<withAlign> #1 for aligning each run separately and 0 for using commmon alignment
whichChip=<whichChip> #0 for small scale, 1/2/3 for pALPIDE-1/2/3
extraBusyTime=<extraBusyTime> #Time to add after normal busy in which events are not considered (in clock cycles). Used for past protection to avoid efficiency loss because of pulse duration differences in tracking planes and DUTs. Only working for pALPIDEfs DUTs
isNoise=<isNoise> #0: decide from the data if it's noise or data run, 1: force it to be treated as data, 2: force it to be treated as noise

if [ -z "$EUTELESCOPE" ]; then
  source ../v01-17-05/Eutelescope/trunk/build_env.sh #Change to your EUTelescope folder if you changed the folder structure with respest to the default after installing
fi

if (( $withAlign!=0 && $withAlign!=1 )); then
  echo -n -e "Wrong setting for withAlign, please use \n  1 for aligning each run separately and \n  0 for using commmon alignment \n"
  exit 1
fi

if (( $whichChip!=0 && $whichChip!=1 && $whichChip!=2 && $whichChip!=3 )); then
  echo -n -e "Wrong setting for whichChip, please use \n  0 for small scale \n  1/2/3 for pALPIDEfs \n"
  exit 1
fi

if (( $isNoise<0 || $isNoise>2 )); then
  echo -n -e "Not able to decide if it's noise or data. Please use one of the following settings in dataProcessing: \n  0: decide from the data if it's noise or data run \n  1: force it to be treated as data, \n  2: force it to be treated as noise \n"
  exit 1
fi

if ! [ -d $outputFolder ]; then
  mkdir -p $outputFolder
fi

re='^[0-9]+$'

fileFound=0

# check whether we can use slurm
command -v sbatch >/dev/null 2>&1
if [[ "$?" -eq 0 ]]
then
  CMD_PREFIX=srun
else
  CMD_PREFIX=
fi

IFS=$'\n'
for line in $(cat $settingsFile)
do
  if [ "$CMD_PREFIX" == "srun" ] && [ "$1" != "DEBUG" ]; then
    if (( $(($(squeue -u $USER | wc -l) -1)) > 100 )); then
      echo "("$(date)") More than a 100 jobs queued for slurm, waiting for one to finish before submitting the next" >> $outputFolder/analysis.log
      echo -e "More than a 100 jobs queued for slurm, waiting for one to finish before submitting the next"
      while (( $(($(squeue -u $USER | wc -l) -1)) > 100 ))
      do
        sleep 10
      done
      echo "("$(date)") Less than a 100 jobs queued for slurm, starting submitting new ones" >> $outputFolder/analysis.log
      echo "Less than a 100 jobs queued for slurm, starting submitting new ones"
    fi
  fi

  if [ "$1" == "ALIGN" ]; then
    break
  fi
  IFS=',' read -ra input <<< "$line"
  if ! [[ ${input[0]} =~ $re ]] ; then
    continue
  fi
  if ! [ -f `printf $rawDataFolder/run"%06d".raw ${input[0]}` ]; then
    continue
  fi
  if [ "$#" -gt 0 ] && ! [ "$1" == "REPROCESS" ] && ! [ "$1" == "DEBUG" ] && ! [ "$1" == "ALIGN" ]; then
    echo "Not a valid option! Choose between REPROCESS, DEBUG, ALIGN or nothing"
    exit 1
  fi
  if [ -d `printf $outputFolder/run"%06d" ${input[0]}` ] && ! [ "$1" == "REPROCESS" ] && ! [ "$1" == "DEBUG" ] && ! [ "$1" == "ALIGN" ] ; then
    continue
  fi
  if [ "$1" == "DEBUG" ]; then
    if ! [[ $2 =~ $re ]] ; then
      echo "Runnumber needed as second argument for debugging"
      exit 1
    elif ! [[ $2 == ${input[0]} ]] ; then
      continue
    fi
    echo -e "\n \n \n REPROCESSING RUN ${input[0]}!!! \n \n \n"
  elif [ "$1" == "REPROCESS" ]; then
    if [ "$#" -gt 1 ]; then
      if [[ $2 =~ $re ]]; then
        if ! [[ $2 == ${input[0]} ]] ; then
          continue
        else
          echo "REPROCESSING RUN ${input[0]}!!!"
          rm -r `printf $outputFolder/run"%06d" ${input[0]}`
        fi
      else
        IFS='-' read -ra range <<< "$2"
        if (( ${#range[@]} != 2)); then
          echo "Add one run number or a range in the format 1-10 or nothing as argument to REPROCESS"
         exit 1
        fi
        if ! [[ ${range[0]} =~ $re ]] || ! [[ ${range[1]} =~ $re ]]; then
          echo "Not a number"
          echo "Add one run number or a range in the format 1-10 or nothing as argument to REPROCESS"
          exit 1
        fi
        for (( run=${range[0]}; run<=${range[1]}; run++ ))
        do
          runFound=0
          if ! [[ $run == ${input[0]} ]] ; then
            continue
          else
            runFound=1
            echo "REPROCESSING RUN IN RANGE ${range[0]}-${range[1]}, CURRENTLY AT RUN ${input[0]}!!!"
            rm -r `printf $outputFolder/run"%06d" ${input[0]}`
            break
          fi
        done
        if (( $runFound==0 ));then
          continue
        fi
      fi
    else
      echo "REPROCESSING ALL RUNS, CURRENTLY AT RUN ${input[0]}!!!"
      rm -r `printf $outputFolder/run"%06d" ${input[0]}`
    fi
  fi
  mkdir `printf $outputFolder/run"%06d" ${input[0]}` `printf $outputFolder/run"%06d"/histogram ${input[0]}` `printf $outputFolder/run"%06d"/lcio ${input[0]}` `printf $outputFolder/run"%06d"/database ${input[0]}` `printf $outputFolder/run"%06d"/logs ${input[0]}`
  IFS=' ' read -ra DUT <<< "${input[3]}"
  IFS=' ' read -ra chips <<< "${input[4]}"
  cd $EUTELESCOPE
  echo "EUTELESCOPE: " > `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git status >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git diff   >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git log -1 >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  cd - > /dev/null
  cd $EUDAQ
  echo -e "\n \n \n \nEUDAQ" >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git status >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git diff   >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git log -1 >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  cd - > /dev/null
  echo -e "\n \n\n \npalpidefs_scripts:" >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git status >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git diff   >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git log -1 >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  if [ "$1" != "DEBUG" ]; then
    if [ "$#" -eq 0 ]; then
      echo "Processing of run" ${input[0]} "started"
      $CMD_PREFIX ./run_processing.sh ${input[0]} ${DUT[0]} ${DUT[${#DUT[@]}-1]} $settingsFile `printf $outputFolder/run"%06d" ${input[0]}` $rawDataFolder ${#chips[@]} $configFile $whichChip $withAlign "NORMAL" $extraBusyTime $isNoise > `printf $outputFolder/run"%06d"/crash.log ${input[0]}` 2>&1 &
    else
      $CMD_PREFIX ./run_processing.sh ${input[0]} ${DUT[0]} ${DUT[${#DUT[@]}-1]} $settingsFile `printf $outputFolder/run"%06d" ${input[0]}` $rawDataFolder ${#chips[@]} $configFile $whichChip $withAlign $1 $extraBusyTime $isNoise > `printf $outputFolder/run"%06d"/crash.log ${input[0]}` 2>&1 &
    fi
    fileFound=1
    sleep 5
  elif [ "$#" -eq 2 ]; then
    ./run_processing.sh ${input[0]} ${DUT[0]} ${DUT[${#DUT[@]}-1]} $settingsFile `printf $outputFolder/run"%06d" ${input[0]}` $rawDataFolder ${#chips[@]} $configFile $whichChip $withAlign $1 $extraBusyTime $isNoise
    fileFound=1
  else
    argArr1=(converter deadColumn hotpixel clustering hitmaker prealign align fitter analysis noise)
    argArr2=(fitter analysis)
    argArr3=(sync converter)
    if (( $whichChip == 0)) && [[ " ${argArr3[*]} " == *"$3"* ]]; then
      if [ -a `printf $outputFolder/run"%06d"/run"%06d".raw $2 $2` ]; then
        rm -r `printf $outputFolder/run"%06d"/run"%06d".raw $2 $2`
      fi
      $EUDAQ/bin/Converter.exe -s -t native `printf $rawDataFolder/run"%06d".raw $2` -o `printf $outputFolder/run"%06d"/run"%06d".raw $2 $2`
      if [ "$3" == "converter" ]; then
        $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=`printf $outputFolder/run"%06d"/database ${input[0]}` --option HistogramPath=`printf $outputFolder/run"%06d"/histogram ${input[0]}` --option LcioPath=`printf $outputFolder/run"%06d"/lcio ${input[0]}` --option LogPath=`printf $outputFolder/run"%06d"/logs ${input[0]}` --option NativePath=`printf $outputFolder/run"%06d"/ ${input[0]}` --config=$configFile -csv $settingsFile $3 $2
        exit 0
      fi
    elif [[ " ${argArr1[*]} " == *"$3"* ]]; then
      res=64.38

      if [[ " ${argArr2[*]} " == *"$3"* ]] && [ "$#" -ne 4 ]; then
        echo "Give as 4th argument the DUT ID"
        exit 1
      fi
      $EUTELESCOPE/jobsub/jobsub.py --option DatabasePath=`printf $outputFolder/run"%06d"/database ${input[0]}` --option HistogramPath=`printf $outputFolder/run"%06d"/histogram ${input[0]}` --option LcioPath=`printf $outputFolder/run"%06d"/lcio ${input[0]}` --option LogPath=`printf $outputFolder/run"%06d"/logs ${input[0]}` --option NativePath=$rawDataFolder --option MaxAllowedFiringFreqpALPIDEss=1 --option MaxAllowedFiringFreq=1 --option ExcludedPlanes="" --option ExcludePlanes="" --option LCIOInputFiles=`printf $outputFolder/run"%06d"/lcio/run@RunNumber@-converter.slcio ${input[0]}` --option dutID="$4" --option ResolutionX="$res $res $res $res $res $res $res" --option ResolutionY="$res $res $res $res $res $res $res" --option MinTimeStamp=0 --option ChipVersion=${whichChip} --config=$configFile -csv $settingsFile $3 $2
      exit 0
    else
      echo "Wrong argument, please run without 3rd argument or give one of the following as 3rd argument: converter deadColumn hotpixel clustering hitmaker prealign align fitter analysis noise"
      exit 1
    fi
  fi
done

if [ "$1" == "ALIGN" ]; then
  IFS=' ' read -ra runs <<< "$2"
  if (( ${#runs[@]}==0 )); then
    echo "Give as second argument run numbers to be used for aligning "
    exit 1
  fi
  if ! [[ $3 =~ $re ]] && ! [[ $4 =~ $re ]]; then
    echo "Please add as third and fourth argument the first and last run for which the produced alignment should be used for"
    exit 1
  fi
  for ((iRun=0; iRun<${#runs[@]}; iRun++)) do
    if ! [[ ${runs[iRun]} =~ $re ]] ; then
      echo "Give as second argument run numbers to be used for aligning "
      exit 1
    else
      found=0
      for line in $(cat $settingsFile)
      do
        IFS=',' read -ra input <<< "$line"
        if ! [[ ${input[0]} =~ $re ]] ; then
          continue
        fi
        if ! [ -f `printf $rawDataFolder/run"%06d".raw ${input[0]}` ]; then
          continue
        fi
        if (( ${runs[iRun]} == ${input[0]} )); then
          found=1
        fi
      done
      if (( $found==0)); then
        echo ${runs[iRun]} "not found in runlist"
        exit 1
      fi
    fi
  done
  echo -e " Running in alignment mode"
  outputAlign=$outputFolder/align/
  outputFolder=$outputAlign
  echo $outputFolder
  if ! [ -d $outputFolder ]; then
    mkdir $outputFolder
  fi
  mkdir `printf $outputFolder/run"%06d"-"%06d" $3 $4` `printf $outputFolder/run"%06d"-"%06d"/histogram $3 $4` `printf $outputFolder/run"%06d"-"%06d"/lcio $3 $4` `printf $outputFolder/run"%06d"-"%06d"/database $3 $4` `printf $outputFolder/run"%06d"-"%06d"/logs $3 $4`
  cd $EUTELESCOPE
  echo "EUTELESCOPE: " > `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git status >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git diff   >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git log -1 >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  cd - > /dev/null
  cd $EUDAQ
  echo -e "\n \n \n \nEUDAQ" >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git status >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git diff   >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git log -1 >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  cd - > /dev/null
  echo -e "\n \n \n \npalpidefs_scripts:" >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git status >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git diff   >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git log -1 >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  $CMD_PREFIX ./run_createAlign $2 $settingsFile `printf $outputFolder/run"%06d"-"%06d" $3 $4` $rawDataFolder $configFile $3 $4  > `printf $outputFolder/run"%06d"-"%06d"/crash.log $3 $4` 2>&1 &

  fileFound=1
fi

if (( $fileFound == 0)); then
  if [ "$#" -eq 0 ]; then
    echo "No new runs to be processed"
  else
    echo "Run not found in settings file or the raw data does not exist"
  fi
else
  echo "All runs sent in for processing"
fi
