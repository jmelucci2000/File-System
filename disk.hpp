#ifndef DISK_H
#define DISK_H
#include <iostream>
#include <exception>

#define BLOCKNUM 64
#define BLOCKSIZE 512


class Disk 
{
    public:
        Disk();
        ~Disk();
        int read_block(int blocknum, char* O);
        int write_block(int blocknum, char* I);  
        void clear();
        

    private:
        char ldisk[BLOCKNUM][BLOCKSIZE];  


};   
