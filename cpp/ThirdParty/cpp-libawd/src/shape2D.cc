#include <stdlib.h>
#include <cstdio>
#include <limits>

#include "math.h"
#include "shape2D.h"
#include "util.h"
#include <vector>  
ShapePoint::ShapePoint() 
{
		this->x=0.0;
		this->y=0.0;
		this->type=0.0;
		this->curveDataX=0.0;
		this->curveDataY=0.0;
		this->red=0.0;
		this->green=0.0;
		this->blue=0.0;
		this->alpha=0.0;
}
ShapePoint::~ShapePoint() 
{
}
static int RESSOURCE_LIMIT = 0xffff;
AWDPathSegment::AWDPathSegment() 
{
    this->subdivide = false;
    this->startPoint = NULL;
    this->endPoint = NULL;
    this->controlPoint = NULL;
	this->edgeType=OUTTER_EDGE;
	this->hole_idx=0;
	this->originalSegment=0;
}
AWDPathSegment::~AWDPathSegment()
{
	delete this->startPoint;
	delete this->endPoint;
	delete this->controlPoint;	
}
ShapePoint*
AWDPathSegment::get_startPoint(){
	return this->startPoint;
}
void
AWDPathSegment::set_startPoint(ShapePoint* startPoint)
{
	this->startPoint=startPoint;
}
ShapePoint*
AWDPathSegment::get_endPoint(){
	return this->endPoint;
}
void
AWDPathSegment::set_endPoint(ShapePoint* endPoint)
{
	this->endPoint=endPoint;
}
ShapePoint* 
AWDPathSegment::get_controlPoint(){
	return this->controlPoint;
}
void
AWDPathSegment::set_controlPoint(ShapePoint* controlPoint)
{
	this->controlPoint=controlPoint;
}
AWD_edge_type 
AWDPathSegment::get_edgeType(){
	return this->edgeType;
}
void
AWDPathSegment::set_edgeType(AWD_edge_type edgeType)
{
	this->edgeType=edgeType;
}

int
AWDPathSegment::get_this_hole_idx(){
	return this->hole_idx;
}
void
AWDPathSegment::set_hole_idx(int hole_idx)
{
	this->hole_idx=hole_idx;
}
bool
AWDPathSegment::get_subdivide(){
	return this->subdivide;
}
void
AWDPathSegment::set_subdivide(bool subdivide)
{
	this->subdivide=subdivide;
}
int
AWDPathSegment::get_originalSegment(){
	return this->originalSegment;
}
void
AWDPathSegment::set_originalSegment(int originalSegment)
{
	this->originalSegment=originalSegment;
}


AWDMergedSubShape::AWDMergedSubShape(AWDBlock* thisFill)
{
	vector < ShapePoint* > newPoints;
	vector < int > newTris;
	this->isMerged=false;
	allSubsVerts.push_back(newPoints);
	allSubsTris.push_back(newTris);
	this->thisFill=thisFill;
}
AWDMergedSubShape::~AWDMergedSubShape()
{
	if(!this->isMerged){
		for (vector < ShapePoint* > pointList : allSubsVerts){
			for (ShapePoint* point : pointList){
				delete point;
			}
		}
	}

}

