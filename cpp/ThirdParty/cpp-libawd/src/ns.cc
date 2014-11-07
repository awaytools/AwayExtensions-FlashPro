#include <stdlib.h>
#include "ns.h"
#include "util.h"
//#include <crtdbg.h>


AWDNamespace::AWDNamespace(string& ns) :
    AWDBlock(NAMESPACE)
{
	this->uri=ns;
    this->handle = 0;
}

AWDNamespace::~AWDNamespace()
{
	// free ns ?
}

awd_nsid
AWDNamespace::get_handle()
{
    return this->handle;
}

void
AWDNamespace::set_handle(awd_nsid handle)
{
    this->handle = handle;
}

string&
AWDNamespace::get_uri()
{
    return this->uri;
}

awd_uint32
AWDNamespace::calc_body_length(BlockSettings * curBlockSetting)
{
    if(!this->get_isValid())
        return 0;
    return sizeof(awd_nsid) + sizeof(awd_uint16) + this->get_uri().size();
}

void
AWDNamespace::write_body(AWDFileWriter * fileWriter, BlockSettings * curBlockSettings)
{
	fileWriter->writeUINT8(this->handle);
    //write(fd, &(this->handle), sizeof(awd_nsid));
	fileWriter->writeSTRING(this->get_uri(), 1);
	//awdutil_write_varstr(fd, this->get_uri().c_str(), this->get_uri().size());
}