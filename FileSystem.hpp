#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP
#include <iostream>
#include <exception>
#include <cstdint>
#include <string>
#include <math.h>
#include "disk.hpp"
#include "OFT.hpp"


struct Descriptor
{
    int fsize;
    int b1;
    int b2;
    int b3;

    Descriptor()
    {
        fsize = -1;
        b1 = -1;
        b2 = -1;
        b3 = -1;
    }
};

struct DirEntry
{
    int sname;
    int desci;
};

class FileSystem 
{
    public:
        FileSystem();
        ~FileSystem();
        int create(char* fname);
        void destroy(char* fname);
        int open(char* fname);
        int close(int i);
        int read(int i, int m, int n);
        int write(int i, int m, int n;
        int seek(int onum, int position);
        std::string directory();
        void init();
        std::string read_memory(int m, int n)
        int write_memory(int m, char* s)
        
        int assign_dir(int dnum, char* fname)
        void remove_dir()
        int next_des(int dnum)
        int get_block()
        int search_dir(char* fname)
        
    private: 
        char I[512];
        char O[512];
        char M[512];
        OFT* oft;
        Disk* disk;
        long long bitmap;
        Descriptor fd[192];  //max file descriptors is 192
}