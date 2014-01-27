##!bin/sh

source /home/leffe/Lima/leffe.env
cd /home/leffe/Lima/Sources/linguisticData/syntacticAnalysis
make clean && make fre && make install-fre
