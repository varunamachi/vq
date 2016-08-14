#/bin/bash

exec &> logfile.txt

#VQ_ROOT=/mnt/Store/Kode/github/quartz/
VQ_ROOT=`readlink -f ..`
echo "Using root $VQ_ROOT"
export VQ_ROOT
if [[ -e ~/bin/qtcreator ]] 
then
    echo "Launching ~/bin/qtcreator"
    ~/bin/qtcreator &
elif [[ -e ~/Bin/qtcreator ]] 
then
    echo "Launching ~/Bin/qtcreator"
    ~/Bin/qtcreator &
else
    echo "Launching `which qtcreator`"
    qtcreator &
fi
    





