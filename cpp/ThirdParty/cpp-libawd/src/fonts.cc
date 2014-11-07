#include "util.h"
#include "fonts.h"
#include <sys/stat.h>
#include "util.h"
#include "audio.h"
#include <sys/stat.h>

AWDTextFormat::AWDTextFormat(string& name) :
    AWDBlock(FORMAT),
    AWDNamedElement(name),
    AWDAttrElement()
{
	this->baseLineShift=BASELINE_NORMAL;
	this->fontSize=0;
	this->letterSpacing=0;
	this->fill=NULL;
}

AWDTextFormat::~AWDTextFormat()
{
	//free the url ?
}
AWD_baselineshift_type AWDTextFormat::get_baseLineShift()
{
	return this->baseLineShift;
}
void AWDTextFormat::set_baseLineShift(AWD_baselineshift_type baseLineShift)
{
	this->baseLineShift=baseLineShift;
}
string& AWDTextFormat::get_fontStyle()
{
	return this->fontStyle;
}
void AWDTextFormat::set_fontStyle(string& fontStyle)
{
	this->fontStyle=fontStyle;
}

string& AWDTextFormat::get_fontName()
{
	return this->fontName;
}
void AWDTextFormat::set_fontName(string& fontName)
{
	this->fontName=fontName;
}

int AWDTextFormat::get_fontSize()
{
	return this->fontSize;
}
void AWDTextFormat::set_fontSize(int fontSize)
{
	this->fontSize=fontSize;
}
int AWDTextFormat::get_letterSpacing()
{
	return this->letterSpacing;
}
void AWDTextFormat::set_letterSpacing(int letterSpacing)
{
	this->letterSpacing=letterSpacing;
}

AWDBlock* AWDTextFormat::get_fill()
{
	return this->fill;
}
void AWDTextFormat::set_fill(AWDBlock* fill)
{
	this->fill=fill;
}


awd_uint32
AWDTextFormat::calc_body_length(BlockSettings * curBlockSettings)
{
    awd_uint32 len;

    len = 0;//sizeof(awd_uint32); //datalength;
    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}


void
 AWDTextFormat::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    // Do nothing
}

void
AWDTextFormat::write_body(AWDFileWriter * fileWriter, BlockSettings * curBlockSettings)
{
    awd_uint32 data_len;
    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
}
AWDTextRun::AWDTextRun() :
    AWDAttrElement()
{
}

AWDTextRun::~AWDTextRun()
{
	//free the url ?
}
AWDTextFormat* AWDTextRun::get_format()
{
	return this->textFormat;
}
void AWDTextRun::set_format(AWDTextFormat* textFormat)
{
	this->textFormat=textFormat;
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
	shape_data=false;
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
	if(subShape!=NULL){
		this->subShape=subShape;
		this->shape_data=true;
	}
}
		
awd_uint32 AWDFontShape::calc_body_length(BlockSettings*) 
{
	return 0;
}
void AWDFontShape::write_body(AWDFileWriter*, BlockSettings*) 
{
}

AWDFontStyle::AWDFontStyle(string& name) 
{
	this->style_size=0.0;
	this->style_name=name;
}
string& AWDFontStyle::get_style_name() 
{
	return this->style_name;
}

AWDFontStyle::~AWDFontStyle()
{
	//free the url ?
}

void AWDFontStyle::set_style_size(int style_size) 
{
	this->style_size=style_size;
}
vector<AWDFontShape*> AWDFontStyle::get_ungenerated_chars()
{
	vector<AWDFontShape*> returner;
	typedef std::map<int, AWDFontShape*>::iterator it_type;
	for(it_type iterator = shapesmap.begin(); iterator != shapesmap.end(); iterator++) {
		// iterator->first = key
		if(!iterator->second->has_shape_data()){	
			returner.push_back(iterator->second);
		}
	}
	return returner;
}
void AWDFontStyle::delete_fontShape(int char_code){
	if(shapesmap.find(char_code) == shapesmap.end())
	{
		return;
	}
	AWDFontShape* newFS=shapesmap[char_code];
	shapesmap.erase(char_code);
	delete newFS;
}
AWDFontShape* AWDFontStyle::get_fontShape(int char_code) 
{
	if(char_code==32){
		return NULL;
	}
	if(shapesmap.find(char_code) == shapesmap.end())
	{
		AWDFontShape* newFS = new AWDFontShape();
		newFS->set_charCode(char_code);
		shapesmap[char_code]=newFS;
		return newFS;
	}
	return shapesmap[char_code];
}
awd_uint32 AWDFontStyle::calc_body_length(BlockSettings*) 
{
	return 0;
}
void AWDFontStyle::write_body(AWDFileWriter*, BlockSettings*) 
{
}

AWDFont::AWDFont(string& name) :
    AWDBlock(FONT),
    AWDNamedElement(name),
    AWDAttrElement()
{
}

AWDFont::~AWDFont()
{
	//free the url ?
}

AWDFontStyle* AWDFont::get_font_style(string& fontstyleName)
{
	for(AWDFontStyle* font_style: font_styles){
		if(font_style->get_style_name()==fontstyleName){
			return font_style;
		}
	}
	font_styles.push_back(new AWDFontStyle(fontstyleName));
	return font_styles.back();
}


vector<AWDFontStyle*> AWDFont::get_font_styles()
{
	return font_styles;
}
awd_uint32
AWDFont::calc_body_length(BlockSettings * curBlockSettings)
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
 AWDFont::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    // Do nothing
}



void
AWDFont::write_body(AWDFileWriter * fileWriter, BlockSettings * curBlockSettings)
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


void
AWDTextElement::set_isLocalized(bool isLocalized)
{
	this->isLocalized=isLocalized;
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