#include "disk.hpp"

Disk::Disk()
{

}

Disk::~Disk()
{

}

int Disk::read_block(int blocknum, char* I)
{
    try
    {
        for (int i = 0; i < BLOCKSIZE; i++)
        {
            I[i] = ldisk[blocknum][i];
        }
        return 0;
    }
    catch (exception e)
    {
        std::cout << "Disk::read_block error" << std::endl;
        return -1;
    }
}

int Disk::write_block(int blocknum, char* O)
{
    try
    {
        for (int i = 0; i < BLOCKSIZE; i++)
        {
            ldisk[blocknum][i] = O[i];
        }
        return 0;
    }
    catch (exception e)
    {
        std::cout << "Disk::write_block error" << std::endl;
        return -1;
    }
}

void Disk::clear()
{
    for(int b = 0; b < BLOCKNUM; b++)
    {
        for(int i = 0; i < BLOCKSIZE; i++)
        {
            ldisk[b][i] = '\0';
        }
    }
}