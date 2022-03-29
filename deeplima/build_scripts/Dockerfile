FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt -y upgrade && apt -y dist-upgrade
RUN apt update && apt install -y git vim unzip wget build-essential cmake libboost-all-dev libprotobuf-dev libeigen3-dev libicu-dev

#RUN wget https://lindat.mff.cuni.cz/repository/xmlui/bitstream/handle/11234/1-3687/ud-treebanks-v2.8.tgz
#RUN tar -xavf ud-treebanks-v2.8.tgz

RUN git clone https://github.com/aymara/lima && cd lima && git checkout deeplima-dev
RUN cd lima && git submodule init && git submodule update

RUN cd /lima/extern && ./download_libtorch.sh
RUN cd /lima/deeplima && ./build_scripts/build_all.sh ../extern/libtorch/
RUN mkdir -p /deeplima/ && cd /lima/deeplima/build/Release && cmake --build . --target install
RUN ldconfig
