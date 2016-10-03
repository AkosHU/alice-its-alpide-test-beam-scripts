#!/bin/bash

for i in $(seq 1 1000)
do
    file=parameters_7_${i}.txt
    cp parameters_7_tmp.txt ${file}

    res=$(bc -l <<< "${i}/100000")

    res_um=$(bc -l <<< "${i}/100")

    sed -i -e '' "s/resTmp/${res}/g" ${file}

    out_residual=$(./trackResolutionCalculation.exe < ${file} 2>&1 )
    out_residual=$(echo ${out_residual} | grep "corresponding residual")
    out_residual=$(echo ${out_residual} | cut -d' ' -f115 )
    out_resolution=$(./trackResolutionCalculation.exe < ${file} 2>&1 )
    out_resolution=$(echo ${out_resolution} | grep "resolution is " )
    out_resolution=$(echo ${out_resolution} | cut -d' ' -f106)

    rm ${file}
    printf "%f\t%f\t%f\n" ${res_um} ${out_residual} ${out_resolution}
done
