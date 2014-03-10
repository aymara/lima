##!bin/sh

source /home/leffe/Lima/leffe.env
cd /home/leffe/Lima/Sources/linguisticData/disambiguisationMatrices/fre
make clean && make distclean
cd ..
./build.sh fre && ./deploy.sh fre

