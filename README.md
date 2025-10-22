# UntitledIBusHandwriting
[![CI](https://github.com/MadLadSquad/UntitledIBusHandwriting/actions/workflows/ci.yml/badge.svg)](https://github.com/MadLadSquad/UntitledIBusHandwriting/actions/workflows/ci.yml)
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://lbesson.mit-license.org/)
[![Discord](https://img.shields.io/discord/717037253292982315.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/4wgH8ZE)

![image](https://github.com/MadLadSquad/UntitledIBusHandwriting/assets/40400590/78654353-07c2-4b0b-bf6c-c39daff37ab6)

A multilingual handwriting IM for IBus.

This project uses the [hanzi-lookup](https://github.com/gugray/hanzi_lookup/) library which handles character recognition.

Features:
1. Fed additional data from [hanzi-writer-data-youyin](https://github.com/MadLadSquad/hanzi-writer-data-youyin/) for larger script support
1. Fast and accurate character recognition, even when you make mistakes

## Showcase
![image](https://github.com/MadLadSquad/UntitledIBusHandwriting/assets/40400590/57042463-cd44-4239-ab4c-26450c2a5c01)

## Install
Other requirements:
1. A recent version of the rust programming language and cargo
1. Developer packages for IBus may be required if your distribution does not ship them as part of the main IBus package

This is a standard [UntitledImGuiFramework](https://github.com/MadLadSquad/UntitledImGuiFramework) project. Building instructions can be found 
[here](https://github.com/MadLadSquad/UntitledImGuiFramework/wiki/Collaborating-on-a-project-with-the-framework#setting-up-another-persons-project).

Before compiling and installing, make sure that you have compiled beforehand `hanzi_lookup`. This can be done like this:
```sh
cd hanzi_lookup || exit
cargo build --release
cd ..
```
