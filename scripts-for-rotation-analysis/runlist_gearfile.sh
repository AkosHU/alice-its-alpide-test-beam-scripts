#!/bin/bash
#This file creates the runlist and the gearfiles for rotation according to the logbook. The logbook has to be a textfile and contain the information of the angles in the config file description. At the moment, the config file contains phi and theta as 8th and 9th parameter and the config file is the 6th column in the logbook. Also, the chip numbers should be checked below. Change below also the files.
#Files
file="LOGBOOKFILE"
runlist="WHERE TO SAVE THE NEW RUNLIST"
gearfile_old="A TEMPLATE GEARFILE, WHERE THE ROTATION PARAMETERS ARE PUT INTO"
gearfilepath="PATH, WHERE THE NEW GEARFILES ARE SUPPOSED TO BE SAVED"
echo "RunNumber,BeamEnergy,GearGeoFile,DUTNumbers,ChipIDs,IrradiationLevel,Rate" > $runlist
linestoskip=2
{ for i in {0..$linestoskip}; do
        read
done
while IFS='' read -r line || [[ -n "$line" ]]; do
    IN=$line 
    set -- "$IN" 
    IFS=","; declare -a Array=($*)

    IN2=${Array[5]}
    set -- "$IN2"
    IFS="_"; declare -a Array2=($*)
    
    phiPrefix="DUTPhi"
    thetaPrefix="DUTTheta"
    
    phi=`expr 180 - ${Array2[7]#$phiPrefix}`
    theta=`expr 90 - ${Array2[8]#$thetaPrefix}`
 
    #create runlist
    gearfile="gear_pALPIDEfs3_withCarrierMaterial_Phi${phi}_Theta${theta}.xml" 
    #RunNumber,BeamEnergy,GearGeoFile,DUTNumbers,ChipIDs,IrradiationLevel,Rate
    echo "${Array[0]},6.0,$gearfile,3,W5-39 W2-25 W2-31 W4-21 W9-16 W9-38 W2-23,0 0 0 0 0 0 0,PS" >> $runlist

    #create specific gearFile
    gearfilename="$gearfilepath/$gearfile"
    
    #check, if gearFile exists and if not, create it, as it is wanted.    
    if [ ! -f "$gearfilename" ];then
    	sed "70a\ \t\t\trotationZY=\"${theta}.0\"        rotationZX=\"${phi}.0\"        rotationXY=\"0.0\"" "$gearfile_old" | sed '70d' > "$gearfilename"
    	echo "The file '$gearfilename' was created."
    fi
done } < "$file"
