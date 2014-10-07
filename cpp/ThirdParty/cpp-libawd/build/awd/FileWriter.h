#ifndef _LIBAWD_FILEWRITER_H
#define _LIBAWD_FILEWRITER_H

#include <stdlib.h>
#include <stdio.h>

#include <string>
//#include "awd.h"
#include "awd_types.h"
using namespace std;
class AWDFileWriter
{
    private:
        // File header fields
        FILE * file;
		bool swapBytes;

    public:
        AWDFileWriter(FILE* file);
        ~AWDFileWriter();
		
		void set_file(FILE* file);
		FILE* get_file();
		void writeBOOL(awd_bool);
		void writeUINT82(awd_uint8);
		void writeUINT8(awd_uint8);
		void writeUINT16(awd_uint16);
		void writeUINT32(awd_uint32);
		void writeFLOAT32(awd_float32);
		void writeFLOAT64(awd_float64);
		void writeBytes(awd_uint8* val, int length);
		void writeSTRING(string& s, int prepentInt);
		void writeFLOAT32multi(awd_float32* val, int length);
		void writeUINT16multi(awd_uint16* val, int length);
};

#endif
