#ifndef _LIBAWD_SHAPE2D_H
#define _LIBAWD_SHAPE2D_H

#include "awd_types.h"
#include "stream.h"
#include "block.h"
#include "name.h"
#include "attr.h"
//#include "anim.h"
#include <vector>
//#include "anim.h"
using namespace std;


#include "poly2tri.h"
using namespace p2t;


class ShapePoint
{
    public:
        ShapePoint();
        ~ShapePoint();
		double x;
		double y;
		double type;
		double curveDataX;
		double curveDataY;
		double red;
		double green;
		double blue;
		double alpha;
};

/**
 * Data stream type
*/
typedef enum {
    VERTICES=1,
    TRIANGLES_INNER=2,
    TRIANGLES_EDGES=3,
    TRIANGLES_CONVEX=4,
    TRIANGLES_CONCAVE=5
} AWD_shape_stream_type;

typedef enum {
    INNER_EDGE=1,
    OUTTER_EDGE=2,
    CONVEX_EDGE=3,
    CONCAVE_EDGE=4,
    CURVED_EDGE=5
} AWD_edge_type;


class AWDPathSegment
{
    private:
		bool subdivide;
		ShapePoint* startPoint;
		ShapePoint* endPoint;
		ShapePoint* controlPoint;
		AWD_edge_type edgeType;
		int hole_idx;
		int originalSegment;
    public:
        AWDPathSegment();
        ~AWDPathSegment();

		
		int get_this_hole_idx();
        void set_hole_idx(int);
        bool get_subdivide();       
        void set_subdivide(bool);
        int get_originalSegment();       
        void set_originalSegment(int);
        ShapePoint* get_startPoint();
        void set_startPoint(ShapePoint*);
        ShapePoint* get_endPoint();
        void set_endPoint(ShapePoint*);
        ShapePoint* get_controlPoint();
        void set_controlPoint(ShapePoint*);
        AWD_edge_type get_edgeType();
        void set_edgeType(AWD_edge_type);

};

class AWDPath
{
    public:
        AWDPath();
        ~AWDPath();
		double x;
		double y;
		double type;
		double curveDataX;
		double curveDataY;
		double red;
		double green;
		double blue;
		double alpha;
};
class AWDSubShape2D :
    public AWDAttrElement
{
    private:
        unsigned int num_streams;
        AWDDataStream * first_stream;
        AWDDataStream * last_stream;
        awd_uint32 calc_streams_length();
		AWDBlock* fill;

    public:
        AWDSubShape2D();
        ~AWDSubShape2D();

        AWDSubShape2D * next;
		
        AWDBlock* get_fill();
        void set_fill(AWDBlock *);
        unsigned int get_num_streams();
        AWDDataStream *get_stream_at(unsigned int);
        void add_stream(AWD_shape_stream_type, AWD_field_type, AWD_str_ptr, awd_uint32);
        void add_uv_transform(awd_float64 *);

        awd_uint32 calc_sub_length(BlockSettings *);
        void write_sub(AWDFileWriter * , BlockSettings *, double);
};

class AWDMergedSubShape
{
    private:
		vector<vector < ShapePoint* > > allSubsVerts; 
		vector<vector < int > > allSubsTris; 
		int vertCnt;
		int triCnt;
		int vertexOffset;
		int lastVertCnt;
		bool isMerged;
		AWDBlock* thisFill;

    public:
        AWDMergedSubShape(AWDBlock*);
        ~AWDMergedSubShape();
		bool isClockWiseXY(double point1x, double point1y, double point2x, double point2y, double point3x, double point3y);
        vector<vector < ShapePoint* > > getAllSubsVerts();
        vector<vector < int > > getAllSubsTris();
		int hasVert(ShapePoint*);
        void checkForRessourceLimits();
		void checkForRessourceLimits_extern(int verts, int faces);
		awd_uint32 calc_sub_length(BlockSettings * blockSettings);
		void write_sub(AWDFileWriter * fileWriter, BlockSettings * blockSettings, double scale);
        void addFillFace(ShapePoint*, ShapePoint*, ShapePoint*);
        void addConvexFace(ShapePoint*, ShapePoint*, ShapePoint*);
        void addConcaveFace(ShapePoint*, ShapePoint*, ShapePoint*);
        void addSubShape(AWDMergedSubShape*);
		void addShapePoint(ShapePoint* point3);
		void addFillFaceIDX(int, int, int);
        AWDBlock* get_fill();
        void set_fill(AWDBlock *);
        void set_isMerged(bool);
		void setColor(double r, double g, double b, double a);
        vector<AWDSubShape2D*> convertToOutputSubShape();
		

};
class AWDShape2D :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        unsigned int num_subs;
        AWDSubShape2D * first_sub;
        AWDSubShape2D * last_sub;
        AWDBlock* fill;
        awd_float64 * bind_mtx;
		vector<AWDMergedSubShape*> mergerSubShapes;


    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(AWDFileWriter * , BlockSettings *curBlockSettings);

    public:
        AWDShape2D(string& name);
        ~AWDShape2D();
		
        void merge_subs();
        unsigned int get_num_subs();
        AWDSubShape2D *get_sub_at(unsigned int);
        void add_sub_mesh(AWDSubShape2D *);
        void add_sub_merger_mesh(AWDMergedSubShape *);
        vector<AWDMergedSubShape*> get_mergerSubShapes();
        AWDMergedSubShape * compare_mergerSubShapes(AWDMergedSubShape *, AWDMergedSubShape *);
		
        AWDBlock* get_fill();
        void set_fill(AWDBlock *);
        AWDSubShape2D *  get_first_sub();
};


#endif
