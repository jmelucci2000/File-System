#include "FileSystem.hpp"

FileSystem::FileSystem()
{
    disk = new Disk();
    oft = new OFT();
    init();

}

FileSystem::~FileSystem()
{
    delete disk;
    delete oft;
}

void FileSystem::create(char* fname)
{
    int dnum;
    int bmnum;

    if (search_dir(fname) == 0)
    {
        throw exception("duplicate name");
    }
    dnum = next_des();
    if (dnum == -1)
    {
        throw exception("too many files");
    }
    fd[dnum].fsize = 0;
    int c = assign_dir(fname);
    if (c == -1)
    {
        //remove descriptor above
        fd[dnum].fsize = -1;
        throw exception("no free directory entry found");
    }
    else if (c == -2)
    {
        //remove descriptor above
        fd[dnum].fsize = -1;
        throw exception("no free directory entry found and no block available to expand directory");
    }
}

void FileSystem::destroy(char* fname)
{
    if (search_dir(fname) == -1)
    {
        throw exception("file not found");
    }
    remove_dir();   // search_dir should leave position at the correct dir entry, we can remove it from there
}

int FileSystem::open(char* fname)
{
    if (search_dir(fname) == -1)
    {
        throw exception("file not found");
    }
    int onum = oft->searchentry();
    if (onum == -1)
    {
        throw exception("too many files open");
    }
    char* buf = oft->get_buf(0);
    DirEntry* cur = (DirEntry*)(buf+oft->get_pos(0));
    oft->loadfile(onum, cur->desci, fd[cur->desci].fsize);
    if (fd[cur->desci].fsize == 0)
    {
       int blocknum = get_block();
       if (blocknum == -1)
       {
           throw exception("no block available");
       }
       fd[cur->desci].b1 = blocknum;
    }
    disk->read_block(fd[cur->desci].b1, oft->get_buf(onum));
    return onum;
}

int FileSystem::close(int i)
{
    if (oft->get_pos(i)-1 / 512 == 0)
    {
        disk->write_block(fd[oft->get_desci(i)].b1, oft->get_buf(i));
    }
    else if (oft->get_pos(i)-1 / 512 == 1)
    {
        disk->write_block(fd[oft->get_desci(i)].b2, oft->get_buf(i));
    }
    else
    {
        disk->write_block(fd[oft->get_desci(i)].b3, oft->get_buf(i));
    }
    fd[oft->get_desci(i)].fszie = oft->get_fsize(i);
    oft->removefile(i);
    return 0;
}

// case 1: pos is in correct position  case 2: pos is 512 and fsize = 512  case 3: pos is fsize
int FileSystem::read(int i, int m, int n)
{
    int bytesread = 0;
    char* start = (M+m);
    
    for (int i = 0; i < n; i++)
    {
        int check = oft->read_check(i);
        if (check == 2)
        {
            disk->write_block(fd[oft->get_desci(i)].b1, oft->get_buf(i));
            disk->read_block(fd[oft->get_desci(i)].b2, oft->get_buf(i));
        }
        else if (check == 3)
        {
            disk->write_block(fd[oft->get_desci(i)].b2, oft->get_buf(i));
            disk->read_block(fd[oft->get_desci(i)].b3, oft->get_buf(i));
        }
        else if (check == 4)
        {
            return bytesread;
        }
        if (m + i >= 512)
        {
            return bytesread;
        }
        oft->readbyte(i, start+i);
        bytesread++;
    }
    return bytesread;
}

int FileSystem::write(int i, int m, int n)
{
    char* start = (M+m);
    int byteswritten = 0;
    
    for (int i = 0; i < n; i++)
    {
        int check = oft->write_check(i);
        if (check == 2)
        {
            disk->write_block(fd[oft->get_desci(i)].b1, oft->get_buf(i));
            if (fd[oft->get_desci(i)].b2 == 0)
            {
                int blocknum = get_block();
                if (blocknum == -1)
                {
                    return byteswritten;
                }
                fd[oft->get_desci(i)].b2 = blocknum;
            }
            disk->read_block(fd[oft->get_desci(i)].b2, oft->get_buf(i));
        }
        else if (check == 3)
        {
            disk->write_block(fd[oft->get_desci(i)].b1, oft->get_buf(i));
            if (fd[oft->get_desci(i)].b3 == 0)
            {
                int blocknum = get_block();
                if (blocknum == -1)
                {
                    return byteswritten;
                }
                fd[oft->get_desci(i)].b3 = blocknum;
            }
            disk->read_block(fd[oft->get_desci(i)].b3, oft->get_buf(i));
        }
        else if (check == 4)
        {
            return byteswritten;
        }
        if (m + i >= 512)
        {
            return byteswritten;
        }
        oft->writebyte(i, start+i);
        byteswritten++;
    }
    return byteswritten;
}

