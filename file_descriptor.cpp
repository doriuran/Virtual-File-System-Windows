#include "vfs.h"

file_descriptor::file_descriptor(char name,int begin,int lenght, bool open)
{
    this->name = name;
    this->begin = begin;
    this->length = length;
    this->open = open;
};

vector<int> file_descriptor::getBlockInUse(fstream *physicalDisk)
{
    

    if (this->length == 0 && this->name == '\0')
    {
        return vector<int>{};
    } else if (this->length == 0 || this->length < VIRTUAL_BLOCK_SIZE) {
        return vector<int>{this->begin};
    }

    vector<int> blocInUse;    
    double totalBlockInUse = ceil((double) this->length / (double) VIRTUAL_BLOCK_SIZE);
    int cureentBlock = this->begin;

    while (totalBlockInUse > 0)
    {
        blocInUse.push_back(cureentBlock);
        totalBlockInUse -= 1;
        if (totalBlockInUse>0){
            physicalDisk->seekg((cureentBlock + VIRTUAL_BLOCK_SIZE));
            cureentBlock = getIntFromFile(physicalDisk); //location in the storge
        }

    }

    return blocInUse;
};

file_descriptor * file_descriptor::readFromFile(fstream *physicalDisk)
{
    file_descriptor* fd = new file_descriptor();
    fd->updateFromFile(physicalDisk);
    return fd;
};


void file_descriptor::updateFromFile(fstream *physicalDisk)
{
    char fileName;
    physicalDisk->get(fileName);
    this->setName(fileName);
    this->setbegin(getIntFromFile(physicalDisk));
    this->setLength(getIntFromFile(physicalDisk));
    char temp;
    physicalDisk->get(temp);
    this->setOpen(int(temp));
};

void file_descriptor::writeToFile(fstream *physicalDisk)
{
    physicalDisk->write(&(this->name), sizeof(char));
    writeIntToFile(physicalDisk, (this->begin));
    writeIntToFile(physicalDisk, (this->length));
    writeBoolToFile(physicalDisk, (this->open));
    physicalDisk->flush();
};

void writeIntToFile(fstream *physicalDisk, int var)
{
    char charArray[sizeof(int)];
    intToBinary(var, charArray);
    physicalDisk->write(charArray, sizeof(int));
    physicalDisk->flush();

};

int getIntFromFile(fstream *physicalDisk)
{ // getting 4 bytes from the file and replacing it to int
    int temp;
    char charArray[sizeof(int)];
    int i;
    for (i = 0; i < sizeof(int); i++)
    {
        physicalDisk->get(charArray[i]);
    }
    temp = binaryToInt(charArray);
    return temp;
};

int file_descriptor::findOffsetInFileTable(virtual_disk &disk, file_table* table,char name )
{
    int check = table->findFdInFileTable(name);
    if (check == -1)
    {
        return -1; 
    }
    else
    {
        return check * sizeOfFileDescriptor();
    }
};
void file_descriptor::writeBoolToFile(fstream *physicalDisk, bool open){
    const char* temp;
    char t = '1';
    char f = '0';
    if(open == true){
        temp = &t;
    }
    else{
        temp = &f;
    }
    physicalDisk->write(temp, sizeof(char));
};

int file_descriptor::sizeOfFileDescriptor(){
    
    int to_return = 0;
    to_return = sizeof(file_descriptor::name) +sizeof(file_descriptor::begin) + sizeof(file_descriptor::length)+sizeof(file_descriptor::open);
    
    return to_return;
};