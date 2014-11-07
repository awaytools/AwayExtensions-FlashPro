#ifndef LIBAWD_FONTS_H
#define LIBAWD_FONTS_H

#include "awd_types.h"
#include "block.h"
#include "name.h"
#include "attr.h"
#include <map>

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
        void set_charCode(int);
        bool has_shape_data();
        AWDSubShape2D* get_subShape();
        void set_subShape(AWDSubShape2D*);
        awd_uint32 calc_body_length(BlockSettings *);
		void write_body(AWDFileWriter*, BlockSettings *);
};
class AWDFontStyle
{
    private:
		string style_name;
		int style_size;
		map<int, AWDFontShape*> shapesmap;
		vector<AWDFontShape*> shapes;

    public:
        AWDFontStyle(string&);
        ~AWDFontStyle();
		vector<AWDFontShape*> get_ungenerated_chars();
		void set_style_size(int);
		void delete_fontShape(int char_code);
        string& get_style_name();
        AWDFontShape* get_fontShape(int);
        awd_uint32 calc_body_length(BlockSettings *);
		void write_body(AWDFileWriter*, BlockSettings *);
};

class AWDFont :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
		vector<AWDFontStyle*> font_styles;
		bool from_library;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
		void write_body(AWDFileWriter*, BlockSettings *);

    public:
        AWDFont(string& name);
        ~AWDFont();

        AWDFontStyle* get_font_style(string&);
        vector<AWDFontStyle*> get_font_styles();
};

typedef enum {
    BASELINE_NORMAL=0,
    BASELINE_UP=1,
    BASELINE_DOWN=2
} AWD_baselineshift_type;
class AWDTextFormat:
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
		string fontName;
		string fontStyle;
		int fontSize;
		int letterSpacing;
		AWDBlock* fill;
		AWD_baselineshift_type baseLineShift;
		
    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
		void write_body(AWDFileWriter*, BlockSettings *);
    public:
        AWDTextFormat(string& name);
        ~AWDTextFormat();
        AWDBlock* get_fill();
        void set_fill(AWDBlock*);
        AWD_baselineshift_type get_baseLineShift();
        void set_baseLineShift(AWD_baselineshift_type);
        string& get_fontStyle();
        void set_fontStyle(string&);
        string& get_fontName();
        void set_fontName(string&);
        int get_fontSize();
        void set_fontSize(int);
        int get_letterSpacing();
        void set_letterSpacing(int);
};
class AWDTextRun:
    public AWDAttrElement
{
    private:
		string text;
		AWDTextFormat* textFormat;
		
    public:
        AWDTextRun();
        ~AWDTextRun();
        AWDTextFormat* get_format();
        void set_format(AWDTextFormat*);
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
        void set_isLocalized(bool);
        void set_text(string&);
		void add_paragraph(AWDParagraph*);

};

#endif
