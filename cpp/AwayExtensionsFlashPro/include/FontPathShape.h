#ifndef FONTPATHSHAPE_H_
#define FONTPATHSHAPE_H_

#include "awd.h"
#include "Utils.h"
#include "Service/Shape/IPath.h"

class FontPathShape 
{
    private:
		int pathID;
		int parentID;
		vector<AWDPathSegment*> main_path;
		vector<FontPathShape*> hole_paths;
		vector<int> tested_pathes;
		vector<bool> tested_pathes_results;
		double startX;
		double startY;
		double endX;
		double endY;
        FCM::PIFCMCallback m_pCallback;

    public:
        FontPathShape(FCM::PIFCMCallback*, vector<AWDPathSegment*>, int);
        ~FontPathShape();

		vector<AWDPathSegment*> get_main_path();
		void set_main_path(vector<AWDPathSegment*>);

		void add_hole(FontPathShape*);
		vector<FontPathShape*> get_holes();
		
        SimplePoint * line_intersection_point(SimplePoint *, SimplePoint *, SimplePoint *, SimplePoint *);		
		bool test_bounding_box_lines(SimplePoint * a1, SimplePoint* a2, SimplePoint * b1, SimplePoint * b2);
		vector<vector<AWDPathSegment*> > remove_intersecting();
		int tested_aginst(int);
		void set_test_result(int);
		int get_pathID();
		void set_pathID(int);
		int get_parentID();
		void set_parentID(int);
		double get_startX();
		void set_startX(double startX);
		double get_startY();
		void set_startY(double startY);
		double get_endX();
		void set_endX(double endX);
		double get_endY();
		void set_endY(double endY);
		
		
};
#endif