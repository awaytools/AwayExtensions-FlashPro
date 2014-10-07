#include "util.h"
#include "fonts.h"
#include <sys/stat.h>
#include "util.h"
#include "audio.h"
#include <sys/stat.h>

AWDTextRun::AWDTextRun() :
    AWDAttrElement()
{
}

AWDTextRun::~AWDTextRun()
{
	//free the url ?
}
string& AWDTextRun::get_text()
{
	return this->text;
}
void AWDTextRun::set_text(string& text)
{
	this->text=text;
}
awd_uint32
AWDTextRun::calc_body_length(BlockSettings * curBlockSettings)
{
    awd_uint32 len;

    len = 0;//sizeof(awd_uint32); //datalength;
    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}

void
AWDTextRun::write_body(AWDFileWriter * fileWriter, BlockSettings * curBlockSettings)
{
    awd_uint32 data_len;
    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
}

AWDParagraph::AWDParagraph() :
    AWDAttrElement()
{
}

AWDParagraph::~AWDParagraph()
{
	//free the url ?
}
void AWDParagraph::add_textrun(AWDTextRun* textRun)
{
	this->textRuns.push_back(textRun);
}


awd_uint32
AWDParagraph::calc_body_length(BlockSettings * curBlockSettings)
{
    awd_uint32 len;

    len = 0;//sizeof(awd_uint32); //datalength;
	len+=sizeof(awd_uint32);
	for(AWDTextRun* tr : this->textRuns){
		len+=tr->calc_body_length(curBlockSettings);
	}
    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}



void
AWDParagraph::write_body(AWDFileWriter * fileWriter, BlockSettings * curBlockSettings)
{
    awd_uint32 data_len;
	fileWriter->writeUINT32(this->textRuns.size());
	for(AWDTextRun* tr : this->textRuns){
		tr->write_body(fileWriter, curBlockSettings);
	}
    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
}


AWDFontShape::AWDFontShape() 
{
}

AWDFontShape::~AWDFontShape()
{
	//free the url ?
}
    

int AWDFontShape::get_charCode() 
{
	return this->charCode;
}
void AWDFontShape::set_charCode(int charCode) 
{
	this->charCode=charCode;
}
bool AWDFontShape::has_shape_data() 
{
	return this->shape_data;
}
AWDSubShape2D* AWDFontShape::get_subShape() 
{
	return this->subShape;
}
void AWDFontShape::set_subShape(AWDSubShape2D* subShape) 
{
	this->subShape=subShape;
}
		
awd_uint32 AWDFontShape::calc_body_length(BlockSettings*) 
{
	return 0;
}
void AWDFontShape::write_body(AWDFileWriter*, BlockSettings*) 
{
}

AWDFontStyle::AWDFontStyle() 
{
}

AWDFontStyle::~AWDFontStyle()
{
	//free the url ?
}

AWDFontShape* AWDFontStyle::get_fontShape(int char_code) 
{
	for(AWDFontShape* fs : shapes){
		if(fs->get_charCode()==char_code){
			return fs;
		}
	}
	AWDFontShape* newFS = new AWDFontShape();
	newFS->set_charCode(char_code);
	return newFS;
}
awd_uint32 AWDFontStyle::calc_body_length(BlockSettings*) 
{
	return 0;
}
void AWDFontStyle::write_body(AWDFileWriter*, BlockSettings*) 
{
}

AWDFontShapes::AWDFontShapes(string& name) :
    AWDBlock(FONT),
    AWDNamedElement(name),
    AWDAttrElement()
{
}

AWDFontShapes::~AWDFontShapes()
{
	//free the url ?
}



awd_uint32
AWDFontShapes::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;

    len = 0;//sizeof(awd_uint32); //datalength;
	//len += sizeof(awd_uint16) + this->get_name().size(); //name
    //len += sizeof(awd_uint8); //save type (external/embbed)


    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}


void
    AWDFontShapes::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    // Do nothing
}



void
AWDFontShapes::write_body(AWDFileWriter * fileWriter, BlockSettings * curBlockSettings)
{
    awd_uint32 data_len;

	//awdutil_write_varstr(fd, this->get_name().c_str(), this->get_name().size());


    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
}

AWDTextElement::AWDTextElement(string& name) :
    AWDBlock(TEXT_ELEMENT),
    AWDNamedElement(name),
    AWDAttrElement()
{
	this->isLocalized=false;
}

AWDTextElement::~AWDTextElement()
{
}


bool
AWDTextElement::get_isLocalized()
{
	return this->isLocalized;
}
void
AWDTextElement::set_text(string& text)
{
	this->text=text;
}
string&
AWDTextElement::get_text()
{
	return this->text;
}


void AWDTextElement::add_paragraph(AWDParagraph* paragraph)
{
	this->textParagraphs.push_back(paragraph);
}



void
    AWDTextElement::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    // Do nothing
}

awd_uint32
AWDTextElement::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;

    len = 0;//sizeof(awd_uint32); //datalength;
	//len += sizeof(awd_uint16) + this->get_name().size(); //name
   // len += sizeof(awd_uint8); //save type (external/embbed)
	len+=1; //isLocalized
	len += sizeof(awd_uint16) + this->get_name().size(); //name
	if(!this->isLocalized){
		len+=sizeof(awd_uint32);
		for(AWDParagraph* p : this->textParagraphs){
			len+=p->calc_body_length(curBlockSettings);
		}
	}
 
    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}



void
AWDTextElement::write_body(AWDFileWriter * fileWriter, BlockSettings * curBlockSettings)
{
    awd_uint32 data_len;
	
	fileWriter->writeBOOL(this->isLocalized);
	fileWriter->writeSTRING(this->get_name(), 1);// name	
	if(!this->isLocalized){
		fileWriter->writeUINT32(this->textParagraphs.size());
		for(AWDParagraph* p : this->textParagraphs){
			p->write_body(fileWriter, curBlockSettings);
		}
	}
    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
}