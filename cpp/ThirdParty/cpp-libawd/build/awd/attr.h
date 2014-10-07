#ifndef _LIBAWD_ATTR_H
#define _LIBAWD_ATTR_H

#include "ns.h"
#include "awd_types.h"

#define ATTR_RETURN_NULL AWD_field_ptr _ptr; _ptr.v = NULL; return _ptr;

typedef awd_uint16 awd_propkey;

class AWDAttr
{
    protected:
        AWD_field_type type;
        AWD_field_ptr value;
        awd_uint32 value_len;
        int storage_type;// can be 0: storage_precision_properties, 1: storage_precision_Matrix

		virtual void write_metadata(AWDFileWriter*)=0;

    public:
		void write_attr(AWDFileWriter*, BlockSettings *);

        void set_val(AWD_field_ptr, awd_uint32, AWD_field_type, int);
        AWD_field_ptr get_val(awd_uint32 *, AWD_field_type *);
        awd_uint32 get_val_len(BlockSettings *);
};

/**
 * User attributes.
*/
class AWDUserAttr :
    public AWDAttr
{
    private:
        string key;
        AWDNamespace *ns;

    protected:
        void write_metadata(AWDFileWriter*);

    public:
        AWDUserAttr *next;

        AWDUserAttr(AWDNamespace *, string&);
        ~AWDUserAttr();

        AWDNamespace *get_ns();
        string& get_key();
};

class AWDUserAttrList {
    private:
        AWDUserAttr *first_attr;
        AWDUserAttr *last_attr;

        AWDUserAttr *find(AWDNamespace *, string&);

    public:
        AWDUserAttrList();
        ~AWDUserAttrList();

        awd_uint32 calc_length(BlockSettings *);
        void write_attributes(AWDFileWriter*, BlockSettings *);

        AWD_field_ptr get_val_ptr(AWDNamespace *ns, const char *, awd_uint16);
        bool get(AWDNamespace *, string&, AWD_field_ptr *, awd_uint32 *, AWD_field_type *);
        void set(AWDNamespace *, string&, AWD_field_ptr, awd_uint32, AWD_field_type);

        //void add_namespaces(AWD *);
};

/**
 * Numeric attributes ("properties")
*/
class AWDNumAttr :
    public AWDAttr
{
    protected:
        void write_metadata(AWDFileWriter*);

    public:
        awd_propkey key;

        AWDNumAttr *next;

        AWDNumAttr();
        ~AWDNumAttr();
};

class AWDNumAttrList {
    private:
        AWDNumAttr *first_attr;
        AWDNumAttr *last_attr;

        AWDNumAttr *find(awd_propkey);

    public:
        AWDNumAttrList();
        ~AWDNumAttrList();
        awd_uint32 calc_length(BlockSettings *);
        void write_attributes(AWDFileWriter*, BlockSettings *);

        AWD_field_ptr get_val_ptr(awd_propkey);
        bool get(awd_propkey, AWD_field_ptr *, awd_uint32 *, AWD_field_type *);
        void set(awd_propkey, AWD_field_ptr, awd_uint32, AWD_field_type, int=0);
};

/**
 * "Attribute element", any element (such as a block, or parts of block
 * like skeleton joints) which can have attributes.
*/
class AWDAttrElement
{
    protected:
        AWDAttrElement();
        ~AWDAttrElement();

        AWDNumAttrList *properties;
        AWDUserAttrList *user_attributes;

        awd_uint32 calc_attr_length(bool, bool, BlockSettings *);

    public:
        bool get_attr(AWDNamespace *, string&, AWD_field_ptr *, awd_uint32 *, AWD_field_type *);
        void set_attr(AWDNamespace *, string&, AWD_field_ptr, awd_uint32, AWD_field_type);
        void add_color_property(int, awd_uint32, awd_uint32);
        void add_number_property(int, float, float);
        void add_int_property(int, int, int);
        void add_int8_property(int, int, int);
        void add_bool_property(int, bool, bool);
};

#endif
