#! /bin/bash

rawDataFolder=<rawDataFolder>
outputFolder=<outputFolder>
settingsFile=<settingsFile>
configFile=<configFile>
withAlign=<withAlign> #1 for aligning each run separately and 0 for using commmon alignment

if [ -z "$EUTELESCOPE" ]; then
  source /afs/cern.ch/work/m/mkofarag/private/ILCSOFT/v01-17-05/Eutelescope/trunk/build_env.sh
fi

if ! [ -d $outputFolder ]; then
  mkdir -p $outputFolder
fi 

re='^[0-9]+$'

IFS=$'\n'
for line in $(cat $settingsFile)
do
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
    if [[ $2 =~ $re ]]; then
      if ! [[ $2 == ${input[0]} ]] ; then
        continue
      else
        rm -r `printf $outputFolder/run"%06d" ${input[0]}`
        echo -e "\n \n \n REPROCESSING RUN ${input[0]}!!! \n \n \n"
      fi
    else
      echo -e "\n \n \n REPROCESSING ALL RUNS, CURRENTLY AT RUN ${input[0]}!!! \n \n \n"
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
  cd -
  cd $EUDAQ
  echo -e "\n \n \n \nEUDAQ" >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git status >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git diff   >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git log -1 >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  cd -
  echo -e "\n \n\n \npalpidefs_scripts:" >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git status >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git diff   >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git log -1 >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  if (( $withAlign==0 )); then
    if [ "$1" == "DEBUG" ]; then
      ./run_pALPIDEfs_PS_7_woAlign ${input[0]} ${DUT[0]} ${DUT[${#DUT[@]}-1]} $settingsFile `printf $outputFolder/run"%06d" ${input[0]}` $rawDataFolder ${#chips[@]} $configFile $1
    else 
      ./run_pALPIDEfs_PS_7_woAlign ${input[0]} ${DUT[0]} ${DUT[${#DUT[@]}-1]} $settingsFile `printf $outputFolder/run"%06d" ${input[0]}` $rawDataFolder ${#chips[@]} $configFile $1 &
      sleep 5
    fi
  else
    if [ "$1" == "DEBUG" ]; then
      ./run_pALPIDEfs_PS_7_wAlign ${input[0]} ${DUT[0]} ${DUT[${#DUT[@]}-1]} $settingsFile `printf $outputFolder/run"%06d" ${input[0]}` $rawDataFolder ${#chips[@]} $configFile $1
    else 
      ./run_pALPIDEfs_PS_7_wAlign ${input[0]} ${DUT[0]} ${DUT[${#DUT[@]}-1]} $settingsFile `printf $outputFolder/run"%06d" ${input[0]}` $rawDataFolder ${#chips[@]} $configFile $1 &
      sleep 5
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
  echo -e "\n \n \n Running in alignment mode \n \n \n" 
  outputAlign=$outputFolder/align/
  outputFolder=$outputAlign
  echo $outputFolder
  if ! [ -d $outputFolder ]; then
    mkdir $outputFolder
  fi 
  mkdir `printf $outputFolder/run"%06d"-"%06d" $3 $4` `printf $outputFolder/run"%06d"-"%06d"/histogram $3 $4` `printf $outputFolder/run"%06d"-"%06d"/lcio $3 $4` `printf $outputFolder/run"%06d"-"%06d"/database $3 $4` `printf $outputFolder/run"%06d"-"%06d"/logs $3 $4`
  cd $EUTELESCOPE
  echo "EUTELESCOPE: " > `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git status >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git diff   >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git log -1 >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  cd -
  cd $EUDAQ
  echo -e "\n \n \n \nEUDAQ" >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git status >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git diff   >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git log -1 >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  cd -
  echo -e "\n \n\n \npalpidefs_scripts:" >> `printf $outputFolder/run"%06d"-"%06d"/git_status.txt $3 $4`
  git status >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git diff   >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git log -1 >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  ./run_createAlign $2 $settingsFile `printf $outputFolder/run"%06d"-"%06d" $3 $4` $rawDataFolder $configFile $3 $4
fi
