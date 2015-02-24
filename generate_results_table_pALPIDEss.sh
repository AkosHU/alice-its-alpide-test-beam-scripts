#! /bin/bash

rawDataFolder=/data0/alice_u/freidt/pALPIDEss/testbeam/2015/DESY/2015-02-09/pALPIDEss
outputFolder=/data0/alice_u/freidt/pALPIDEss/testbeam/2015/DESY/2015-02-09/pALPIDEss/output
settingsFile=/data0/alice_u/freidt/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/pALPIDEss/runlist_pALPIDEss.csv
configFile=/data0/alice_u/freidt/ilcsoft/v01-17-05/Eutelescope/trunk/jobsub/examples/pALPIDEss/config_pALPIDEss.cfg
withAlign=1 #1 for aligning each run separately and 0 for using commmon alignment
whichChip=1 #0 for full scale, 1 for small scale
runFirst=88
runLast=88 #4
dutID=3

if [ -z "$EUTELESCOPE" ]; then
  source ../v01-17-05/Eutelescope/trunk/build_env.sh #Change to your EUTelescope folder if you changed the folder structure with respest to the default after installing
fi

for r in $(seq ${runFirst} ${runLast})
do
    r_str=$(printf "run%06d" $r)
    settings_line=$(cat $settingsFile | grep "^$r,")
    testreader_out=$($EUDAQ/bin/TestReader.exe -b ${outputFolder}/${r_str}/${r_str}.raw)
    echo ${r_str}
    echo ${settings_line}
    energy=$(echo ${settings_line} | cut -d',' -f 2)
    dut=$(echo ${settings_line} | cut -d',' -f 5 | cut -d' ' -f $(($dutID + 1)))
    rad_level=$(echo ${settings_line} | cut -d',' -f 6 | cut -d' ' -f $(($dutID + 1)))
    rate=$(echo ${settings_line} | cut -d',' -f 7)
    for sub_str in ${testreader_out}
    do
        case $sub_str in
            "Vbb="*)
                vbb=${sub_str:4}
                ;;
            "Vcasn="*)
                vcasn=${sub_str:6}
                ;;
            "Vcasp="*)
                vcasp=${sub_str:6}
                ;;
            "Vrst="*)
                vrst=${sub_str:5}
                ;;
            "Vlight="*)
                vlight=${sub_str:7}
                ;;
            "AcqTime="*)
                acqTime=${sub_str:8}
                ;;
            "TrigDelay="*)
                trigDelay=${sub_str:10}
                ;;
            "palpideteam"*)
                config=${sub_str}
                ;;
        esac
    done
    echo ${vbb}
    echo ${vcasn}
    echo ${vcasp}
    echo ${vrst}
    echo ${vlight}
    echo ${acqTime}
    echo ${trigDelay}
    echo ${config}
done
