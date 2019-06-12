FROM ubuntu:16.04

ARG BINTRAYKEY
ARG JOB_NUMBER
ARG LIMA_DISABLE_FSW_TESTING
ARG LIMA_DISABLE_CPACK_DEBIAN_PACKAGE_SHLIBDEPS
ARG NLTK_PTB_DP_FILE

# Setup
RUN echo "Acquire::http::Pipeline-Depth 0;" > /etc/apt/apt.conf.d/99fixbadproxy && echo "Acquire::http::No-Cache true;" >> /etc/apt/apt.conf.d/99fixbadproxy && echo "Acquire::BrokenProxy    true;" >> /etc/apt/apt.conf.d/99fixbadproxy

RUN apt-get clean && rm -rf /var/lib/apt/lists/* && apt-get update -o Acquire::CompressionTypes::Order::=gz && apt-get install -y lsb-release locales apt-utils git gcc g++ make cmake cmake-data curl python-nltk gawk wget python3 qt5-default libqt5xmlpatterns5 libqt5qml5 libqt5declarative5 libqt5quick5 libqt5quicktest5 qtdeclarative5-models-plugin qml-module-qtquick-extras qml-module-qtquick-controls qml-module-qtquick-layouts qml-module-qtquick2 libenchant1c2a libboost-date-time1.58.0 libboost-filesystem1.58.0 libboost-graph1.58.0 libboost-program-options1.58.0 libboost-test1.58.0 libboost-thread1.58.0 libboost-chrono1.58.0 libboost-atomic1.58.0  qtbase5-dev-tools libqt5xmlpatterns5-dev qtdeclarative5-dev     qml-module-qtquick2 libenchant-dev libboost-all-dev nodejs npm  libtre-dev -qq
RUN locale-gen en_US.UTF-8

ENV LANG=en_US.UTF-8 LANGUAGE=en_US:en LC_ALL=en_US.UTF-8

RUN npm install -g json
RUN ln -s /usr/bin/nodejs /usr/bin/node


RUN sed -ie "s|DEFAULT_URL = 'http://nltk.googlecode.com/svn/trunk/nltk_data/index.xml'|DEFAULT_URL = 'http://nltk.github.com/nltk_data/'|" /usr/lib/python2.7/*/nltk/downloader.py && python -m nltk.downloader -d nltk_data dependency_treebank && cat nltk_data/corpora/dependency_treebank/wsj_*.dp | grep -v "^$" > nltk_data/corpora/dependency_treebank/nltk-ptb.dp

RUN wget http://osmot.cs.cornell.edu/svm_light/current/svm_light.tar.gz
WORKDIR /svm_light
RUN tar xzf ../svm_light.tar.gz && make && cp svm_classify svm_learn /usr/bin

WORKDIR /
RUN wget https://dl.bintray.com/kleag/SVMTool-1.3.1/SVMTool-1.3.1.tgz
RUN tar xzf SVMTool-1.3.1.tgz
WORKDIR /SVMTool-1.3.1
RUN perl Makefile.PL && make && make install

WORKDIR /
RUN wget https://bintray.com/artifact/download/kleag/ubuntu-16.04/$(curl https://api.bintray.com/packages/kleag/ubuntu-16.04/qhttpserver/versions/$(curl https://api.bintray.com/packages/kleag/ubuntu-16.04/qhttpserver | json -a latest_version)/files | json 0.path) && dpkg -i $(curl https://api.bintray.com/packages/kleag/ubuntu-16.04/qhttpserver/versions/$(curl https://api.bintray.com/packages/kleag/ubuntu-16.04/qhttpserver | json -a latest_version)/files | json 0.name) && wget https://bintray.com/artifact/download/kleag/ubuntu-16.04/$(echo $(curl https://api.bintray.com/packages/kleag/ubuntu-16.04/svmtool-cpp/versions/$(curl https://api.bintray.com/packages/kleag/ubuntu-16.04/svmtool-cpp | json -a latest_version)/files | json 0.path)  | sed -e 's/\+/%2B/g') && dpkg -i $(curl https://api.bintray.com/packages/kleag/ubuntu-16.04/svmtool-cpp/versions/$(curl https://api.bintray.com/packages/kleag/ubuntu-16.04/svmtool-cpp | json -a latest_version)/files | json 0.name)

