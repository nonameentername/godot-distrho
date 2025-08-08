FROM ghcr.io/nonameentername/godot-osxcross AS base-image

RUN apt update
RUN apt install -y libboost-all-dev libssl-dev
RUN apt install -y libasound2-dev libcairo2-dev libdbus-1-dev libgl1-mesa-dev liblo-dev libpulse-dev libsdl2-dev libx11-dev libxcursor-dev libxext-dev libxrandr-dev xvfb
RUN apt install -y wget

RUN mkdir /.local 
RUN chmod 777 /.local

RUN mkdir /.config 
RUN chmod 777 /.config

RUN mkdir /.cache 
RUN chmod 777 /.cache

#TODO: move to base image
RUN /osxcross/build_compiler_rt.sh

#TODO: move to base image
RUN wget https://github.com/godotengine/moltenvk-osxcross/releases/download/vulkan-sdk-1.3.283.0-2/MoltenVK-all.tar
RUN tar xvf MoltenVK-all.tar
RUN rm MoltenVK-all.tar

FROM ubuntu:24.04
COPY --from=base-image / /
