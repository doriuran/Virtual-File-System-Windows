#include "vfs.h"

int main()
{
    fstream *physical_disk = new fstream();
    physical_disk->open("C:\\virtualfile\\vfs.txt",ios::in | ios::out | fstream::trunc);
    if (!physical_disk)
    {
        cerr << "Cannot open the output file." << endl;
    }

    string to_write = "trying to wr";
    string to_write1 ="ite";
    string to_write2 = "dor";
    string to_write3 = "bool";

    virtual_disk *vfs = new virtual_disk(physical_disk);

    file_descriptor* a = virtual_file::openVirtualFile(*vfs,'a');
    
    virtual_file::writeToVirtualFile(*vfs, a, 12, to_write.c_str(), false);
    //output show be "trying to wr"
    cout <<virtual_file::readFromVirtualFile(*vfs,a,12)<<endl;
    virtual_file::appendToVirtualFile(*vfs, a, 3, to_write1.c_str());
    //output show be "trying to write"
    cout <<virtual_file::readFromVirtualFile(*vfs,a,15)<<endl;


    file_descriptor* b =virtual_file::openVirtualFile(*vfs,'b');
    virtual_file::writeToVirtualFile(*vfs, b, 3, to_write2.c_str(), false);
     //output show be "dor"
    cout <<virtual_file::readFromVirtualFile(*vfs,b,3)<<endl;
    
    virtual_file::deleteFile(*vfs, a);


    file_descriptor* c = virtual_file::openVirtualFile(*vfs,'c');
    virtual_file::writeToVirtualFile(*vfs, c, 4, to_write3.c_str(), false);
    //output show be "bool"
    cout <<virtual_file::readFromVirtualFile(*vfs,c,4)<<endl;




}