#include <cstdio>
#include <string.h>

#include "meta.h"
#include "awd.h"

AWDMetaData::AWDMetaData() :
    AWDBlock(METADATA),
    AWDAttrElement()
{
    this->encoder_name = "libawd";
	this->encoder_version = to_string(AWD::VERSION_MAJOR) + "." + to_string(AWD::VERSION_MINOR) + "." + to_string(AWD::VERSION_BUILD) + "." + AWD::VERSION_RELEASE;

}

AWDMetaData::~AWDMetaData() {
}
void
AWDMetaData::override_encoder_metadata(string& name, string& version)
{
    this->encoder_name = name;
    this->encoder_version = version;
}
void
AWDMetaData::set_generator_metadata(string& name, string& version)
{
    this->generator_name = name;
    this->generator_version = version;
}

void
AWDMetaData::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    AWD_field_ptr val;

	val.str = (char*)this->encoder_name.c_str();
    this->properties->set(PROP_META_ENCODER_NAME, val, (awd_uint32)this->encoder_name.size(), AWD_FIELD_STRING);

    AWD_field_ptr val2;
    val2.str = (char*)this->encoder_version.c_str();
    this->properties->set(PROP_META_ENCODER_VER, val2, (awd_uint32)this->encoder_version.size(), AWD_FIELD_STRING);

    AWD_field_ptr val3;
    val3.str = (char*)this->generator_name.c_str();
    this->properties->set(PROP_META_GENERATOR_NAME, val3, (awd_uint32)this->generator_name.size(), AWD_FIELD_STRING);
    
    AWD_field_ptr val4;
    val4.str = (char*)this->generator_version.c_str();
    this->properties->set(PROP_META_GENERATOR_VER, val4, (awd_uint32)this->generator_version.size(), AWD_FIELD_STRING);
    
}

awd_uint32
AWDMetaData::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    return this->calc_attr_length(true, false, curBlockSettings);
}

void
AWDMetaData::write_body(AWDFileWriter * fileWriter, BlockSettings *curBlockSettings)
{
    this->properties->write_attributes(fileWriter, curBlockSettings);
}