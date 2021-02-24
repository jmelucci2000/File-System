#include "FileSystem.hpp"
#include <iostream>
#include <fstream>

void split(std::string s)
{
    std::string delim = " ";

}

int main()
{
	FileSystem fs;
    fs.init();
    std::string infile;
	cout << "Input File Name: ";
	getline(cin, infile);
    std::ifstream in;
    in.open(infile);
    std::string outfile;
    cout << "Output File Name: ";
    getline(cin, outfile);
    std::ofstream out;
	out.open(outfile);
    
	
    char line[512];
    char delim[] = " ";
	while (in.getline(line, 512))
	{
		try 
		{
            char* t = strtok(line, delim);
			if (strcmp(t, "cr") == 0)
			{
				char* name = strtok(NULL, delim);
                if (strlen(name) > 3)
                {
                    out << "error" << endl;
                }
				fs.create(name);
				out << name << " created "<<endl;
			}
			else if (strcmp(t, "de") == 0)
			{
				char* name = strtok(NULL, delim);
				fs.destroy(name);
				out << name << " destroyed " << endl;
			}
			else if (strcmp(t, "op") == 0)
			{
				char* name = strtok(NULL, delim);
				int in = fs.destroy(name);
				out << name << " opened " << index << endl;
			}
			else if (strcmp(t, "cl") == 0)
			{
				char* ind = strtok(NULL, delim);
				int in = atoi(ind);
				if (in < 1)
				{
					out << "error" << endl;
				}
				fs.close(in);
				out << in << " closed" << endl;
			}
			else if (strcmp(t, "rd") == 0)
			{
                char* ic1 = strtok(NULL, delim);
				int i1 = atoi(ind);
				if (i1 < 1)
				{
					out << "error" << endl;
				}
				char* ic2 = strtok(NULL, delim);
                int i2 = atoi(ic2);
                char* ic3 = strtok(NULL, delim);
                int i3 = atoi(ic3);
                int read = fs.read(i1, i2, i3);
				out << read << " bytes read from " << i1 << endl;
			}
			else if (strcmp(t, "wr") == 0)
			{
				char* ic1 = strtok(NULL, delim);
				int i1 = atoi(ind);
				if (i1 < 1)
				{
					out << "error" << endl;
				}
				char* ic2 = strtok(NULL, delim);
                int i2 = atoi(ic2);
                char* ic3 = strtok(NULL, delim);
                int i3 = atoi(ic3);
                int write = fs.write(i1, i2, i3);
				out << write << " bytes written to " << i1 << endl;
			}
			else if (strcmp(t, "sk") == 0)
			{
                char* ic1 = strtok(NULL, delim);
				int i1 = atoi(ind);
				char* ic2 = strtok(NULL, delim);
                int i2 = atoi(ic2);
                int npos = fs.seek(i1, i2);
				out << "position is " << npos << endl;
			}
			else if (strcmp(t, "dr") == 0)
			{
				std::string dirstring = fs.directory();
				out << dirstring << endl;
			}
			else if (strcmp(t, "in") == 0)
			{
				fs.init();
                out << "system initialized" << endl;
			}
            else if (strcmp(t, "rm") == 0)
			{
				char* ic1 = strtok(NULL, delim);
				int i1 = atoi(ind);
				char* ic2 = strtok(NULL, delim);
                int i2 = atoi(ic2);
                
                std::string mem = fs.read_memory(i1, i2);
                out << mem << endl;
			}
            else if (strcmp(t, "wm") == 0)
			{
				char* ic1 = strtok(NULL, delim);
				int i1 = atoi(ind);
				char* ic2 = strtok(NULL, delim);
                
                int written = fs.write_memory(i1, ic2);
				out << written << endl;
			}
			else 
			{
				out << "error" << endl;
			}
		}
		catch (exception& e)
		{
			out << "error" << endl;
		}
	}
	in.close();
	out.close();
	return 0;
}