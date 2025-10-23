FROM jdesmedt/crosscompile-i686-elf 

RUN apt-get update 
RUN apt-get upgrade -y
RUN apt-get install -y nasm
RUN apt-get install -y xorriso
RUN apt-get install -y grub-pc-bin
RUN apt-get install -y grub-common
RUN apt-get install -y dosfstools
RUN apt-get install -y mtools
RUN apt-get install -y mingw-w64
RUN apt-get install -y bear

VOLUME /root/env
WORKDIR /root/env
