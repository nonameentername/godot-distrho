FROM ghcr.io/nonameentername/godot-ubuntu

RUN apt install -y libboost-all-dev libssl-dev
RUN apt install -y libasound2-dev libcairo2-dev libdbus-1-dev libgl1-mesa-dev liblo-dev libpulse-dev libsdl2-dev libx11-dev libxcursor-dev libxext-dev libxrandr-dev xvfb

RUN mkdir /.local 
RUN chmod 777 /.local

RUN mkdir /.config 
RUN chmod 777 /.config

RUN mkdir /.cache 
RUN chmod 777 /.cache
