# UntitledIBusHandwriting
[![CI](https://github.com/MadLadSquad/UntitledIBusHandwriting/actions/workflows/ci.yml/badge.svg)](https://github.com/MadLadSquad/UntitledIBusHandwriting/actions/workflows/ci.yml)
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://lbesson.mit-license.org/)
[![trello](https://img.shields.io/badge/Trello-UDE-blue])](https://trello.com/b/HmfuRY2K/untitleddesktop)
[![Discord](https://img.shields.io/discord/717037253292982315.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/4wgH8ZE)

![image](https://github.com/MadLadSquad/UntitledIBusHandwriting/assets/40400590/78654353-07c2-4b0b-bf6c-c39daff37ab6)

A multilingual handwriting IM for IBus.

This project uses the [hanzi-lookup](https://github.com/gugray/hanzi_lookup/) library which handles character recognition.

Features:
1. Fed additional data from [hanzi-writer-data-youyin](https://github.com/MadLadSquad/hanzi-writer-data-youyin/) for larger script support
1. Fast and accurate character recognition, even when you make mistakes

## Showcase
![image](https://github.com/MadLadSquad/UntitledIBusHandwriting/assets/40400590/b15035d2-2687-40c2-a9c1-0305182a966f)

## Install
Run clone the source code with submodules and run `ci.sh` to install the application. Note that you might need the following dependencies(Ubuntu as an example):
```
sudo apt-get update && sudo apt-get install x11-xserver-utils libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev gcc make g++ libxmu-dev libxi-dev libgl-dev libglu1-mesa-dev libvulkan1 mesa-vulkan-drivers vulkan-tools libvulkan-dev libasound2-dev libflac-dev libogg-dev libtool libvorbis-dev libopus-dev libsndfile1-dev libglew-dev libssl-dev zlib1g zlib1g-dev
```
\+ a modern installation of the Rust programming language.