void
	AWDMergedSubShape::set_isMerged(bool isMerged)
{
	this->isMerged=isMerged;

}
void
AWDMergedSubShape::setColor(double r, double g, double b, double a)
{
	for(vector < ShapePoint* > pointList : allSubsVerts){
		for ( ShapePoint*  point : pointList){
			point->red=r;
			point->green=g;
			point->blue=b;
			point->alpha=a;
		}
	}

}
void
AWDMergedSubShape::checkForRessourceLimits_extern(int verts, int faces)
{
	if((((allSubsVerts.back().size()+verts)*9)>=(3*RESSOURCE_LIMIT))||(((allSubsTris.back().size()+faces))>=(15*RESSOURCE_LIMIT))){
		vector < ShapePoint* > newPoints;
		vector < int > newTris;
		allSubsVerts.push_back(newPoints);
		allSubsTris.push_back(newTris);
	}
}
void
AWDMergedSubShape::checkForRessourceLimits()
{
	if((((allSubsVerts.back().size()+3)*9)>=(3*RESSOURCE_LIMIT))||(((allSubsTris.back().size()+3))>=(15*RESSOURCE_LIMIT))){
		vector < ShapePoint* > newPoints;
		vector < int > newTris;
		allSubsVerts.push_back(newPoints);
		allSubsTris.push_back(newTris);
	}
}
int AWDMergedSubShape::hasVert(ShapePoint* vert)
{
	int vertCnt=0;
	for(vertCnt=this->lastVertCnt; vertCnt<allSubsVerts.back().size(); vertCnt++)
	{
		ShapePoint* oneVert = allSubsVerts.back()[vertCnt];
		if((vert->x==oneVert->x)&&(vert->y==oneVert->y)){
			delete vert; 
			return vertCnt;
		}
	}
	return -1;
}
void
AWDMergedSubShape::addShapePoint(ShapePoint* point3)
{
	allSubsVerts.back().push_back(point3);	
}
void
AWDMergedSubShape::addFillFaceIDX(int point1, int point2, int point3)
{
	allSubsTris.back().push_back(point1);	
	allSubsTris.back().push_back(point2);
	allSubsTris.back().push_back(point3);
}
void
AWDMergedSubShape::addFillFace(ShapePoint* point1, ShapePoint* point2, ShapePoint* point3)
{
	checkForRessourceLimits();
	int verIdx;
	if (isClockWiseXY(point1->x, point1->y, point2->x, point2->y, point3->x, point3->y)){
		verIdx=hasVert(point1);
		if(verIdx<0){
			point1->type=0.0;
			point1->curveDataX=0.0;
			point1->curveDataY=0.0;
			allSubsTris.back().push_back(allSubsVerts.back().size());	
			allSubsVerts.back().push_back(point1);	
		}
		else{
			allSubsTris.back().push_back(verIdx);	
		}
		verIdx=hasVert(point2);
		if(verIdx<0){
			point2->type=0.0;
			point2->curveDataX=0.0;
			point2->curveDataY=0.0;
			allSubsTris.back().push_back(allSubsVerts.back().size());	
			allSubsVerts.back().push_back(point2);	
		}
		else{
			allSubsTris.back().push_back(verIdx);	
		}
		verIdx=hasVert(point3);
		if(verIdx<0){
			point3->type=0.0;
			point3->curveDataX=0.0;
			point3->curveDataY=0.0;
			allSubsTris.back().push_back(allSubsVerts.back().size());	
			allSubsVerts.back().push_back(point3);	
		}
		else{
			allSubsTris.back().push_back(verIdx);	
		}
	}
	else{
		verIdx=hasVert(point3);
		if(verIdx<0){
			point3->type=0.0;
			point3->curveDataX=0.0;
			point3->curveDataY=0.0;
			allSubsTris.back().push_back(allSubsVerts.back().size());	
			allSubsVerts.back().push_back(point3);	
		}
		else{
			allSubsTris.back().push_back(verIdx);	
		}
		verIdx=hasVert(point2);
		if(verIdx<0){
			point2->type=0.0;
			point2->curveDataX=0.0;
			point2->curveDataY=0.0;
			allSubsTris.back().push_back(allSubsVerts.back().size());	
			allSubsVerts.back().push_back(point2);	
		}
		else{
			allSubsTris.back().push_back(verIdx);	
		}
		verIdx=hasVert(point1);
		if(verIdx<0){
			point1->type=0.0;
			point1->curveDataX=0.0;
			point1->curveDataY=0.0;
			allSubsTris.back().push_back(allSubsVerts.back().size());	
			allSubsVerts.back().push_back(point1);	
		}
		else{
			allSubsTris.back().push_back(verIdx);	
		}
	}
}


