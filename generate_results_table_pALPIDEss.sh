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

echo "#Run number;Energy;Chip ID;Irradiation level(0-nonIrradiated,1-2.5e12,2-1e13,3-700krad,4-combined:1e13+700krad);Rate;Vbb;Ithr;Vcasn;Vcasp;Vrst;Vlight;Trigger delay;Acq time; Data (1) or noise (0);Efficiency (0);Number of tracks (0);Number of tracks with associated hit (0);Efficiency(1);Number of tracks(1);Number of tracks with associated hit(1);Efficiency(2);Number of tracks(2);Number of tracks with associated hit(2);Efficiency(3);Number of tracks(3);Number of tracks with associated hit(3);Config Filename;Event Count;Tracks Alignment;Tracks Fitted" > ${resultsTable}

for r in $(seq ${runFirst} ${runLast})
do
    rStr=$(printf "run%06d" $r)
    settingsLine=$(cat $settingsFile | grep "^$r,")
    energy=$(echo ${settingsLine} | cut -d',' -f 2)
    dut=$(echo ${settingsLine} | cut -d',' -f 5 | cut -d' ' -f $(($dutID + 1)))
    radLevel=$(echo ${settingsLine} | cut -d',' -f 6 | cut -d' ' -f $(($dutID + 1)))
    rate=$(echo ${settingsLine} | cut -d',' -f 7)

    while read l;
    do
        index=$(echo $l | cut -f1 -d' ')
        efficiency[${index}]=$(echo $l | cut -f2 -d' ')
        refTracks[${index}]=$(echo $l | cut -f3 -d' ')
        matchedTracks[${index}]=$(echo $l | cut -f4 -d' ')
    done < <(cat ${outputFolder}/${rStr}/analysis.log | grep Sector | cut -d' ' -f2 | sed 's/://g')

    eventCnt=$(cat ${outputFolder}/${rStr}/analysis.log | grep "good events" | cut -f3 -d' ')
    alignmentTracks=$(cat ${outputFolder}/${rStr}/analysis.log | grep "alignment" | cut -f6 -d' ')
    fittedTracks=$(cat ${outputFolder}/${rStr}/analysis.log | grep "tracks used in DUT" | cut -f1 -d' ')

    testreaderOutput=$($EUDAQ/bin/TestReader.exe -b ${outputFolder}/${rStr}/${rStr}.raw)
    for subStr in ${testreaderOutput}
    do
        case $subStr in
            "Vbb="*)
                vbb=${subStr:4}
                ;;
            "Ithr="*)
                ithr=${subStr:5}
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
    #echo ${eventCnt}
    #echo ${alignmentTracks}
    #echo ${fittedTracks}
    printf ${r}";"${energy}";"${dut}";"${radLevel}";"${rate}";"${vbb}";"${ithr}";"${vcasn}";"${vcasp}";"${vrst}";"${vlight}";"${trigDelay}";"${acqTime}";"1";" >> ${resultsTable}
    for i in $(seq 0 3)
    do
        printf ${efficiency[i]}";"${refTracks[i]}";"${matchedTracks[i]}";" >> ${resultsTable}
    done
    printf ${config}";"${eventCnt}";"${alignmentTracks}";"${fittedTracks}"\n" >> ${resultsTable}
done
