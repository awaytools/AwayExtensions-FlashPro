#ifndef LIBAWD_FONTS_H
#define LIBAWD_FONTS_H

#include "awd_types.h"
#include "block.h"
#include "name.h"
#include "attr.h"

class AWDFontShape
{
    private:
		int charCode;
		bool shape_data;
		AWDSubShape2D* subShape;

    public:
        AWDFontShape();
        ~AWDFontShape();
        int get_charCode();
        bool has_shape_data();
        void set_charCode(int);
        AWDSubShape2D* get_subShape();
        void set_subShape(AWDSubShape2D*);
        awd_uint32 calc_body_length(BlockSettings *);
		void write_body(AWDFileWriter*, BlockSettings *);
};
class AWDFontStyle
{
    private:
		string style_name;
		vector<AWDFontShape*> shapes;

    public:
        AWDFontStyle();
        ~AWDFontStyle();
        AWDFontShape* get_fontShape(int);
        awd_uint32 calc_body_length(BlockSettings *);
		void write_body(AWDFileWriter*, BlockSettings *);
};

class AWDFontShapes :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
		vector<AWDFontStyle*> font_styles;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
		void write_body(AWDFileWriter*, BlockSettings *);

    public:
        AWDFontShapes(string& name);
        ~AWDFontShapes();

        void get_font_style(AWDFontStyle*);
};

class AWDTextRun:
    public AWDAttrElement
{
    private:
		string text;


    public:
        AWDTextRun();
        ~AWDTextRun();
        string& get_text();
        void set_text(string&);
        awd_uint32 calc_body_length(BlockSettings *);
		void write_body(AWDFileWriter*, BlockSettings *);
};
class AWDParagraph:
	public AWDAttrElement
{
    private:
		vector<AWDTextRun*> textRuns;

    public:
        AWDParagraph();
        ~AWDParagraph();
		void add_textrun(AWDTextRun*);
        awd_uint32 calc_body_length(BlockSettings *);
		void write_body(AWDFileWriter*, BlockSettings *);
};
class AWDTextElement :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
		vector<AWDParagraph*> textParagraphs;
		string text;
		bool isLocalized;
		
    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
		void write_body(AWDFileWriter*, BlockSettings *);

    public:
        AWDTextElement(string& name);
        ~AWDTextElement();
		
        string& get_text();
        bool get_isLocalized();
        void set_text(string&);
		void add_paragraph(AWDParagraph*);

};

#endif