bool
AWDMergedSubShape::isClockWiseXY(double point1x, double point1y, double point2x, double point2y, double point3x, double point3y)
{
   return ((point1x - point2x) * (point3y - point2y) - (point1y - point2y) * (point3x - point2x) < 0);
}
void
AWDMergedSubShape::addConvexFace(ShapePoint*  point1, ShapePoint*  point2, ShapePoint*  point3)
{
			
	//checkForRessourceLimits();
	point1->type=-1.0;
	point1->curveDataX=1.0;
	point1->curveDataY=1.0;
	allSubsTris.back().push_back(allSubsVerts.back().size());	
	allSubsVerts.back().push_back(point1);		
	point2->type=-1.0;
	point2->curveDataX=0.5;
	point2->curveDataY=0.0;
	allSubsTris.back().push_back(allSubsVerts.back().size());	
	allSubsVerts.back().push_back(point2);
	point3->type=-1.0;
	point3->curveDataX=0.0;
	point3->curveDataY=0.0;
	allSubsTris.back().push_back(allSubsVerts.back().size());	
	allSubsVerts.back().push_back(point3);
	
}
void
AWDMergedSubShape::addConcaveFace(ShapePoint*  point1, ShapePoint*  point2, ShapePoint*  point3)
{
	//checkForRessourceLimits();
		point1->type=1.0;
		point1->curveDataX=0.0;
		point1->curveDataY=0.0;
		allSubsTris.back().push_back(allSubsVerts.back().size());	
		allSubsVerts.back().push_back(point1);		
		point2->type=1.0;
		point2->curveDataX=0.5;
		point2->curveDataY=0.0;
		allSubsTris.back().push_back(allSubsVerts.back().size());	
		allSubsVerts.back().push_back(point2);
		point3->type=1.0;
		point3->curveDataX=1.0;
		point3->curveDataY=1.0;
		allSubsTris.back().push_back(allSubsVerts.back().size());	
		allSubsVerts.back().push_back(point3);	

}

 
vector<vector < ShapePoint*  > >
AWDMergedSubShape::getAllSubsVerts()
 {
	 return allSubsVerts;
 }
vector<vector < int > >
AWDMergedSubShape::getAllSubsTris()
 {
	 return allSubsTris;
 }
 
AWDBlock *
AWDMergedSubShape::get_fill()
{
	return this->thisFill;
}
void
AWDMergedSubShape::set_fill(AWDBlock *thisFill)
{
	this->thisFill=thisFill;

}
 vector<AWDSubShape2D*> 
AWDMergedSubShape::convertToOutputSubShape()
{
	vector<AWDSubShape2D*> returnedSubShapes;
	int thisIdx=0;
	int subShapeLength=allSubsVerts.size();
	for (thisIdx=0; thisIdx<subShapeLength; thisIdx++){
		AWDSubShape2D* sub = new AWDSubShape2D();
		sub->set_fill(this->thisFill);
		AWD_str_ptr v_str;
		AWD_str_ptr i_inner_str;
		int allVertsCount = 9 * allSubsVerts[thisIdx].size();
		v_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * allVertsCount);
		int v_idx=0;
		for (ShapePoint*  thisVert : allSubsVerts[thisIdx]){
			v_str.f64[v_idx++]=thisVert->x*-1;
			v_str.f64[v_idx++]=thisVert->y*-1;
			v_str.f64[v_idx++]=thisVert->type;
			v_str.f64[v_idx++]=thisVert->curveDataX;
			v_str.f64[v_idx++]=thisVert->curveDataY;
			v_str.f64[v_idx++]=thisVert->red;
			v_str.f64[v_idx++]=thisVert->green;
			v_str.f64[v_idx++]=thisVert->blue;
			v_str.f64[v_idx++]=thisVert->alpha;
		}
		sub->add_stream(VERTICES, AWD_FIELD_FLOAT32, v_str,  v_idx);

		i_inner_str.ui32 = (awd_uint32*) malloc(sizeof(awd_uint32) * allSubsTris[thisIdx].size());
		v_idx=0;
		for (int thisTrInt : allSubsTris[thisIdx]){
			i_inner_str.ui32[v_idx++]=thisTrInt;
		}
		sub->add_stream(TRIANGLES_CONCAVE, AWD_FIELD_UINT16, i_inner_str, v_idx);
		returnedSubShapes.push_back(sub);
	}
	
	return returnedSubShapes;
 }
 
