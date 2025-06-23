# yumulk

This repository contains an example Metin2 source with client, binary, and server components.

## Offline Shop

A minimal offline shop system has been added. Server, client and binary contain placeholder structures and packets for offline shop support. The system is not fully functional but illustrates how such a feature could be integrated.

## Porting to Unreal Engine

Porting the project to Unreal Engine requires rewriting most engine-specific code. Suggested steps:

1. Create an Unreal project and set up basic character and world assets.
2. Convert server-side gameplay logic into Unreal C++ modules or Blueprints.
3. Reimplement networking using Unreal's replication system.
4. Replace Python UI scripts with Unreal UI widgets (UMG).
5. Gradually migrate features while maintaining data structures compatible with existing packets.

These steps provide a broad overview and will require significant development effort.
