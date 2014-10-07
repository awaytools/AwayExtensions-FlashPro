#include <cstdio>
#include <string.h>

#include "message.h"
#include "awd.h"

AWDMessageBlock::AWDMessageBlock(string& objName, string& warningMessage) :
    AWDBlock(MESSAGE)
{

    this->message = warningMessage;

    //free(warningMessage);
}

AWDMessageBlock::~AWDMessageBlock() {
	//free this->message ?
}

void
AWDMessageBlock::prepare_and_add_dependencies(AWDBlockList *export_list)
{
}
string
AWDMessageBlock::get_message()
{
    return this->message;
}

awd_uint32
AWDMessageBlock::calc_body_length(BlockSettings * curBlockSettings)
{
    return 0;
}

void
AWDMessageBlock::write_body(AWDFileWriter * fileWriter, BlockSettings *curBlockSettings)
{
}