RUN mkdir -p /src/
RUN git clone https://github.com/aymara/lima /src/lima
WORKDIR /src/lima
ARG CACHEBUST=1
RUN git pull
RUN echo "$(git show -s --format=%cI HEAD | sed -e 's/[^0-9]//g')-$(git rev-parse --short HEAD)" > release

RUN mkdir -p /src/lima/build
WORKDIR /src/lima/build

ENV PERL5LIB=/SVMTool-1.3.1/lib:$PERL5LIB PATH=/SVMTool-1.3.1/bin:/usr/share/apps/lima/scripts:/usr/bin:$PATH NLTK_PTB_DP_FILE=/nltk_data/corpora/dependency_treebank/nltk-ptb.dp LIMA_DISABLE_FSW_TESTING=true LIMA_DISABLE_CPACK_DEBIAN_PACKAGE_SHLIBDEPS=true LIMA_DIST=/usr LIMA_CONF=/usr/share/config/lima LIMA_RESOURCES=/usr/share/apps/lima/resources

# Build
RUN cmake -DLIMA_RESOURCES:STRING=build -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE:STRING=Debug  -DLIMA_VERSION_RELEASE:STRING="$(cat /src/lima/release)" -DSHORTEN_POR_CORPUS_FOR_SVMLEARN:BOOL=ON .. && make -j2 && install -D -t /usr/share/apps/lima/packages /src/lima/build/*/src/*-build/*.deb

WORKDIR /usr/share/apps/lima/tests
RUN /bin/bash -c "set -o pipefail && tva --language=eng test-eng.*.xml 2>&1 | tee tva-eng.log"
RUN /bin/bash -c "set -o pipefail && tva --language=fre test-fre.default.xml test-fre.disambiguated.xml test-fre.hyphen.xml test-fre.idiom.xml test-fre.sa.xml test-fre.se.xml test-fre.simpleword.xml test-fre.tokenizer.xml 2>&1 | tee tva-fre.log"
WORKDIR /usr/share/apps/lima/tests/xmlreader
RUN /bin/bash -c "set -o pipefail && tvx --language=eng --language=fre test-fre.xmlreader.xml 2>&1 | tee tvx-fre.log"

RUN curl -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limacommon-2.1.$(cat /src/lima/release)-runtime.deb  https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limacommon-2.1.$(cat /src/lima/release)-runtime.deb && curl -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limacommon-2.1.$(cat /src/lima/release)-devel.deb  https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limacommon-2.1.$(cat /src/lima/release)-devel.deb &&  curl -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limalinguisticprocessing-2.1.$(cat /src/lima/release)-runtime.deb https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limalinguisticprocessing-2.1.$(cat /src/lima/release)-runtime.deb && curl -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limalinguisticprocessing-2.1.$(cat /src/lima/release)-devel.deb https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limalinguisticprocessing-2.1.$(cat /src/lima/release)-devel.deb && curl  -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limalinguisticdata-2.1.$(cat /src/lima/release)-common.deb  https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limalinguisticdata-2.1.$(cat /src/lima/release)-common.deb && curl  -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limalinguisticdata-2.1.$(cat /src/lima/release)-eng.deb  https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limalinguisticdata-2.1.$(cat /src/lima/release)-eng.deb && curl  -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limalinguisticdata-2.1.$(cat /src/lima/release)-fre.deb  https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limalinguisticdata-2.1.$(cat /src/lima/release)-fre.deb && curl  -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limalinguisticdata-2.1.$(cat /src/lima/release)-por.deb  https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limalinguisticdata-2.1.$(cat /src/lima/release)-por.deb && curl -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limagui-2.1.$(cat /src/lima/release)-runtime.deb  https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limagui-2.1.$(cat /src/lima/release)-runtime.deb && curl -ukleag:$BINTRAYKEY -T /usr/share/apps/lima/packages/limagui-2.1.$(cat /src/lima/release)-devel.deb  https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/$JOB_NUMBER/limagui-2.1.$(cat /src/lima/release)-devel.deb &&  curl -XPOST -ukleag:$BINTRAYKEY https://api.bintray.com/content/kleag/ubuntu-16.04/lima/$JOB_NUMBER/publish && curl -XGET https://bintray.com/kleag/ubuntu-16.04/lima/$JOB_NUMBER


