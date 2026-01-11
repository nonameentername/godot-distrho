#FROM ghcr.io/nonameentername/godot-ubuntu AS base-image
FROM ubuntu:24.04 AS base-image

#TODO: use the previous godot-ubuntu base image
RUN apt update -y
RUN apt install -y build-essential
RUN apt install -y cmake
RUN apt install -y git
RUN apt install -y python3
RUN apt install -y python3-pip
RUN apt install -y zip
RUN apt install -y curl
RUN apt install -y unzip
RUN apt install -y pkg-config
RUN apt install -y autoconf
RUN apt install -y libtool
RUN apt install -y flex
RUN apt install -y bison
RUN apt install -y scons
RUN apt update -y

RUN apt install -y libboost-all-dev libssl-dev
RUN apt install -y libasound2-dev libcairo2-dev libdbus-1-dev libgl1-mesa-dev liblo-dev libpulse-dev libsdl2-dev libx11-dev libxcursor-dev libxext-dev libxrandr-dev xvfb

RUN mkdir /.local 
RUN chmod 777 /.local

RUN mkdir /.config 
RUN chmod 777 /.config

RUN mkdir /.cache 
RUN chmod 777 /.cache

#FROM ubuntu:20.04
#COPY --from=base-image / /
