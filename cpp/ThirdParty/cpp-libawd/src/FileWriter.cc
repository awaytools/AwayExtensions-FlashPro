#include "awd_types.h"
#include "FileWriter.h"
#include "util.h"


AWDFileWriter::AWDFileWriter(FILE * file)
{
        this->file=file;
		this->swapBytes=false;
		int num = 1;
		if(*(char *)&num == 1)
		{
			this->swapBytes=false;
		}
		else
		{
			this->swapBytes=true;
		}
}
AWDFileWriter::~AWDFileWriter()
{
}
void
AWDFileWriter::set_file(FILE*file)
{
	this->file=file;
}
FILE*
AWDFileWriter::get_file()
{
	return this->file;
}
void
AWDFileWriter::writeBOOL(awd_bool val)
{
    fwrite(reinterpret_cast<const char*>(&val), sizeof(awd_bool), 1, this->file);
	fflush (this->file);
}
void
AWDFileWriter::writeUINT8(awd_uint8 val)
{
    fwrite(reinterpret_cast<const char*>(&val), sizeof(awd_uint8), 1, this->file);
	fflush (this->file);
}
void
AWDFileWriter::writeBytes(awd_uint8* val, int length)
{
    fwrite(reinterpret_cast<const char*>(val), sizeof(awd_uint8), length, this->file);
	fflush (this->file);
}
void
AWDFileWriter::writeUINT16(awd_uint16 val)
{
	if(this->swapBytes){
		awd_uint16 swaptVal=awdutil_swapui16(val);
		fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(awd_uint32), 1, this->file);
	}
	else{
		fwrite(reinterpret_cast<const char*>(&val), sizeof(awd_uint16), 1, this->file);
	}
	fflush (this->file);
}
void
AWDFileWriter::writeUINT16multi(awd_uint16* val, int length)
{
    fwrite(reinterpret_cast<const char*>(val), sizeof(awd_uint16), length, this->file);
	fflush (this->file);
}
void
AWDFileWriter::writeUINT32(awd_uint32 val)
{
	if(this->swapBytes){
		awd_uint32 swaptVal=awdutil_swapui32(val);
		fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(awd_uint32), 1, this->file);
	}
	else{
		fwrite(reinterpret_cast<const char*>(&val), sizeof(awd_uint32), 1, this->file);
	}
	fflush (this->file);
}
void
AWDFileWriter::writeFLOAT32(awd_float32 val)
{
	if(this->swapBytes){
		awd_float32 swaptVal=awdutil_swapf32(val);
		fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(awd_float32), 1, this->file);
	}
	else{
		fwrite(reinterpret_cast<const char*>(&val), sizeof(awd_float32), 1, this->file);
	}
    fflush (this->file);
}
void
AWDFileWriter::writeFLOAT32multi(awd_float32* val, int length)
{
    fwrite(reinterpret_cast<const char*>(val), sizeof(awd_float32), length, this->file);
	fflush (this->file);
}
void
AWDFileWriter::writeFLOAT64(awd_float64 val)
{
	if(this->swapBytes){
		awd_float64 swaptVal=awdutil_swapf64(val);
		fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(awd_float64), 1, this->file);
	}
	else{
		fwrite(reinterpret_cast<const char*>(&val), sizeof(awd_float64), 1, this->file);
	}
	fflush (this->file);
}
void
AWDFileWriter::writeSTRING(string& s, int prepentInt)
{
	unsigned int N(s.size());
	if(prepentInt==1){
		awd_uint16 awduint16=awd_uint16(N);
		fwrite(&awduint16,sizeof(awd_uint16), 1 ,this->file);
	}
	if(prepentInt==2){
		awd_uint32 awduint32=awd_uint32(N);
		fwrite(&awduint32,sizeof(awd_uint32), 1 ,this->file);
	}
	fwrite(s.c_str(),1, N ,this->file);
	fflush(this->file);
}