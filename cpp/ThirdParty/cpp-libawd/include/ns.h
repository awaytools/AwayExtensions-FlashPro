#ifndef _LIBAWD_NS_H
#define _LIBAWD_NS_H

#include "block.h"
#include "awd_types.h"

#include "FileWriter.h"
#include <string>
using namespace std;
class AWDNamespace :
    public AWDBlock
{
    private:
        string uri;
        awd_nsid handle;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(AWDFileWriter*, BlockSettings *);

    public:
        AWDNamespace(string& ns);
        ~AWDNamespace();

        string& get_uri();

        awd_nsid get_handle();
        void set_handle(awd_nsid handle);
};

#endif
