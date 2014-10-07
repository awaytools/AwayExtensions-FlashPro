#ifndef _LIBAWD_META_H
#define _LIBAWD_META_H

#include "block.h"
#include "attr.h"
#include "awd_types.h"

#define PROP_META_TIMESTAMP 1
#define PROP_META_ENCODER_NAME 2
#define PROP_META_ENCODER_VER 3
#define PROP_META_GENERATOR_NAME 4
#define PROP_META_GENERATOR_VER 5

class AWDMetaData :
    public AWDBlock,
    public AWDAttrElement
{
    private:
        string encoder_name;
        string encoder_version;
        string generator_name;
        string generator_version;
    protected:
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(AWDFileWriter * , BlockSettings *);

    public:
        AWDMetaData();
        ~AWDMetaData();

        void override_encoder_metadata(string&, string&);
        void set_generator_metadata(string&, string&);
};

#endif
