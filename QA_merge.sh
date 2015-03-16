#! /bin/bash

outputFolder=$3

if [ -f $outputFolder/merged_1.pdf ]; then
  rm $outputFolder/merged_*.pdf
fi

if [ -f $outputFolder/merged_info.txt ]; then
  rm $outputFolder/merged_info.txt
fi
index=1
mergedFileName=merged_1.pdf
for ((i=$1; i<=$2; i++))
do
  echo Run: $i
  if [ -d `printf $outputFolder/run"%06d"/Plots/important/ $i` ]; then
    folder=`printf $outputFolder/run"%06d"/Plots/important/ $i`
  elif [ -d `printf $outputFolder/run"%06d"/Plots/ $i` ]; then
    folder=`printf $outputFolder/run"%06d"/Plots/ $i`
  else
    echo Run $i has no QA plots saved >> $outputFolder/merged_info.txt
    continue
  fi
  if ! [ -f $outputFolder/$mergedFileName ]; then
    echo Run $i starts on page 1 >> $outputFolder/merged_info.txt
  else
    page=`pdfinfo $outputFolder/$mergedFileName | grep "Pages: " | sed -n -e 's/Pages: //p'`
    if (( $page>150 )); then
      index=$((index+1))
      mergedFileName=merged_$index.pdf
      echo Moving to $mergedFileName >> $outputFolder/merged_info.txt
      page=0
    fi
    echo Run $i starts on page $((page+1)) >>  $outputFolder/merged_info.txt
 fi
  if [ -f $outputFolder/$mergedFileName ]; then
    pdfjam $outputFolder/$mergedFileName $folder/*.pdf -o $outputFolder/$mergedFileName > /dev/null 2>&1
  else
    pdfjam  $folder/*.pdf -o $outputFolder/$mergedFileName > /dev/null 2>&1
  fi
done
echo Merged pdf written to $outputFolder/merged_N.pdf 
echo Info on which page belongs to which run written to $outputFolder/merged_info.txt
#evince $outputFolder/merged.pdf & > /dev/null 2>&1
