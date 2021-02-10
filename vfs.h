#ifndef VFS_H
#define VFS_H
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <windows.h>
#include <iterator>
using namespace std;
#define FILE_DESCRIPTOR_PER_TABLE 10
#define VIRTUAL_BLOCK_SIZE 8
class file_table;
class virtual_disk;

//Ulities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char decToBinary(int n);
int binaryToDec(char c);
void intToBinary(int n, char charArray[]);
int binaryToInt(char arrayChar[]);
int getIntFromFile(fstream *physicalDisk);
void writeIntToFile(fstream *physicalDisk, int var);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class file_descriptor
{
private:
    char name = '\0';               // the first char that writtemn to the file ->that his name
    int begin = -1;                 // index of start file in the storge file
    int length = 0;                // length of the file
    bool open = false;              // open/close

public:
    file_descriptor(){};
    file_descriptor(char name,int begin,int lenght, bool open);
    vector<int> getBlockInUse(fstream *physicalDisk);

    char getName()
    {
        return this->name;
    };

    void setName(char temp)
    {
        this->name = temp;
    };

    int getBegin()
    {
        return this->begin;
    };

    void setbegin(int temp)
    {
        this->begin = temp;
    };

    int getLength()
    {
        return this->length;
    };

    void setLength(int temp)
    {
        this->length = temp;
    };

    bool getOpen()
    {
        return this->open;
    };

    void setOpen(bool temp)
    {
        this->open = temp;
    };
    static int sizeOfFileDescriptor();
    static file_descriptor* readFromFile(fstream *physicalDisk);
    void updateFromFile(fstream *physicalDisk);
    void writeToFile(fstream *physicalDisk);
    static int findOffsetInFileTable(virtual_disk &disk, file_table* table,char name);
    static void writeBoolToFile(fstream *physicalDisk, bool open);
    
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class file_table
{
private:

    int next_file_page = -1; //initializ
    file_descriptor *fds[10];
    int begin = -1;

public:
file_table(){
    for (size_t i = 0; i < 10; i++)
    {
    fds[i]=new file_descriptor('\0',-1,0,false);
    }
};

    int getNextFilePage()
    {
        return this->next_file_page;
    };
    void setNextFilePage(int temp)
    {
        this->next_file_page = temp;
    };
    file_descriptor* getFdFileDescriptor(int index)
    {

        return this->fds[index];
    };

    void setFdFileDescriptor(file_descriptor* fd, int index)
    {
        this->fds[index] = fd;
    };
    int getFileTableLocation()
    {
        return this->begin;
    };
    void setFileTableLocation(int index)
    {
        this->begin = index;
    };
    static file_table* readFromFile(fstream *physicalDisk);
    void updateFromFile(fstream *physicalDisk);
    int findFdInFileTable(char name);
    void writeToFile(fstream *physicalDisk);
    static file_table* findFileTable(virtual_disk &disk,char name );
    int findEmpyFdInTable();

    
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class virtual_disk
{
private:
    fstream *physicalDisk;
    file_table* rootFileTable;

public:
    virtual_disk(fstream *file_input) : physicalDisk(file_input) {

    file_input->seekg(0, file_input->end);
    int length =  file_input->tellg();
    if(length == 0){
        this->physicalDisk->seekp(0);
        this->rootFileTable = new file_table();
        this->rootFileTable->setFileTableLocation(0);
        this->rootFileTable->writeToFile(this->physicalDisk);
    } else {
        this->rootFileTable->updateFromFile(this->physicalDisk);
        this->rootFileTable->setFileTableLocation(0);
    }
    };
    
    file_table *getRootFileTable()
    {
        return this->rootFileTable;
    };

    fstream *getPhysicalDisk()
    {
        return (this->physicalDisk);
    };
    
    int findEmptyBlock();
    static int getFileDescriptorInDisk(virtual_disk &disk, char name);
    static int findEmptyfdSpot(virtual_disk &disk);
    static file_table* allocatNewFileTable(virtual_disk &disk);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class virtual_file
{

public:
    static file_descriptor* openVirtualFile(virtual_disk &disk, char name);
    static char *readFromVirtualFile(virtual_disk &disk, file_descriptor* fd, int size_buff);
    static void writeToVirtualFile(virtual_disk &disk, file_descriptor* fd, int size,const char *buff, bool need_seek);
    static void appendToVirtualFile(virtual_disk &disk, file_descriptor* fd, int size, const char *buff);
    static void seekVirtualFile(file_descriptor* fd, fstream *physicalDisk, int location);
    static void deleteFile(virtual_disk &disk, file_descriptor* fd);
    
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif