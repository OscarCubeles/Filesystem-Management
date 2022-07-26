# File System Management 
C Program program capable of reading and writing data on volumes from two different file systems, [EXT2](Docs/EXT2%20Documentation.pdf) and [FAT16](Docs/FAT16%20Documentation.pdf). 

## Table of Contents
1. [General Info](#general-info)
2. [Technologies](#technologies)
3. [Installation](#installation)
4. [Code Executeion](#code-execution)

## General Info
***
This university project consists of developing a program to perform different functions on FAT16 and EXT2 file system volumes. 
Concretely, 3 actions can be done. The first one is to extract metadata from the volumes, the second one is to find a file inside the volume and its data and the last one is to delete a file inside the volume. 
Two sample volumes are provided in this repository, Ext2 and FAT16.bin volume files.

## Technologies
***
A list of technologies used within the project:
* [C Programming](https://docs.microsoft.com/en-us/cpp/c-language/?view=msvc-170)
* File System Management

## Installation
***
```
$ git clone https://github.com/OscarCubeles/Filesystem-Management.git
$ make
```

## Code Execution
***
```
$ ./Shooter /info <volume_name>             #Shows metadata about <volume_name>
$ ./Shooter /find <volume_name> <file_name> #Shows metadata about <file_name> if exists in <volume_name>
$ ./Shooter /info <volume_name> <file_name> #Deletes <file_name> if exists in <volume_name>
```
