#! /bin/bash

rawDataFolder=<rawDataFolder>
outputFolder=<outputFolder>
settingsFile=<runlist>
configFile=<configFile>
if ! [ -d $outputFolder ]; then
  mkdir $outputFolder
fi 

re='^[0-9]+$'

IFS=$'\n'
for line in $(cat $settingsFile)
do           
  IFS=',' read -ra input <<< "$line"
  if ! [[ ${input[0]} =~ $re ]] ; then
    continue
  fi
  if ! [ -f `printf $rawDataFolder/run"%06d".raw ${input[0]}` ]; then
    continue
  fi
  if [ -d `printf $outputFolder/run"%06d" ${input[0]}` ] && ! [ "$1" == "REPROCESS" ] && ! [ "$1" == "DEBUG" ]; then
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
    rm -r `printf $outputFolder/run"%06d" ${input[0]}`
    echo -e "\n \n \n REPROCESSING RUN ${input[0]}!!! \n \n \n"
  fi
  mkdir `printf $outputFolder/run"%06d" ${input[0]}` `printf $outputFolder/run"%06d"/histogram ${input[0]}` `printf $outputFolder/run"%06d"/lcio ${input[0]}` `printf $outputFolder/run"%06d"/database ${input[0]}` `printf $outputFolder/run"%06d"/logs ${input[0]}`
  IFS=' ' read -ra DUT <<< "${input[3]}"
  IFS=' ' read -ra chips <<< "${input[4]}"
  cd $EUTELESCOPE
  git status > `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git diff   >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  git log -1 >> `printf $outputFolder/run"%06d"/git_status.txt ${input[0]}`
  cd -
  if [ "$1" == "DEBUG" ]; then
    ./run_pALPIDEfs_PS_7_woAlign ${input[0]} ${DUT[0]} ${DUT[${#DUT[@]}-1]} $settingsFile `printf $outputFolder/run"%06d" ${input[0]}` $rawDataFolder ${#chips[@]} $configFile $1
  else 
    srun ./run_pALPIDEfs_PS_7_woAlign ${input[0]} ${DUT[0]} ${DUT[${#DUT[@]}-1]} $settingsFile `printf $outputFolder/run"%06d" ${input[0]}` $rawDataFolder ${#chips[@]} $configFile $1 &
  fi
  sleep 5
done