int FileSystem::seek(int onum, int position)
{
    if (position < 0 || position > oft->get_fsize(onum))
    {
        throw exception("position is invalid");
    }
    int check = oft->seek(onum, position);
    if (check == 1)
    {
        read_block(fd[oft.get_desci(onum)].b1, oft.get_buffer(onum));
    }
    else if (check == 2)
    {
        read_block(fd[oft.get_desci(onum)].b2, oft.get_buffer(onum));
    }
    else if (check == 3)
    {
        read_block(fd[oft.get_desci(onum)].b3, oft.get_buffer(onum));
    }
    return oft->get_pos(onum);
}

std::string FileSystem::directory()
{
    seek(0, 0);
    std::string out;
    char* dir = oft->getbuf(0);
    int begin = 0;
    for (int i = 0; i < 512; i = i + 8)
    {
        DirEntry* cur = (DirEntry*)(dir+i);
        if (cur->sname == 0)
        {
            if (begin != 0)
            {
                out.append(" ");
            }
            out.append((char*)(&cur->sname));
            int fs = fd[cur->desci].fsize;
            out.append(" ");
            out.append((char*)(&fs));
            begin = 1;

        }
        oft->change_pos(i+8);
    }
    // check if next block for directory exists, if not, assign one
    if (fd[dnum].b2 == 0)
    {
        return out;
    }
    disk->read_block(fd[dnum].b2, oft->getbuf(0));
    dir = oft->getbuf(0);
    for (int i = 0; i < 512; i = i + 8)
    {
        DirEntry* cur = (DirEntry*)(dir+i);
        if (cur->sname == 0)
        {
            if (begin != 0)
            {
                out.append(" ");
            }
            out.append((char*)(&cur->sname));
            int fs = fd[cur->desci].fsize;
            out.append(" ");
            out.append((char*)(&fs));
            begin = 1;

        }
        oft->change_pos(i+8);
    }
    if (fd[dnum].b3 == 0)   //last block of dir, check if it exists or not
    {
        return out;
    }
    disk->read_block(fd[dnum].b3, oft->getbuf(0));
    dir = oft->getbuf(0);
    for (int i = 0; i < 512; i = i + 8)
    {
        DirEntry* cur = (DirEntry*)(dir+i);
        if (cur->sname == 0)
        {
            if (begin != 0)
            {
                out.append(" ");
            }
            out.append((char*)(&cur->sname));
            int fs = fd[cur->desci].fsize;
            out.append(" ");
            out.append((char*)(&fs));
            begin = 1;

        }
        oft->change_pos(i+8);
    }
    return out;
}

void FileSystem::init()
{
    for (int i = 0; i < 512; i++)
    {
        I[i] = '\0';
        O[i] = '\0';
        M[i] = '\0';
    }
    disk->clear();
    bitmap = 0xFF00000000000000; 
    /*
    for (int i = 0; i < 192; i++)
    {
        fd[i] = Descriptor();
    }
    */
    setdes(0, 0, 7, -1, -1);
    oft->init();
    oft->loadfile(0, 0, 0);

}

std::string FileSystem::read_memory(int m, int n)
{
    std::string out;
    for (int i = 0; i < n; i++)
    {
        if (m + i >= 512)
        {
            return out;
        }
        out.append(*(M+m+i));
    }
    return out;
}

int FileSystem::write_memory(int m, char* s)
{
    int byteswritten = 0;
    for (int i = 0; i < strlen(s); i++)
    {
        if (m + i >= 512)
        {
            return byteswritten;
        }
        *(M+m+i) = *(s+i);
        byteswritten++;
    }
    return byteswritten;
}

