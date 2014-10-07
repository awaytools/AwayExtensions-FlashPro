#include <cstring>
#include <ctime>
#include <iostream>
#include "name.h"

AWDNamedElement::AWDNamedElement(string& name)
{
    this->name = name;
}

AWDNamedElement::~AWDNamedElement()
{
	//deallocate the name-string ?
}

string&
AWDNamedElement::get_name()
{
    return this->name;
}
void
AWDNamedElement::set_name(string& name)
{
    this->name = name;
}