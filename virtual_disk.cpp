#include "vfs.h"
int virtual_disk::findEmptyBlock()
{
    int i;

    file_table *current = this->getRootFileTable();
    vector<int> allBlockInUse;
    vector<int> allFileTable;

    allFileTable.push_back(0);
    while (true)
    {
        for (i = 0; i < FILE_DESCRIPTOR_PER_TABLE; i++)
        {

            vector<int> temp = current->getFdFileDescriptor(i)->getBlockInUse(this->physicalDisk);

            allBlockInUse.insert(allBlockInUse.end(), temp.begin(), temp.end());
        }
        int seekFileTable = current->getNextFilePage();

        if (seekFileTable != -1)
        {
            allFileTable.push_back(seekFileTable); //hold all file table location
            this->physicalDisk->seekg(seekFileTable);
            current = file_table::readFromFile(this->physicalDisk);
        }
        else
        {
            break;
        }
    }
    sort(allBlockInUse.begin(), allBlockInUse.end());
    sort(allFileTable.begin(), allFileTable.end());
    int counter = 0;

    while (true)
    {
        if (binary_search(allFileTable.begin(), allFileTable.end(), counter))
        {
            counter += (FILE_DESCRIPTOR_PER_TABLE * file_descriptor::sizeOfFileDescriptor()) + 4;
        }
        else if (binary_search(allBlockInUse.begin(), allBlockInUse.end(), counter))
        {
            counter += VIRTUAL_BLOCK_SIZE + 4;
        }
        else
        {
            break;
        }
    }

    return counter;
};

int virtual_disk::getFileDescriptorInDisk(virtual_disk &disk, char name)
{
    file_table *table = file_table::findFileTable(disk, name);

    if (table == nullptr)
    {
        return -1;
    }

    return file_descriptor::findOffsetInFileTable(disk, table, name) + table->getFileTableLocation();
};

int virtual_disk::findEmptyfdSpot(virtual_disk &disk)
{
    file_table *current = disk.getRootFileTable();
    int next_location = current->getFileTableLocation();
    int check;
    while (true)
    {
        //didnt find the fd in this table need to go to the next one
        int check = current->findEmpyFdInTable();
        if (check == -1)
        {
            next_location = current->getNextFilePage();
            if (next_location == -1)
            {
                return -1;
            }
            disk.getPhysicalDisk()->seekg(next_location);
            current = file_table::readFromFile(disk.getPhysicalDisk());
        }
        else
        {
            int sumSizeOfMembersOfFileDesc = file_descriptor::sizeOfFileDescriptor();
            return next_location + (sumSizeOfMembersOfFileDesc * check);
        }
    }
};

file_table *virtual_disk::allocatNewFileTable(virtual_disk &disk)
{
    file_table *table = new file_table();
    disk.getPhysicalDisk()->seekp(0, disk.getPhysicalDisk()->end);
    table->setFileTableLocation(disk.getPhysicalDisk()->tellp());
    table->writeToFile(disk.getPhysicalDisk());

    return table;
};
