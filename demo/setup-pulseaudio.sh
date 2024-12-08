#!/bin/bash

# Set PulseAudio environment variables
export XDG_RUNTIME_DIR=/run/user/1000
export PULSE_SERVER=unix:/run/user/1000/pulse/native

# Ensure the runtime directory exists and has correct permissions
mkdir -p $XDG_RUNTIME_DIR
chown $(id -u):$(id -g) $XDG_RUNTIME_DIR
chmod 700 $XDG_RUNTIME_DIR

# Start PulseAudio if not running
pulseaudio --check || pulseaudio --start --log-level=debug
