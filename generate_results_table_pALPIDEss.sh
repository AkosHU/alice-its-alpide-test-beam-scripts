#! /bin/bash

rawDataFolder=/data0/alice_u/freidt/pALPIDEss/testbeam/2015/DESY/2015-02-09/pALPIDEss
outputFolder=/data0/alice_u/freidt/pALPIDEss/testbeam/2015/DESY/2015-02-09/pALPIDEss/output
settingsFile=/data0/alice_u/freidt/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/pALPIDEss/runlist_pALPIDEss.csv
configFile=/data0/alice_u/freidt/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/pALPIDEss/config_pALPIDEss.cfg
withAlign=1 #1 for aligning each run separately and 0 for using commmon alignment
whichChip=1 #0 for full scale, 1 for small scale
runFirst=88
runLast=894
dutID=3
resultsTable=/data0/alice_u/freidt/pALPIDEss/testbeam/2015/DESY/2015-02-09/pALPIDEss/output/results.csv

if [ -z "$EUTELESCOPE" ]; then
  source ../v01-17-05/Eutelescope/trunk/build_env.sh #Change to your EUTelescope folder if you changed the folder structure with respest to the default after installing
fi

for r in $(seq ${runFirst} ${runLast})
do
    rStr=$(printf "run%06d" $r)
    settingsLine=$(cat $settingsFile | grep "^$r,")
    energy=$(echo ${settingsLine} | cut -d',' -f 2)
    dut=$(echo ${settingsLine} | cut -d',' -f 5 | cut -d' ' -f $(($dutID + 1)))
    radLevel=$(echo ${settingsLine} | cut -d',' -f 6 | cut -d' ' -f $(($dutID + 1)))
    rate=$(echo ${settingsLine} | cut -d',' -f 7)

    efficiencyLine=$(cat ${outputFolder}/${rStr}/analysis.log | grep efficiency )
    efficiency=$(echo $efficiencyLine | cut -d' ' -f 5)
    refTracks=$(echo $efficiencyLine | cut -d' ' -f 6)
    matchedTracks=$(echo $efficiencyLine | cut -d' ' -f 7)

    testreaderOutput=$($EUDAQ/bin/TestReader.exe -b ${outputFolder}/${rStr}/${rStr}.raw)
    for subStr in ${testreaderOutput}
    do
        case $subStr in
            "Vbb="*)
                vbb=${subStr:4}
                ;;
            "Vcasn="*)
                vcasn=${subStr:6}
                ;;
            "Vcasp="*)
                vcasp=${subStr:6}
                ;;
            "Vrst="*)
                vrst=${subStr:5}
                ;;
            "Vlight="*)
                vlight=${subStr:7}
                ;;
            "AcqTime="*)
                acqTime=${subStr:8}
                ;;
            "TrigDelay="*)
                trigDelay=${subStr:10}
                ;;
            "palpideteam"*)
                config=${subStr}
                ;;
        esac
    done
    #echo ${energy}
    #echo ${dut}
    #echo ${radLevel}
    #echo ${rate}
    #echo ${efficiency}
    #echo ${refTracks}
    #echo ${matchedTracks}
    #echo ${vbb}
    #echo ${vcasn}
    #echo ${vcasp}
    #echo ${vrst}
    #echo ${vlight}
    #echo ${acqTime}
    #echo ${trigDelay}
    #echo ${config}
    echo ${r},${energy},${dut},${radLevel},${rate},${vbb},${vcasn},${vcasp},${vrst},${vlight},${trigDelay},${acqTime},1,${efficiency},${refTracks},${matchedTracks},${config} >> ${resultsTable}
done
