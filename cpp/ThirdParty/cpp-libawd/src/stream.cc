#include <stdlib.h>

#include "shape2D.h"
#include "stream.h"
#include "awd_types.h"
#include "util.h"


AWDDataStream::AWDDataStream(awd_uint8 type, AWD_field_type data_type, AWD_str_ptr data, awd_uint32 num_elements)
{
    this->type = type;
    this->data = data;
    this->data_type = data_type;
    this->num_elements = num_elements;
    this->next = NULL;
}

AWDDataStream::~AWDDataStream()
{
    free(this->data.v);
    this->num_elements = 0;
}

awd_uint32
AWDDataStream::get_num_elements()
{
    return this->num_elements;
}

awd_uint32
AWDDataStream::get_length()
{
    awd_uint32 elem_size;
    elem_size = (awd_uint32)awdutil_get_type_size(this->data_type, false);
    return (this->num_elements * elem_size);
}

void
AWDDataStream::write_stream(AWDFileWriter * fileWriter, double scale)
{
    unsigned int e;
    awd_uint32 num;
    awd_uint32 str_len;

    str_len = this->get_length();
	fileWriter->writeUINT8(this->type);
	fileWriter->writeUINT8(this->data_type);
	fileWriter->writeUINT32(str_len);
    //write(fd, (awd_uint8*)&this->type, sizeof(awd_uint8));
   // write(fd, (awd_uint8*)&this->data_type, sizeof(awd_uint8));
    //write(fd, &str_len, sizeof(awd_uint32));
    if ((this->type!=VERTICES))
        scale = 1;
    num = this->num_elements;

    // Encode according to data type field
    if (this->data_type == AWD_FIELD_INT8) {
        for (e=0; e<num; e++) {
            awd_int32 *p = (this->data.i32 + e);
            awd_int8 elem = (awd_int8)*p;
           // write(fd, &elem, sizeof(awd_int8));
			fileWriter->writeUINT8(elem);
        }
    }
    else if (this->data_type == AWD_FIELD_INT16) {
        for (e=0; e<num; e++) {
            awd_int32 *p = (this->data.i32 + e);
            awd_int16 elem = (awd_int16)*p;
            //write(fd, &elem, sizeof(awd_int16));
			fileWriter->writeUINT16(elem);
        }
    }
    else if (this->data_type == AWD_FIELD_INT32) {
        for (e=0; e<num; e++) {
            awd_int32 *p = (this->data.i32 + e);
            awd_int32 elem = (awd_int32)*p;
            //write(fd, &elem, sizeof(awd_int32));
			fileWriter->writeUINT32(elem);
        }
    }
    else if (this->data_type == AWD_FIELD_UINT8) {
        for (e=0; e<num; e++) {
            awd_uint32 *p = (this->data.ui32 + e);
            awd_uint8 elem = (awd_uint8)*p;
            //write(fd, &elem, sizeof(awd_uint8));
			fileWriter->writeUINT8(elem);
        }
    }
    else if (this->data_type == AWD_FIELD_UINT16) {
        for (e=0; e<num; e++) {
			awd_uint16 *pf = (awd_uint16*)malloc(sizeof(awd_uint16)*num);				
            for (e=0; e<num; e++) {
				awd_uint32 *p = (this->data.ui32 + e);
                awd_uint16 elem = ((awd_uint16)*p);
				pf[e]=elem;
            }
			fileWriter->writeUINT16multi((awd_uint16*)pf, num);
			free(pf);
        }
    }
    else if (this->data_type == AWD_FIELD_UINT32) {
        for (e=0; e<num; e++) {
            awd_uint32 *p = (this->data.ui32 + e);
            awd_uint32 elem = awd_uint32((awd_uint32)*p);
            //write(fd, &elem, sizeof(awd_uint32));
			fileWriter->writeUINT32(elem);
        }
    }
	else if (this->type==VERTICES){
        if (scale!=1.0){
            if (this->data_type == AWD_FIELD_FLOAT32) {
				//fileWriter->writeFLOAT32multi(*this->data.f32, num);
                /*for (e=0; e<num; e++) {
                    awd_float64 *p = (this->data.f64 + e);
                    awd_float32 elem = awd_float32((awd_float32)*p*scale);
                    //write(fd, &elem, sizeof(awd_float32));
					fileWriter->writeFLOAT32(elem);
                }*/
            }
            else if (this->data_type == AWD_FIELD_FLOAT64) {
                for (e=0; e<num; e++) {
                    awd_float64 *p = (this->data.f64 + e);
                    awd_float64 elem = awd_float64((awd_float64)*p*scale);
                    //write(fd, &elem, sizeof(awd_float64));
					fileWriter->writeFLOAT64(elem);
                }
            }
        }
        else if (scale==1.0){
            if (this->data_type == AWD_FIELD_FLOAT32) {
				
				awd_float32 *pf = (awd_float32*)malloc(sizeof(awd_float32)*num);
				
                for (e=0; e<num; e++) {
                    awd_float64 *p = (this->data.f64 + e);
                    awd_float32 elem = ((awd_float32)*p);
					pf[e]=elem;
                }
				fileWriter->writeFLOAT32multi((awd_float32*)pf, num);
				free(pf);
            }
            else if (this->data_type == AWD_FIELD_FLOAT64) {
                for (e=0; e<num; e++) {
                    awd_float64 *p = (this->data.f64 + e);
                    awd_float64 elem = ((awd_float64)*p);
					//write(fd, &elem, sizeof(awd_float64));
					fileWriter->writeFLOAT64(elem);
                }
            }
        }
    }/*
    else if (this->type==ALLVERTDATA){
        if (scale!=1.0){
            for (e=0; e<num; e++) {
                awd_float64 *p = (this->data.f64 + e);
                awd_float32 elem;
                if (e<3)
                    elem = awd_float32((awd_float32)*p*scale);
                else
                    elem = awd_float32((awd_float32)*p);
                //write(fd, &elem, sizeof(awd_float32));
				fileWriter->writeFLOAT32(elem);
            }
        }
        else if (scale==1.0){
            for (e=0; e<num; e++) {
                awd_float64 *p = (this->data.f64 + e);
                awd_float32 elem;
                elem = awd_float32((awd_float32)*p);
               // write(fd, &elem, sizeof(awd_float32));
				fileWriter->writeFLOAT64(elem);
            }
        }
    }
	*/
}


AWDGeomDataStream::AWDGeomDataStream(awd_uint8 type, AWD_field_type data_type, AWD_str_ptr data, awd_uint32 num_elements)
    : AWDDataStream(type, data_type, data, num_elements)
{}

AWDPathDataStream::AWDPathDataStream(awd_uint8 type, AWD_field_type data_type, AWD_str_ptr data, awd_uint32 num_elements)
    : AWDDataStream(type, data_type, data, num_elements)
{}