void 
AWDMergedSubShape::addSubShape(AWDMergedSubShape* mergeSubShape)
 {
	 // all subShapes except the last one should already be filled to Ressource Limits, so we can just add them.

	 if(mergeSubShape->getAllSubsVerts().size()==0){
		 // nothing to do: empty mergeSub
		 return;
	 }
	 // Check if both shapes can be simply merged 
	 // if not, for now we just create 2 subs
	 if(((mergeSubShape->getAllSubsVerts().back().size()+allSubsVerts.back().size())<(RESSOURCE_LIMIT*3))&&((mergeSubShape->getAllSubsTris().back().size()+allSubsTris.back().size())<(RESSOURCE_LIMIT*15))){
		int cnt=0;
		int vertOffset=allSubsVerts.back().size();
		vector< ShapePoint* > thisPointVec = mergeSubShape->getAllSubsVerts().back();
		for(ShapePoint*  onePoint : thisPointVec){
			allSubsVerts.back().push_back(onePoint);
		}
		vector< int > thisTriVec = mergeSubShape->getAllSubsTris().back();
		for(int oneInt : thisTriVec){
			allSubsTris.back().push_back(oneInt+vertOffset);
		}
	 }
	 if(mergeSubShape->getAllSubsVerts().size()>1){
		 int thisIdx=0;
		 for (thisIdx=0; thisIdx<(mergeSubShape->getAllSubsVerts().size()-1); thisIdx++){
			 allSubsVerts.push_back(mergeSubShape->getAllSubsVerts()[thisIdx]);
			 allSubsTris.push_back(mergeSubShape->getAllSubsTris()[thisIdx]);
		 }
	 }

 }
awd_uint32
AWDMergedSubShape::calc_sub_length(BlockSettings * blockSettings)
{
    awd_uint32 len;

    len = 4; // Sub-mesh header
    //len += this->calc_streams_length();

	len += 8 ;//2 x 4 for length of attributes (both empty right now)
    //len += this->calc_attr_length(true,true, blockSettings);

    return len;
}

void
AWDMergedSubShape::write_sub(AWDFileWriter * fileWriter, BlockSettings * blockSettings, double scale)
{
    AWDDataStream *str = NULL;
    awd_uint32 sub_len = 0;

    // Verify byte-order
   // sub_len = this->calc_streams_length();

    // Write sub-mesh header
	fileWriter->writeUINT32(sub_len);
    //write(fd, &sub_len, sizeof(awd_uint32));

  //  this->properties->write_attributes(fileWriter, blockSettings);

 //   str = this->first_stream;
    while(str) {
        str->write_stream(fileWriter, scale);
        str = str->next;
    }

 //   this->user_attributes->write_attributes(fileWriter, blockSettings);
}

AWDSubShape2D::AWDSubShape2D() :
    AWDAttrElement()
{
    this->num_streams = 0;
    this->first_stream = NULL;
    this->last_stream = NULL;
    this->next = NULL;
	this->fill=NULL;
}

AWDSubShape2D::~AWDSubShape2D()
{
    AWDDataStream *cur;
    cur = this->first_stream;
    while (cur) {
        AWDDataStream *next = cur->next;
        cur->next = NULL;
        delete cur;
        cur = next;
    }
    this->first_stream = NULL;
    this->last_stream = NULL;
    this->first_stream = NULL;
}

unsigned int
AWDSubShape2D::get_num_streams()
{
    return this->num_streams;
}

