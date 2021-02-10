#include "vfs.h"
#include <iostream>

file_table* file_table::findFileTable(virtual_disk &disk, char name ){
    
    file_table *current = disk.getRootFileTable();
    current->updateFromFile(disk.getPhysicalDisk());
    disk.getPhysicalDisk()->seekg(0);
    int next_location = current->getFileTableLocation();
    int check;
    while (true)
    {
        //didnt find the fd in this table need to go to the next time
        int check = current->findFdInFileTable(name);
        if (check == -1)
        {
            next_location = current->getNextFilePage();
            if (next_location == -1)
            {
                return nullptr;//was break;
            }
            disk.getPhysicalDisk()->seekg(next_location);
            current = file_table::readFromFile(disk.getPhysicalDisk());
        }
        else
        {
            return current;
        }
    }
};

file_table* file_table::readFromFile(fstream *physicalDisk)
{
    file_table* table = new file_table();//defult constractor
    table->updateFromFile(physicalDisk);
    return table;
};


void file_table::updateFromFile(fstream *physicalDisk)
{
    int i;
    physicalDisk->seekg(this->getFileTableLocation());

    for (i = 0; i < FILE_DESCRIPTOR_PER_TABLE; i++)
    {
        this->getFdFileDescriptor(i)->updateFromFile(physicalDisk);
    }
    this->setNextFilePage(getIntFromFile(physicalDisk)); //next table if end
};

void file_table::writeToFile(fstream *physicalDisk)
{
    int i;
    for (i = 0; i < FILE_DESCRIPTOR_PER_TABLE; i++)
    {
        this->getFdFileDescriptor(i)->writeToFile(physicalDisk);
    }
    writeIntToFile(physicalDisk, this->getNextFilePage()); //next table if end
};

int file_table ::findFdInFileTable(char name)
{
    int i;
    
    for (i = 0; i < FILE_DESCRIPTOR_PER_TABLE; i++)
    {
        if (this->getFdFileDescriptor(i)->getName() == name)
        {
            break;
        }
    }
    if (i == FILE_DESCRIPTOR_PER_TABLE){
        return -1;
    }
    return i;
};
int file_table ::findEmpyFdInTable()
{
    int i;
    for (i = 0; i < FILE_DESCRIPTOR_PER_TABLE; i++)
    {
        if (this->getFdFileDescriptor(i)->getBegin() == -1)
        {
            break;
        }
    }
    if (i == VIRTUAL_BLOCK_SIZE){
        return -1;
    }
    return i;
};
