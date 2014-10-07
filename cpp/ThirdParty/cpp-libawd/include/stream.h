#ifndef _LIBAWD_STREAM_H
#define _LIBAWD_STREAM_H

#include "awd_types.h"

#include "FileWriter.h"
/** 
 * Data stream pointer
*/
typedef union {
    void *v;
    awd_int32 *i32;
    awd_uint32 *ui32;
    awd_float32 *f32;
    awd_float64 *f64;
} AWD_str_ptr;



class AWDDataStream
{
    private:
        awd_uint32 num_elements;

    public:
        awd_uint8 type;
        AWD_field_type data_type;
        AWD_str_ptr data;

        AWDDataStream * next;
        
        AWDDataStream(awd_uint8, AWD_field_type, AWD_str_ptr, awd_uint32);
        ~AWDDataStream();

        awd_uint32 get_num_elements();
        awd_uint32 get_length();
        void write_stream(AWDFileWriter * , double);
};



class AWDGeomDataStream : public AWDDataStream
{
    public:
        AWDGeomDataStream(awd_uint8, AWD_field_type, AWD_str_ptr, awd_uint32);
};


class AWDPathDataStream : public AWDDataStream
{
    public:
        AWDPathDataStream(awd_uint8, AWD_field_type, AWD_str_ptr, awd_uint32);
};



#endif