//return -2 = no available block to expand directory  return -1 = no available directory entry  return 0 = success!
int FileSystem::assign_dir(int dnum, char* fname)  
{
    seek(0, 0);  //seek changes oft buffer to proper block
    int f = (int)*fname;
    char* dir = oft->getbuf(0);
    for (int i = 0; i < 512; i = i + 8)
    {
        DirEntry* cur = (DirEntry*)(dir+i);
        oft->change_pos(i+8);
        if (cur->sname == 0)
        {
            cur->sname = f;
            cur->desci = dnum;
            return 0;
        }
    }
    // check if next block for directory exists, if not, assign one
    if (fd[dnum].b2 == 0)
    {
        int nblock = get_block(dnum);
        if (nblock == -1)
        {
            return -2;
        }
        // load new block into oft[0]
        disk->read_block(nblock, oft->getbuf(0));
        DirEntry* cur = (DirEntry*)(oft->getbuf(0));
        cur->sname = f;
        cur->desci = dnum;
        oft->change_pos(520);
        return 0;
    }
    disk->read_block(fd[dnum].b2, oft->getbuf(0));
    dir = oft->getbuf(0);
    for (int i = 0; i < 512; i = i + 8)
    {
        DirEntry* cur = (DirEntry*)(dir+i);
        oft->change_pos(i+8);
        if (cur->sname == 0)
        {
            cur->sname = f;
            cur->desci = dnum;
            return 0;
        }
    }
    if (fd[dnum].b3 == 0)   //last block of dir, check if it exists or not
    {
        int nblock = get_block(dnum);
        if (nblock == -1)
        {
            return -2;
        }
        // load new block into oft[0]
        disk->read_block(nblock, oft->getbuf(0));
        DirEntry* cur = (DirEntry*)(oft->getbuf(0));
        cur->sname = f;
        cur->desci = dnum;
        oft->change_pos(520);
        return 0;
    }
    disk->read_block(fd[dnum].b3, oft->getbuf(0));
    dir = oft->getbuf(0);
    for (int i = 0; i < 512; i = i + 8)
    {
        DirEntry* cur = (DirEntry*)(dir+i);
        oft->change_pos(i+8);
        if (cur->sname == 0)
        {
            cur->sname = f;
            cur->desci = dnum;
            return 0;
        }
    }
    return -1;
}

void FileSystem::remove_dir()
{
    char* buf = oft->get_buf(0);
    DirEntry* cur = (DirEntry*)(buf+oft->get_pos(0));
    cur->sname = 0;
    fd[cur->desci].fsize = -1;
    if (fd[cur->desci].b1 != 0)
    {
        bitmap = bitmap - pow(2, (63-fd[cur->desci].b1));
        fd[cur->desci].b1 = 0;
    }
    if (fd[cur->desci].b2 != 0)
    {
        bitmap = bitmap - pow(2, (63-fd[cur->desci].b2));
        fd[cur->desci].b1 = 0;
    }
    if (fd[cur->desci].b3 != 0)
    {
        bitmap = bitmap - pow(2, (63-fd[cur->desci].b1));
        fd[cur->desci].b3 = 0;
    }
    cur->desci = 0;
}

int FileSystem::next_des(int dnum)
{
    for(int i = 1; i < 192; i++)
    {
        if (fd[i].fsize == -1)
        {
            return i;
        }
    }
    return -1;
}

int FileSystem::get_block()
{
    for (int i = 8; i < 64; i++)
    {
        long long m = pow(2, 63-i);
        if (bitmap & m == 0)  //block is free
        {
            bitmap = bitmap + m;
            return i;
        }
    }
    return -1;
}

int FileSystem::search_dir(char* fname)
{
    seek(0, 0);
    int f = (int)*fname;
    char* dir = oft->getbuf(0);
    for (int i = 0; i < 512; i = i + 8)
    {
        DirEntry* cur = (DirEntry*)(dir+i);
        if (cur->sname == f)
        {
            return 0;
        }
        oft->change_pos(i+8);
    }
    // check if next block for directory exists, if not, assign one
    if (fd[dnum].b2 == 0)
    {
        return -1;
    }
    disk->read_block(fd[dnum].b2, oft->getbuf(0));
    dir = oft->getbuf(0);
    for (int i = 0; i < 512; i = i + 8)
    {
        DirEntry* cur = (DirEntry*)(dir+i);
        if (cur->sname == f)
        {
            return 0;
        }
        oft->change_pos(i+8);
    }
    if (fd[dnum].b3 == 0)   //last block of dir, check if it exists or not
    {
        return -1;
    }
    disk->read_block(fd[dnum].b3, oft->getbuf(0));
    dir = oft->getbuf(0);
    for (int i = 0; i < 512; i = i + 8)
    {
        DirEntry* cur = (DirEntry*)(dir+i);
        if (cur->sname == f)
        {
            return 0;
        }
        oft->change_pos(i+8);
    }
    return -1;
}

