FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y git wget vim pkg-config python3.6-dev python3-pip vim cmake protobuf-compiler libprotoc-dev llvm-9

RUN ln -s /usr/bin/llvm-config-9 /usr/bin/llvm-config
ARG DEBIAN_FRONTEND=noninteractive

RUN pip3 install -U pip
RUN pip3 install --default-timeout=100 lief numpy pandas matplotlib  torch cython==0.29.22
RUN pip3 install --default-timeout=100 pyarrow==0.16.0 simpletransformers==0.47.2 transformers==3.0.2  numba==0.52.0 nuitka==0.6.12.3 onnxruntime==1.4.0

RUN ln -sf /usr/bin/python3 /usr/bin/python
RUN sed -i '/shutil.rmtree(build_dir)/d' /usr/local/lib/python3.6/dist-packages/numba/pycc/cc.py

ADD src src
ADD test test
ADD build.sh build.sh
ADD exp exp
ADD main.c main.c
ADD CMakeLists.txt CMakeLists.txt
