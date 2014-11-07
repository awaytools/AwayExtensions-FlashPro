#ifndef LIBAWD_AUDIO_H
#define LIBAWD_AUDIO_H

#include "awd_types.h"
#include "block.h"
#include "name.h"
#include "attr.h"

typedef enum {
    EXTERNAL_AUDIO=0,
    EMBEDDED_AUDIO,
    UNDEFINEDTEXTYPE_AUDIO
} AWD_Audio_type;


class AWDAudio :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWD_Audio_type saveType;
        string url;

        awd_uint8 *embed_data;
        awd_uint32 embed_data_len;
        int height;
        int width;
		char* test;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
		void write_body(AWDFileWriter*, BlockSettings *);

    public:
        AWDAudio(string& name);
        ~AWDAudio();

        bool set_embed_data();
		
		int get_embbed_length();
		awd_uint8 * get_embbed_data();
        string get_url();
        awd_uint16 get_url_length();
        void set_url(string& url);

        AWD_Audio_type get_saving_type();
        void set_saving_type(AWD_Audio_type);
        int get_height();
        void set_height(int);
        int get_width();
        void set_width(int);
};

#endif
