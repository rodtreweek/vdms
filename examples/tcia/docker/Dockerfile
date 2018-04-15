# Pull base image.
FROM ubuntu:16.04

RUN apt-get -qq update
RUN apt-get -qq install -y python python-dev python-pip python-virtualenv
RUN apt-get -qq install -y bzip2
RUN apt-get install --no-install-recommends -y libjsoncpp-dev build-essential cmake git pkg-config libprotobuf-dev libleveldb-dev libsnappy-dev libhdf5-serial-dev protobuf-compiler libatlas-base-dev libboost-all-dev libgflags-dev libgoogle-glog-dev liblmdb-dev libgflags-dev libgoogle-glog-dev liblmdb-dev
RUN apt-get install -y scons libjsoncpp-dev flex javacc libbison-dev openjdk-8-jdk
RUN apt-get install -y autoconf automake libtool curl make g++ unzip
RUN apt-get install -y cmake wget git zlib1g-dev libbz2-dev libssl-dev liblz4-dev mpich
RUN apt-get install -y libjpeg8-dev libtiff5-dev libjasper-dev libpng12-dev libgtk-3-dev
RUN apt-get install -y zlib1g-dev
RUN apt-get install -y libbz2-dev
RUN apt-get install -y libssl-dev
RUN apt-get install -y liblz4-dev
RUN apt-get install -y autoconf automake libtool curl make g++ unzip
RUN apt-get install -y libopenmpi-dev
RUN apt-get install -y libgtest-dev
RUN apt-get install -y ed
RUN apt-get install -y libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
RUN apt-get install -y libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
RUN pip install scons numpy
RUN apt-get remove -y libprotobuf-dev protobuf-compiler
RUN cd / && git clone https://github.com/google/protobuf.git
RUN cd protobuf && git checkout v3.3.0 && ./autogen.sh && ./configure --prefix=/usr/local && make -j $(cat /proc/cpuinfo | wc -l) && make install && ldconfig
RUN rm -rf /protobuf
RUN cd / && git clone https://github.com/Blosc/c-blosc.git
RUN cd c-blosc && mkdir build && cd build && cmake -DCMAKE_INSTALL_PREFIX='/usr' .. && cmake --build . && ctest && cmake --build . --target install
RUN rm -rf /c-blosc
RUN cd / && wget https://github.com/facebook/zstd/archive/v1.1.0.tar.gz && tar xf v1.1.0.tar.gz
RUN cd zstd-1.1.0 && make install PREFIX='/usr'
RUN cd /usr/src/gtest && cmake . && make && mv libgtest* /usr/lib/
RUN cd / && wget https://github.com/TileDB-Inc/TileDB/archive/0.6.1.tar.gz && tar xf 0.6.1.tar.gz
RUN cd TileDB-0.6.1 && mkdir build && cd build && cmake .. && make -j $(cat /proc/cpuinfo | wc -l) && make install
RUN rm -rf /TileDB-0.6.1

RUN mkdir /opencv && cd /opencv && wget -O opencv.zip https://github.com/opencv/opencv/archive/3.3.0.zip
RUN cd /opencv && unzip opencv.zip && cd opencv-3.3.0 && mkdir build && cd build && cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. && make -j $(cat /proc/cpuinfo | wc -l) && make install && rm -rf /opencv

# VCL install
RUN cd / && wget https://github.intel.com/labs-csr/vcl/archive/v0.1.0.tar.gz && tar xf v0.1.0.tar.gz
RUN mv vcl-0.1.0 vcl && cd vcl && scons -j16
RUN rm -rf v0.1.0.tar.gz

# PMGD install
RUN cd / && wget https://github.intel.com/labs-csr/pmgd/archive/v0.1.0.tar.gz && tar xf v0.1.0.tar.gz
RUN mv pmgd-0.1.0 jarvis && cd jarvis && make
RUN rm -rf v0.1.0.tar.gz

# Athena install
RUN cd / && wget https://github.intel.com/labs-csr/athena/archive/v0.2.0.tar.gz && tar xf v0.2.0.tar.gz
RUN mv athena-0.2.0 athena && cd athena && mkdir db && scons -j16 INTEL_PATH=../
RUN rm -rf v0.2.0.tar.gz

COPY data.tar.bz2 /
RUN bzcat /data.tar.bz2 | tar -C / -xf - && rm data.tar.bz2

RUN rm -rf /var/lib/apt/lists/* /root/.cache

RUN mv /athena_for_docker /data
RUN chmod 700 /data/hls/clean.sh
RUN echo '#!/bin/bash' > /start.sh
RUN echo 'export LD_LIBRARY_PATH=/jarvis/lib:$(find /usr/local/lib/ / -type f -name "*.so" | xargs dirname | sort | uniq | tr "\n" ":")' >> /start.sh

RUN  echo 'cd /data/hls/' >> /start.sh
RUN  echo './clean.sh' >> /start.sh
RUN chmod 755 /start.sh
ENTRYPOINT "/start.sh"

