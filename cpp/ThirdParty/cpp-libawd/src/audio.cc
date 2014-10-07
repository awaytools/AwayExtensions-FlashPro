#include "util.h"
#include "audio.h"
#include <sys/stat.h>


AWDAudio::AWDAudio(string& name) :
    AWDBlock(SOUND_SOURCE),
    AWDNamedElement(name),
    AWDAttrElement()
{
    this->saveType = EXTERNAL_AUDIO;
    this->embed_data = NULL;
    this->embed_data_len = 0;
    this->height = 0;
    this->width = 0;
}

AWDAudio::~AWDAudio()
{
    if (this->embed_data_len>0) free(this->embed_data);
    this->embed_data = NULL;
    this->embed_data_len = 0;
	//free the url ?
}

int
AWDAudio::get_width()
{
    return this->width;
}

void
AWDAudio::set_width(int width)
{
    this->width=width;
}
int
AWDAudio::get_height()
{
    return this->height;
}
void
AWDAudio::set_height(int height)
{
    this->height=height;
}
string
AWDAudio::get_url()
{
    return this->url;
}


void
AWDAudio::set_url(string& url)
{
    this->url = url;
}

AWD_Audio_type
AWDAudio::get_saving_type()
{
    return this->saveType;
}
void
AWDAudio::set_saving_type(AWD_Audio_type texType)
{
    this->saveType = texType;
}

bool
AWDAudio::set_embed_data()
{
	if(this->url.empty()){
		return false;
	}
	
	size_t result;

	FILE * tmp_file = fopen (this->url.c_str(), "rb");
	//fread(this->embed_data, tmp_file->_bufsiz, tmp_file->_bufsiz, tmp_file);
    //this->embed_data_len = tmp_file->_bufsiz;
	// obtain file size:
	fseek (tmp_file , 0 , SEEK_END);
	this->embed_data_len = ftell (tmp_file);
	rewind (tmp_file);

	// allocate memory to contain the whole file:
	this->embed_data = (awd_uint8*) malloc (sizeof(awd_uint8)*this->embed_data_len);
	//if (this->embed_data == NULL) {fputs ("Memory error",stderr); exit (2);}
	// copy the file into the buffer:
	result = fread (this->embed_data,sizeof(awd_uint8),this->embed_data_len,tmp_file);
	//if (result != this->embed_data_len) {fputs ("Reading error",stderr); exit (3);}

	/* the whole file is now loaded in the memory buffer. */

	  // terminate
	fclose (tmp_file);
	return true;
}

void
    AWDAudio::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    // Do nothing
}


awd_uint32
AWDAudio::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;

    len = sizeof(awd_uint32); //datalength;
	len += sizeof(awd_uint16) + this->get_name().size(); //name
    len += sizeof(awd_uint8); //save type (external/embbed)

	if (this->saveType == EXTERNAL_AUDIO) {
        len +=awd_uint16(this->url.size());
    }
    else {
        len += awd_uint16(this->embed_data_len);
	}

    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}


void
AWDAudio::write_body(AWDFileWriter * fileWriter, BlockSettings * curBlockSettings)
{
    awd_uint32 data_len;
	
	fileWriter->writeSTRING( this->get_name(), 1);
	fileWriter->writeUINT8( this->saveType);
	//awdutil_write_varstr(fd, this->get_name().c_str(), this->get_name().size());

    //write(fd, &this->saveType, sizeof(awd_uint8));
	if (this->saveType == EXTERNAL_AUDIO) {
		//fileWriter->writeUINT32(data_len);// 
        //write(fd, &data_len, sizeof(awd_uint32));
        //write(fd, this->url.c_str(), this->url.size());
		fileWriter->writeSTRING(this->url, 2);// frame code	
    }
    else {
		fileWriter->writeUINT32(sizeof(this->embed_data));// 
		awd_uint8* thisEmbeddData=this->embed_data;
		fileWriter->writeBytes(this->embed_data, this->embed_data_len);// frame code	
		//fwrite(reinterpret_cast<const char*>(thisEmbeddData), 1,  this->embed_data_len, fileWriter->get_file());		
    }

    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
}