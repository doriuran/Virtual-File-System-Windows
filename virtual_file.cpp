#include "vfs.h"

// virtual file represnt each file is written to the vfs system file(the physical file)
void virtual_file ::deleteFile(virtual_disk &disk, file_descriptor* fd)
{
    file_table *current = disk.getRootFileTable();
    int next_location = virtual_disk::getFileDescriptorInDisk(disk, fd->getName());
    disk.getPhysicalDisk()->seekp(next_location);

    fd->setName('\0');
    fd->setbegin(-1);
    fd->setLength(-1);
    fd->setOpen(false);
    fd->writeToFile(disk.getPhysicalDisk());
};

char *virtual_file::readFromVirtualFile(virtual_disk &disk, file_descriptor* fd, int size_buff)
{
    char *to_return = new char[(size_buff+1)];
    if (fd->getOpen() != true)
    {
        cerr << "FIle wasnt open correctly" << endl;
    }
    disk.getPhysicalDisk()->seekg(fd->getBegin());
    if (size_buff > fd->getLength()){
        cerr << "Trying to read more characters then written"<< endl;
    }
    int num_of_blocks = ceil((double) size_buff / VIRTUAL_BLOCK_SIZE);
    int location= fd->getBegin();
    int total_char=0;
    while (num_of_blocks > 0)
    {
        int size_to_read = min(VIRTUAL_BLOCK_SIZE, size_buff);
        disk.getPhysicalDisk()->read((to_return + total_char) , (size_to_read));
        total_char +=size_to_read;
        size_buff -= size_to_read;
        num_of_blocks--;
        if (num_of_blocks != 0)
        {
            disk.getPhysicalDisk()->seekg(location + VIRTUAL_BLOCK_SIZE);
            //need to read 4 chars for the int and replace it to real int
            location = getIntFromFile(disk.getPhysicalDisk());
            disk.getPhysicalDisk()->seekg(location); //move to the right block
            
        }
        
    }
    return to_return;
};

void virtual_file::writeToVirtualFile(virtual_disk &disk, file_descriptor* fd, int size, const char *buff,bool dont_seek)
{
    int total_size = size + fd->getLength();
    int emptyDisk;
    if (fd->getOpen() != true)
    {
        cerr << "FIle wasnt open correctly" << endl;
    }

    if (fd->getBegin() == -1)
    {
        emptyDisk = disk.findEmptyBlock();
        fd->setbegin(emptyDisk);
    }
    if(dont_seek != true){
        disk.getPhysicalDisk()->seekp(fd->getBegin());
    }
    
    int num_of_blocks = ceil((double) size /(double) VIRTUAL_BLOCK_SIZE);
    int location = disk.getPhysicalDisk()->tellg();

    while (num_of_blocks > 0)
    {
        int size_to_write = min(VIRTUAL_BLOCK_SIZE, size);
        disk.getPhysicalDisk()->write(buff, size_to_write);
        buff += size_to_write;
        size -= size_to_write;
        num_of_blocks--;
        if (num_of_blocks != 0)
        {
            int next_block_location = location + VIRTUAL_BLOCK_SIZE;
            location = disk.findEmptyBlock();

            disk.getPhysicalDisk()->seekp(next_block_location);
            writeIntToFile(disk.getPhysicalDisk(), location);
            disk.getPhysicalDisk()->seekp(location);
        }
    }
    
    int next_location = virtual_disk::getFileDescriptorInDisk(disk, fd->getName());
    fd->setLength(total_size);
    disk.getPhysicalDisk()->seekp(next_location);//go just the lengh place 
    fd->writeToFile(disk.getPhysicalDisk());   
};

void virtual_file::appendToVirtualFile(virtual_disk &disk, file_descriptor* fd,int size, const char *buff)
{
    
    //check first the length that already wrtitten before
    int to_go = fd->getLength();
    int current = fd->getBegin();

    while(to_go > VIRTUAL_BLOCK_SIZE){
        disk.getPhysicalDisk()->seekp(current + VIRTUAL_BLOCK_SIZE);
        current = getIntFromFile(disk.getPhysicalDisk());
        to_go -= VIRTUAL_BLOCK_SIZE;
    }
    current += to_go;
    disk.getPhysicalDisk()->seekp(current);
    writeToVirtualFile(disk, fd, size, buff,true);
};

void virtual_file::seekVirtualFile(file_descriptor* fd, fstream *physicalDisk, int location)
{
    physicalDisk->seekp((fd->getBegin()) + location);
};

file_descriptor* virtual_file::openVirtualFile(virtual_disk &disk, char name)
{
    file_descriptor *fd= nullptr;
    int location_of_fd_in_disk = virtual_disk::getFileDescriptorInDisk(disk,name);
    bool new_file = false;

    if (location_of_fd_in_disk == -1){
        location_of_fd_in_disk = virtual_disk::findEmptyfdSpot(disk);
        new_file = true;
    }
    if (location_of_fd_in_disk == -1){//still didnt find spot 
        file_table* table = virtual_disk::allocatNewFileTable(disk);
        disk.getRootFileTable()->file_table::setNextFilePage(table->getFileTableLocation());
        location_of_fd_in_disk =table->getFileTableLocation();
        new_file = true;
    }
    if (new_file) {

        int b = disk.findEmptyBlock();
        file_descriptor temp = file_descriptor(name,b,0,true);
        disk.getPhysicalDisk()->seekp(location_of_fd_in_disk);//move to the right location
        temp.writeToFile(disk.getPhysicalDisk());
        file_table* table = file_table::findFileTable(disk, name);
        fd = table->getFdFileDescriptor(table->findFdInFileTable(name));
    }
    else {
        file_table* table = file_table::findFileTable(disk, name);
        fd = table->getFdFileDescriptor(table->findFdInFileTable(name));

    }
    return fd;
};