AWDDataStream *
AWDSubShape2D::get_stream_at(unsigned int idx)
{
    if (idx < this->num_streams) {
        unsigned int cur_idx;
        AWDDataStream *cur;

        cur_idx = 0;
        cur = this->first_stream;
        while (cur) {
            if (cur_idx == idx)
                return cur;

            cur_idx++;
            cur = cur->next;
        }
    }

    return NULL;
}

AWDBlock *
AWDSubShape2D::get_fill()
{
	return this->fill;
}
void
AWDSubShape2D::set_fill(AWDBlock *fill)
{
	this->fill=fill;

}
void
AWDSubShape2D::add_stream(AWD_shape_stream_type type, AWD_field_type data_type, AWD_str_ptr data, awd_uint32 num_elements)
{
    AWDDataStream *str;

    str = new AWDGeomDataStream((awd_uint8)type, data_type, data, num_elements);

    if (this->first_stream == NULL) {
        this->first_stream = str;
    }
    else {
        this->last_stream->next = str;
    }

    this->num_streams++;
    this->last_stream = str;
    this->last_stream->next = NULL;
}

awd_uint32
AWDSubShape2D::calc_streams_length()
{
    awd_uint32 len;
    AWDDataStream *str;

    len = 0;
    str = this->first_stream;
    while (str) {
        len += 6 + str->get_length();
        str = str->next;
    }

    return len;
}

void
AWDSubShape2D::add_uv_transform(awd_float64 * uv_transform)
{
    //if (matrix_floats!=matrix_defaultfloats){
		AWD_field_ptr matrix_val;
        matrix_val.v = malloc(sizeof(awd_float64)*6);
		int i=0;
		for(i=0;i<6;i++){
			matrix_val.f64[i]=uv_transform[i];
		}
        this->properties->set(1, matrix_val, sizeof(awd_float64)*6, AWD_FIELD_FLOAT64);
    //}
}
awd_uint32
AWDSubShape2D::calc_sub_length(BlockSettings * blockSettings)
{
    awd_uint32 len;

    len = 4; // Sub-mesh header
    len += this->calc_streams_length();
    len += this->calc_attr_length(true,true, blockSettings);

    return len;
}

void
AWDSubShape2D::write_sub(AWDFileWriter * fileWriter, BlockSettings * blockSettings, double scale)
{
    AWDDataStream *str;
    awd_uint32 sub_len;

    // Verify byte-order
    sub_len = this->calc_streams_length();

    // Write sub-mesh header
	fileWriter->writeUINT32(sub_len);
    //write(fd, &sub_len, sizeof(awd_uint32));

    this->properties->write_attributes(fileWriter, blockSettings);

    str = this->first_stream;
    while(str) {
        str->write_stream(fileWriter, scale);
        str = str->next;
    }

    this->user_attributes->write_attributes(fileWriter, blockSettings);
}

AWDShape2D::AWDShape2D(string& name) :
    AWDBlock(SHAPE2D_GEOM),
    AWDNamedElement(name),
    AWDAttrElement()
{
    this->first_sub = NULL;
    this->last_sub = NULL;
    this->num_subs = 0;
	this->fill=NULL;
		
}

AWDShape2D::~AWDShape2D()
{
    AWDSubShape2D *cur;
    cur = this->first_sub;
    while (cur) {
        AWDSubShape2D *next = cur->next;
        cur->next = NULL;
        delete cur;
        cur = next;
    }


    this->first_sub = NULL;
    this->last_sub = NULL;
	
	for(AWDMergedSubShape* finalMerged : mergerSubShapes)
	{
		delete finalMerged;
	}

}



vector<AWDMergedSubShape*> 
AWDShape2D::get_mergerSubShapes()
{
	return this->mergerSubShapes;
}
AWDBlock *
AWDShape2D::get_fill()
{
	return this->fill;
}
void
AWDShape2D::set_fill(AWDBlock *fill)
{
	this->fill=fill;

}

