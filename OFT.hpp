#ifndef OFT_HPP
#define OFT_HPP
#include <stdlib.h>

#define BUFFSIZE 512
#define OFTSIZE 4

struct OFTentry 
{
    char buff[BUFFSIZE];
    int position;
    int desci;
    int fsize;
};

class OFT
{
    public:
        OFT();
        ~OFT();
        void init();
        void loadfile(int onum, int ndesci, int nfsize);
        void removefile(int onum);
        int searchentry();
        int readbyte(int onum, char* b);
        int writebyte(int onum, char* b);
        int seek(int onum, int newpos);
        char* get_buf(int onum);
        int get_pos(int onum);
        int get_fsize(int onum);
        int get_desci(int onum);
        int checkpos(int onum);


    private:
        OFTentry oft[OFTSIZE];
        
}