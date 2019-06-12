FROM ubuntu:14.04

RUN apt-get clean && apt-get update && apt-get install -y locales
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8
RUN locale

ARG BINTRAYKEY
ARG JOB_NUMBER

# Setup
RUN apt-get update -y -qq
RUN apt-get install -y lsb-release apt-utils software-properties-common python-software-properties python3-software-properties ninja-build git gcc g++ autotools-dev dh-autoreconf curl python-nltk gawk wget qt5-default qtbase5-dev-tools libqt5xmlpatterns5-dev libqt5qml5 qtdeclarative5-dev libboost-all-dev libenchant-dev libtre-dev -qq

# Install cmake 3.4 needed by LIMA for ExternalProject_Add_Step options to be
# used with Ninja
RUN wget http://www.cmake.org/files/v3.4/cmake-3.4.1.tar.gz && tar xzf cmake-3.4.1.tar.gz && cd cmake-3.4.1/ && ./configure && make && make install && update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 1 --force

RUN curl -sL https://deb.nodesource.com/setup | bash -
RUN apt-get install -y nodejs
RUN npm install -g json


RUN sed -ie "s|DEFAULT_URL = 'http://nltk.googlecode.com/svn/trunk/nltk_data/index.xml'|DEFAULT_URL = 'http://nltk.github.com/nltk_data/'|" /usr/lib/python2.7/*/nltk/downloader.py
RUN python -m nltk.downloader -d nltk_data dependency_treebank
RUN cat nltk_data/corpora/dependency_treebank/wsj_*.dp | grep -v "^$" > nltk_data/corpora/dependency_treebank/nltk-ptb.dp

RUN wget http://osmot.cs.cornell.edu/svm_light/current/svm_light.tar.gz
WORKDIR /svm_light
RUN tar xzf ../svm_light.tar.gz
RUN make
RUN cp svm_classify svm_learn /usr/bin

WORKDIR /
RUN TEMP_DEB="$(mktemp)" && wget -O "$TEMP_DEB" $(curl -s https://api.github.com/repos/aymara/SVMTool/releases/latest | grep browser_download_url | cut -d '"' -f 4) && tar xzf "$TEMP_DEB"
WORKDIR /SVMTool-1.3.1
RUN perl Makefile.PL && make && make install

RUN TEMP_DEB="$(mktemp)" && wget -O "$TEMP_DEB" $(curl -s https://api.github.com/repos/aymara/qhttpserver/releases/latest | grep browser_download_url | grep ubuntu16 | cut -d '"' -f 4) && dpkg -i "$TEMP_DEB"


RUN TEMP_DEB="$(mktemp)" && wget -O "$TEMP_DEB" $(curl -s https://api.github.com/repos/aymara/svmtool-cpp/releases/latest | grep browser_download_url | grep ubuntu16 | cut -d '"' -f 4) && dpkg -i "$TEMP_DEB"

RUN mkdir -p /src/
RUN git clone https://github.com/aymara/lima /src/lima
WORKDIR /src/lima
ARG CACHEBUST=1
RUN git pull
RUN echo "$(git show -s --format=%cI HEAD | sed -e 's/[^0-9]//g')-$(git rev-parse --short HEAD)" > release

RUN mkdir -p /src/lima/build
WORKDIR /src/lima/build

ENV PERL5LIB /SVMTool-1.3.1/lib:$PERL5LIB
ENV PATH /SVMTool-1.3.1/bin:/usr/share/apps/lima/scripts:/usr/bin:$PATH
ENV NLTK_PTB_DP_FILE /nltk_data/corpora/dependency_treebank/nltk-ptb.dp
ENV LIMA_DISABLE_FSW_TESTING true
ENV LIMA_DISABLE_CPACK_DEBIAN_PACKAGE_SHLIBDEPS true
ENV LIMA_DIST /usr
ENV LIMA_CONF /usr/share/config/lima
ENV LIMA_RESOURCES /usr/share/apps/lima/resources

# Build
RUN cmake -G Ninja -DLIMA_RESOURCES:STRING=build -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE:STRING=Release -DLIMA_VERSION_RELEASE:STRING="$(cat /src/lima/release)" -DSHORTEN_POR_CORPUS_FOR_SVMLEARN:BOOL=ON  ..
RUN ninja
RUN install -D -t /usr/share/apps/lima/packages /src/lima/build/*/src/*-build/*.deb

WORKDIR /usr/share/apps/lima/tests
RUN /bin/bash -c "set -o pipefail && tva --language=eng test-eng.*.xml 2>&1 | tee tva-eng.log"
RUN /bin/bash -c "set -o pipefail && tva --language=fre test-fre.default.xml test-fre.disambiguated.xml test-fre.hyphen.xml test-fre.idiom.xml test-fre.sa.xml test-fre.se.xml test-fre.simpleword.xml test-fre.tokenizer.xml 2>&1 | tee tva-fre.log"
WORKDIR /usr/share/apps/lima/tests/xmlreader
RUN /bin/bash -c "set -o pipefail && tvx --language=eng --language=fre test-fre.xmlreader.xml 2>&1 | tee tvx-fre.log"

# install github-release to be able to deploy packages
RUN wget https://github.com/aktau/github-release/releases/download/v0.7.2/linux-amd64-github-release.tar.bz2 && tar xjf linux-amd64-github-release.tar.bz2 && cp bin/linux/amd64/github-release /usr/bin
