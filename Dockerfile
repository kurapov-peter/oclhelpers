FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    make \
    git \
    libboost-all-dev \
    opencl-headers \
    ocl-icd-opencl-dev \
    --