void
AWDShape2D::add_sub_merger_mesh(AWDMergedSubShape *sub)
{
	mergerSubShapes.push_back(sub);
}
void
AWDShape2D::add_sub_mesh(AWDSubShape2D *sub)
{
    if (this->first_sub == NULL) {
        this->first_sub = sub;
    }
    else {
        this->last_sub->next = sub;
    }

    this->num_subs++;
    this->last_sub = sub;
}

unsigned int
AWDShape2D::get_num_subs()
{
    return this->num_subs;
}

AWDSubShape2D *
AWDShape2D::get_sub_at(unsigned int idx)
{
    if (idx < this->num_subs) {
        unsigned int cur_idx;
        AWDSubShape2D *cur;

        cur_idx = 0;
        cur = this->first_sub;
        while (cur) {
            if (cur_idx == idx)
                return cur;

            cur_idx++;
            cur = cur->next;
        }
    }

    return NULL;
}

AWDSubShape2D *
AWDShape2D::get_first_sub()
{
    return this->first_sub;
}
 AWDMergedSubShape * 
AWDShape2D::compare_mergerSubShapes(AWDMergedSubShape * mshape1, AWDMergedSubShape * mshape2)
 {
	 if(true){// todo: Add the check if this can be merged based on assigned fill (assigned fill is NULL for shapes generated from fonts)
		return mshape1;
	 }
	 return NULL;
 }
void
AWDShape2D::merge_subs()
{
	vector<AWDMergedSubShape*> finalmergedShapes;
	int cntShapes=0;
	for(AWDMergedSubShape* mergSubShape : this->mergerSubShapes)
	{
		if(cntShapes==0){
			finalmergedShapes.push_back(mergSubShape);
		}
		else{
			for(AWDMergedSubShape* finalMerged : finalmergedShapes)
			{
				AWDMergedSubShape* mergeTarget=compare_mergerSubShapes(finalMerged, mergSubShape);
				if(mergeTarget==NULL){
					// this is a new subGeometry - should not happen at the moment
				}
				else{
					// merge into he found MergerSubShape
					mergeTarget->addSubShape(mergSubShape);
					mergSubShape->set_isMerged(true);
				}

			}
		}
		cntShapes++;
	}
	for(AWDMergedSubShape* mergSubShape : finalmergedShapes)
	{		
		vector<AWDSubShape2D*> newShapes=mergSubShape->convertToOutputSubShape();
		for (AWDSubShape2D* subShape : newShapes){
			this->add_sub_mesh(subShape);
		}
	}
	this->mergerSubShapes.clear();
	this->mergerSubShapes=finalmergedShapes;



}

awd_uint32
AWDShape2D::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    AWDSubShape2D *sub;
    awd_uint32 mesh_len;

    // Calculate length of entire mesh
    // data (not block header)
	mesh_len = sizeof(awd_uint16) + this->get_name().size();//name
    mesh_len += sizeof(awd_uint16); // Num sub shapes
    mesh_len += this->calc_attr_length(true,true,  curBlockSettings);
    sub = this->first_sub;
    while (sub) {
        mesh_len += sub->calc_sub_length( curBlockSettings);
        sub = sub->next;
    }
    return mesh_len;
}

void
AWDShape2D::write_body(AWDFileWriter * fileWriter, BlockSettings *curBlockSettings)
{
	
    awd_uint16 num_subs_be;

    // Write name and sub count
	fileWriter->writeSTRING(this->get_name(), 1);
    num_subs_be = this->num_subs;
	fileWriter->writeUINT16(num_subs_be);

    // Write list of optional properties
    this->properties->write_attributes(fileWriter, curBlockSettings);
	
    AWDSubShape2D *sub;
    // Write all sub-meshes
    sub = this->first_sub;
    while (sub) {
        sub->write_sub(fileWriter, curBlockSettings, curBlockSettings->get_scale());
        sub = sub->next;
    }

    // Write list of user attributes
    this->user_attributes->write_attributes(fileWriter, curBlockSettings);
	
}
