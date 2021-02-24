#include "OFT.hpp"

OFT::OFT()
{
    
}

OFT::~OFT()
{

}

void OFT::init()
{
    for (int i = 0; i < OFTSIZE; i++)
    {
        oft[i].position = -1;
        oft[i].desci = -1;
        oft[i].fsize = -1;
        for (int j = 0; j < 512; j++)
        {
            oft[i].buff[j] = '\0';
        }
    }
}

void OFT::loadfile(int onum, int ndesci, int nfsize)
{
    oft[onum].position = 0;
    oft[onum].desci = ndesci;
    oft[onum].fsize = nfsize;
}

void OFT::removefile(int onum)
{
    oft[i].position = -1;
    oft[i].desci = -1;
    oft[i].fsize = -1;
    for (int j = 0; j < 512; j++)
    {
        oft[i].buff[j] = '\0';
    }
}

int OFT::searchentry()
{
    for (int i = 0; i < OFTSIZE; i++)
    {
        if (oft[i].position == -1)
        {
            return i;
        }
    }
    return -1;
}

void OFT::readbyte(int onum, char* b)  //must check if current position is valid before calling this function
{
    *b = oft[i].buff[oft[i].position % 512];
    oft[i].position++;
}

int OFT::writebyte(int onum, char* b)  //must check if current position is valid before calling this function
{
    oft[i].buff[oft[i].position % 512] = *b;
    oft[i].position++;
    if (oft[i].position == oft[i].fsize+1)
    {
        oft[i].fsize++;
    }
    return checkpos(onum);
}


// need to think about the case when newpos = 512 and fsize = 512  do I want read/write to get the next block? maybe yes
int OFT::seek(int onum, int newpos)  //check if newpos in incorrect in FileSystem::seek()
{
    oft[onum].position = newpos;
    if (newpos <= 512 && oft[onum].position > 512)
    {
        return 1;
    }
    else if (newpos <= 1024 && (oft[onum].position <= 512 || oft[onum].position > 1024))
    {
        return 2;
    }
    else if (newpos > 1024 && oft[onum].position <= 1024)
    {
        return 3;
    }
    return 0;
}

char* OFT::get_buf(int onum)
{
    return oft[onum].buff;
}

int OFT::get_pos(int onum)
{
    return oft[onum].position;
}

int OFT::get_desci(int onum)
{
    return oft[onum].desci;
}

int OFT::read_check(int onum)
{
    if (oft[i].position == oft[i].fsize)
    {
        return 4;
    }
    else if (oft[i].position == 512)
    {
        return 2; //get block 2 from buffer
    }
    else if (oft[i].position == 1024)
    {
        return 3;
    }
    else if (oft[i].position == 1536)
    {
        return 4;  
    }
    else
    {
        return 0;
    }
} 

int OFT::write_check(int onum)
{

    if (oft[i].position == 512)
    {
        return 2; //get block 2 from buffer
    }
    else if (oft[i].position == 1024)
    {
        return 3;
    }
    else if (oft[i].position == 1536)
    {
        return 4;  
    }
    else
    {
        return 0;
    }
} 

