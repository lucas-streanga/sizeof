# sizeof
sizeof is a command line program for determining the size of files and directories in bytes and number of files. It is written in C++.

# Usage
Simply pass in a file or directory as the first command line argument: ```sizeof my_file```. The ```-v``` option can be 
passed as the last argument for verbose output, and the ```-vv``` flag for super verbose output. 

## Output
sizeof will output the size in a human readable unit (bytes, KiB, MiB, GiB) and the number of files if the input is a directory.
Any file that is skipped because the size it not determinable will be printed.
Sample run:
```
sizeof ~
SKIPPED: size is not determinable: "/home/lucas/.steam/steam.pipe"
/home/lucas is 18.00 gigabytes with 136223 files.
```

## Compilation
Run ```make``` to compile to program. ```make install``` can be used to install the program to /usr/local/bin 
so that it may be run system wide. The install path may be changed by editing the ```INSTALL_PATH``` variable,
for example: ```make install INSTALL_PATH=~/programs```. This program only relies on the C++17 standard libraries,
and as such your version of GCC must support these features to compile the program.

## Bugs
Attempting sizeof on the root directory (/) will cause incorrect output. The program will not terminate, but
it is not able to properly skip through the files like intended, thus leading to incorrect output. 
