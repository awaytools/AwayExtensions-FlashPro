#ifndef _LIBAWD_NAME_H
#define _LIBAWD_NAME_H

//#include <stdlib.h>
#include "awd_types.h"
#include <string>
using namespace std;

class AWDNamedElement
{
    private:
		string name;


    public:
        AWDNamedElement(string& name);
        ~AWDNamedElement();

        string& get_name();
        void set_name(string& name);
};

#endif
