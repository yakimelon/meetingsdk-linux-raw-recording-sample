# Use the official Ubuntu image as the base image
FROM ubuntu:22.04

RUN echo "root:admin" | chpasswd

# Install necessary dependencies
RUN apt-get update && \
    apt-get install -y build-essential cmake

RUN apt-get install -y pkgconf


RUN apt-get update && apt-get install -y --no-install-recommends --no-install-suggests \
    libx11-xcb1 \
    libxcb-xfixes0 \
    libxcb-shape0 \
    libxcb-shm0 \
    libxcb-randr0 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-xtest0 \
    libdbus-1-3 \
    libglib2.0-0 \
    libgbm1 \
    libxfixes3 \
    libgl1 \
    libdrm2 \
    libgssapi-krb5-2 \
    openssl \
    ca-certificates \
    pkg-config \
    libegl-mesa0 \
    libsdl2-dev \
    g++-multilib

# Install CURL related libs
RUN apt-get install -y libcurl4-openssl-dev

# Install ALSA
RUN apt-get install -y libasound2 libasound2-plugins alsa alsa-utils alsa-oss

# Install Pulseaudio
RUN apt-get install -y  pulseaudio pulseaudio-utils libpulse0

RUN apt-get upgrade -y

RUN apt-get install -f
RUN apt-get update --fix-missing

RUN apt-get install gdb -y

# TODO: Dockerだと apt install -y libopencv-dev のインストールで完了しなくなってしまうので、手動でインストールを行う
# RUN DEBIAN_FRONTEND=noninteractive apt-get update && \
#    apt-get install -y libopencv-dev && \
#    apt-get clean && rm -rf /var/lib/apt/lists/* \

# Set the working directory
WORKDIR /app

# Copy your application files to the container
COPY demo/ /app/demo/

# Execute additional commands
#RUN cd /app/demo && rm -rf bin && rm -rf build && cmake -B build && cd build && make

# Set the working directory to the source folder
WORKDIR /app/demo

# Make the run script executable
RUN chmod +x /app/demo/setup-pulseaudio.sh

# Ensure PulseAudio runtime directory exists
RUN mkdir -p /run/user/1000 && chmod 700 /run/user/1000

# Set runtime environment
ARG USER_UID=1000
ARG USER_GID=1000

# Create user and runtime directory
RUN groupadd -g ${USER_GID} pulseuser && \
    useradd -m -u ${USER_UID} -g ${USER_GID} -s /bin/bash pulseuser && \
    mkdir -p /run/user/${USER_UID} && \
    chown ${USER_UID}:${USER_GID} /run/user/${USER_UID} && \
    chmod 700 /run/user/${USER_UID}


ENV XDG_RUNTIME_DIR=/run/user/1000
USER pulseuser

# Set the working directory to the binary folder
WORKDIR /app/demo/bin


# Define a shell script to run multiple commands
#RUN echo '#!/bin/bash' > /app/demo/run.sh \
#    && echo '/app/demo/setup-pulseaudio.sh' >> /app/demo/run.sh \
#    && echo './meetingSDKDemo' >> /app/demo/run.sh

# Make the run script executable
#RUN chmod +x /app/demo/run.sh

# Specify the run script as the CMD
#CMD ["/app/demo/run.sh"]
#CMD ["/bin/bash"]
#CMD ["./meetingSDKDemo"]