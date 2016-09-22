#this file simulates the extrapolation error for depending on the angle of an inclination of a plane. It calculates three datapoints, one from the origin of the rotated plane, one from the closest and one from the furthest point to the previous plane in beam direction.
#It requires the logbook as a text file and that trackResolutionCalculation.cc was compiled and the executable is in the same directory.
#Please change the path to the results below, as well as the logbook link (search for logbook in this file). The logbook must be a text file with the config file name as 6th column. The config file must contain phi as 8th and theta as 9th argument. If things change, change this, as well as possible prefixes.

trackResolution()
{
    phi="$1"
    theta="$2"
    pi=$(echo "scale=100; 4*a(1)" | bc -l)
    phirad=$(echo "$phi*$pi/180" | bc -l)
    thetarad=$(echo "$theta*$pi/180" | bc -l)
    parafile_old="PARAMETER TEMPLATE NAME"

    width=30.
    height=15.
    resultfilename="resolution_Phi${phi}_Theta${theta}.txt" 
    resultfile="PATH TO SAVE RESULTS/$resultfilename"

    if [ ! -f "$resultfile" ];then
    
	    intermed=$(echo "scale=100; c($phirad)*c($thetarad)" | bc -l )
	    newlength=$(echo "scale=10; 0.1/$intermed" | bc -l)
	    zdist=$(echo "scale=10; (-1)*s($phirad)*$width/2+s($thetarad)*c($phirad)*$height/2" | bc -l)
	    plane3=117.7    
	    plane4=172.7

	    dist1=$(echo "$plane3-($zdist)" | bc -l)
	    dist2=$(echo "$plane4+($zdist)" | bc -l)
	    
	    dist3=$(echo "$plane3+($zdist)" | bc -l)
	    dist4=$(echo "$plane4-($zdist)" | bc -l)
	    echo "newlength $newlength, zdist $zdist, dist1 $dist1, dist2 $dist2, dist3 $dist3, dist4 $dist4"
	    #resolution at origin
	    sed "s/0.050 0.050.*/0.050 0.050 0.050 $newlength 0.050 0.050 0.050/g" "$parafile_old" >> "paras.txt"
	    ./trackResolutionCalculation.exe < paras.txt >> results.txt
	    res="$(tail -n 1 results.txt)"
	    res="$(echo ${res//[^0-9.]/})"
	    stripoff="."
	    res="${res%$stripoff}"
	    echo "$res" >> "$resultfile"
	    rm results.txt

	    #resolution at closest corner
	    sed "s/19.8 19.8.*/19.8 19.8 $dist1 $dist2 19.8 19.8/g" "paras.txt" >> "paras2.txt"
	    ./trackResolutionCalculation.exe < paras2.txt >> results.txt
	    res="$(tail -n 1 results.txt)"
	    res="$(echo ${res//[^0-9.]/})"
	    stripoff="."
	    res="${res%$stripoff}"
	    echo "$res" >> "$resultfile"
	    rm results.txt

	    #resolution at furthest corner
	    sed "s/19.8 19.8.*/19.8 19.8 $dist3 $dist4 19.8 19.8/g" "paras2.txt" >> "paras3.txt"
	    ./trackResolutionCalculation.exe < paras3.txt >> results.txt
	    res="$(tail -n 1 results.txt)"
	    res="$(echo ${res//[^0-9.]/})"
	    stripoff="."
	    res="${res%$stripoff}"
	    echo "$res" >> "$resultfile"
	    rm results.txt

	    rm paras.txt
	    rm paras2.txt
	    rm paras3.txt

    fi
}

#!/bin/bash
#Files
file="PATH TO LOGBOOK"
linestoskip=2
{ for ((i=$linestoskip;i--;)); do
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
    echo "$phi $theta"
    trackResolution $phi $theta
done } < "$file"
