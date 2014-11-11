#include "ShapeEncoder.h"
#include "tesselator.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG
void
ShapeEncoder::encode_subShape(bool checkForIntersect)
{	

	// check if any data has been set
	if(all_segments.size()<1){
		//AwayJS::Utils::Trace(m_pCallback, "    -> Skip SubShape NR: %d,no Path was set.\n", awd_shape->get_num_subs());
		for(vector<AWDPathSegment*> path:all_segments){
			for(AWDPathSegment* pathSeg:path){
				delete pathSeg;
			}
			path.clear();
		}
		all_segments.clear();
		vector<AWDPathSegment *> boundary_segs;
		all_segments.push_back(boundary_segs);
		return;
	}
	
	// for each curve, we need to decide if it is a Concave or Convex.

	// i found 2 reliable ways: 
	// one:
	//		- pre-triangulate the shape without using the control points
	//		- for each curve, find the triangle that contains start and end point of the curve
	//		- compare the vert of the tri that is not start or endpoint of the curve to the control-point of the curve
	//		- if both points are on same side of line start<->end of curve, this is a convex, otherwise it is concave
	//
	//two: 
	//		- for each curve, move the controlpoint closer to the startpoint.
	//		- if the controlpoint is inside the path = convex curve . if not = concave curve

	// with poly2tri i went the first way.
	// now i implemented (and use) the secont way. 
	// both are expensive on process time
		
	pointlength.clear();	
	ShapePoint * start, * end, * control;
	ShapePoint * lstart, * lend, * lcontrol;
	int curveCnt, lineCnt, pntCnt, pathCnt, segCnt;
	curveCnt = lineCnt = pntCnt = pathCnt = segCnt =0;
	int segCnt2=0;
	int subsegCnt=0;
	int subsegCnt2=0;
	for(vector<AWDPathSegment*> path:all_segments){
		vector<SimplePoint * > newPoints;
		for(AWDPathSegment* pathSeg:path){
			start=pathSeg->get_startPoint();
			newPoints.push_back(new SimplePoint(start->x, start->y));
		}
		allPoints.push_back(newPoints);
		//AwayJS::Utils::Trace(m_pCallback, "    -> CHECK SEGMENT %d\n", pathCnt);
		curveCnt=lineCnt=pntCnt=0;
		for(AWDPathSegment* pathSeg:path){
			pntCnt++;
			if(pathSeg->get_edgeType()==CURVED_EDGE){
				curveCnt++;
				start=pathSeg->get_startPoint();
				control=pathSeg->get_controlPoint();
				bool isInPath2=PointInHole(pathCnt, control->x, control->y);
				bool isInPath=PointInHole(pathCnt, start->x+((control->x - start->x)*0.000001), start->y+((control->y - start->y)*0.000001));

				if(pathCnt>0){
					isInPath=!isInPath;
					isInPath2=!isInPath2;
				}
				if(!isInPath){
					pathSeg->set_edgeType(CONCAVE_EDGE);
					//AwayJS::Utils::Trace(m_pCallback, "    -> CONCAVE %d\n", pathCnt);
				}
				else{
					pathSeg->set_edgeType(CONVEX_EDGE);
					//AwayJS::Utils::Trace(m_pCallback, "    -> CONVEXE %d\n", pathCnt);
				}
			}
			else{
				lineCnt++;
			}
		}
		pathCnt++;
	}	
	
	for(vector<SimplePoint*> plist: allPoints){
		for(SimplePoint * p: plist){
			delete p;
		}
	}
	points.clear();
	allPoints.clear();
	
	// we know the curve type for each curve. 
	// This is all that is needed for valid export.
	
	// however, there might be curves-intersecting other curves, wich will result in errors in the triangulation

	// Optionally resolve all intersections:
	if(awd->getExporterSettings()->get_resolve_intersections()){
		
		// resolving:
		// step1:	pre-calculate bounds for all segments. store this bounds in list.
		// step2:	check all bounding-boxes agains each other. 
		//			For each encountered intersection, create a AWDPathIntersection.
		//			The AWDPathInteresection gets added to the "interesection"-list of both segments.	
		// step3:	Loop over all segments, and resolve all AWDPathIntersections for segments that are not curved.
		//			These intersections are easier to resolve, so we process them first
		//step4:	Loop over all segmens, and resolve all remaining AWDPathIntersections.

		vector<vector<double> > allBounds;
		vector<double> curBounds;
		vector<double> curTestBounds;
		vector<double> mergedBounds;
		vector<vector<AWDPathSegment*> > allBoundSegs;
		pointlength.clear();	
		pntCnt=0;
		int boundsCnt=0;
		int lastusedBounds=-1;
		bool newBounds=true;
		bool isIntersect=true;
		pathCnt=0;
		AWDPathSegment* pathSeg2;

		// collect a flat list of all segments, and pre-calculate their bounds
		vector<AWDPathSegment*> all_segs_flat;
		vector<AWDPathIntersection*> all_path_intersections;
		double allsize=0;
		int allCnt=0;
		for(vector<AWDPathSegment*> path:all_segments)
		{	
			for(AWDPathSegment* pathSeg:path)
			{
				if(pathSeg->get_edgeType()!=OUTTER_EDGE){
					allCnt++;
					allsize+=pathSeg->get_length();
				}
				curBounds=pathSeg->get_bounds();
				all_segs_flat.push_back(pathSeg);
			}
		}
		// loop over the previous collected list, and collect information about intersecting.
		// for each intersecting, we create a AWDPathIntersection object, and store a reference to it on the Path-Segment->path_intersections
		AWDPathSegment* pathSeg;
		AWDPathIntersection* thisPathIntersec;
		AWDPathIntersection* newIntersection;
		int childCnt=0;
		int childCnt2=0;
		for(segCnt=0; segCnt<all_segs_flat.size(); segCnt++){
			for(childCnt=0; childCnt<all_segs_flat[segCnt]->get_subdivided_path().size(); childCnt++){
				pathSeg=all_segs_flat[segCnt]->get_subdivided_path()[childCnt];
				pathCnt++;
				newBounds=true;
				curBounds=pathSeg->get_bounds();
				//AwayJS::Utils::Trace(m_pCallback, "Bounds A = %f, %f, %f, %f\n", curBounds[0], curBounds[1], curBounds[2], curBounds[3]);
				for(segCnt2=0; segCnt2<segCnt; segCnt2++){
					for(childCnt2=0; childCnt2<all_segs_flat[segCnt2]->get_subdivided_path().size(); childCnt2++){
						pathSeg2=all_segs_flat[segCnt2]->get_subdivided_path()[childCnt2];
						if((pathSeg->get_edgeType()!=OUTTER_EDGE)||(pathSeg2->get_edgeType()!=OUTTER_EDGE)){
							thisPathIntersec=pathSeg->find_path_intersections(pathSeg, pathSeg2);
							if(thisPathIntersec==NULL){
								curTestBounds=pathSeg2->get_bounds();
								isIntersect=bounds_interesect(curTestBounds, curBounds);
								if(isIntersect){
									newIntersection = new AWDPathIntersection(pathSeg, pathSeg2);
									pathSeg->add_path_intersections(newIntersection);
									pathSeg2->add_path_intersections(newIntersection);					
									all_path_intersections.push_back(newIntersection);
								}
							}
						}
					}
				}
			}
		}

		// now each AWDSegment, that intersects with another AWDSegment, has a list of AWDPathIntersection objects.
		// Both intersecting AWDSegments share the same AWDPathIntersection object.

		// first we resolve all PathIntersections, that are involving straight segments
		// they are easier to resolve, because we dont have to decide wich triangle needs subdividng
	
		for(AWDPathSegment* pathSeg:all_segs_flat)
		{	
			if(pathSeg->get_edgeType()==OUTTER_EDGE){
				for(AWDPathIntersection* intersect:pathSeg->get_path_intersections()){
					if(intersect->get_state()==UNCHECKED){
						resolve_segment_intersection(intersect);
					}		
				}
			}	
		}
	
		// loop over the list of PathIntersections, and resolve the remaining
		// (only curve <-> curve intersections should be left unsolved by now)
		// they are more tricky to resolve, because we want no unneccessary subdividing
		for (AWDPathIntersection* oneintersect: all_path_intersections){
			if(oneintersect->get_state()==UNCHECKED){
				resolve_segment_intersection(oneintersect);					
			}	
			delete oneintersect;	
		}	
		// by now, all Curves should have been subdivided, so they no longer intersect each other
	}
	
	//	prepare the final lists of segments for libTess2
	//  if we resolved intersections, a curve can be subdivided, 
	//	wich means it contains a list of AWDSegments in itself.
	//	at this step, we resolve this nested path into one flat path.

	vector<double> newPoints;
	vector<double> newPoints_1;
	vector<double> newPoints_2;
	ShapePoint* tpoint1;
	ShapePoint* tpoint2;
	ShapePoint* tpoint3;
	ShapePoint* tpoint4;
	ShapePoint* tpoint5;
	ShapePoint* tpoint6;
	ShapePoint* tpoint7;
	ShapePoint* tpoint8;
	ShapePoint* tpoint9;
	ShapePoint* tpoint10;
	AWDPathSegment* newSeg1;
	AWDPathSegment* newSeg2;
	AWDPathSegment* newSeg3;
	
	vector<vector<AWDPathSegment*> > newAllPathes;
	pointlength.clear();	
	pntCnt=0;
	int cnt1=0;
	int cnt2=0;
	for(vector<AWDPathSegment*> path:all_segments){	
		pntCnt=0;
		vector<AWDPathSegment*> newPath;
		for(AWDPathSegment* pathSeg:path)
		{
			cnt1++;			
			for(AWDPathSegment* pathSeg2:pathSeg->get_subdivided_path())
			{
				cnt2++;
				pntCnt++;		
				if(pathSeg2->get_edgeType()==CONVEX_EDGE){
					pntCnt++;
				}
				newPath.push_back(pathSeg2);
			}			
		}
		pointlength.push_back(pntCnt);
		newAllPathes.push_back(newPath);
	}
	
	//AwayJS::Utils::Trace(m_pCallback, "    -> inSegments = %d, outSegments = %d\n", cnt1, cnt2);
	// create the final list of points that will be sended to libtess
	TESStesselator* tess = 0;
	tess = tessNewTess(NULL);
	pathCnt=0;
	vector<TESSreal*> tesPoints;	
	for(vector<AWDPathSegment*> path:newAllPathes){	
		//AwayJS::Utils::Trace(m_pCallback, "    -> CURVECNT =new  %d\n", pointlength[pathCnt]);
		TESSreal* thisPoints= (TESSreal*)malloc(sizeof(TESSreal)*pointlength[pathCnt]*2);
		int pntCnt=0;
		for(AWDPathSegment* pathSeg:newAllPathes[pathCnt])
		{
			thisPoints[pntCnt++]=pathSeg->get_startPoint()->x;
			thisPoints[pntCnt++]=pathSeg->get_startPoint()->y;
			if(pathSeg->get_edgeType()==CONVEX_EDGE){
				thisPoints[pntCnt++]=pathSeg->get_controlPoint()->x;
				thisPoints[pntCnt++]=pathSeg->get_controlPoint()->y;
			}
		}
		//AwayJS::Utils::Trace(m_pCallback, "    -> CURVECNT 2=  %d\n", pointlength[pathCnt]);
		tessAddContour(tess, 2, &thisPoints[0], sizeof(TESSreal)*2, pointlength[pathCnt]);
		pathCnt++;
		tesPoints.push_back(thisPoints);		
	}
	newAllPathes.clear();
	//AwayJS::Utils::Trace(m_pCallback, "    -> CURVECNT =new  \n");
	/*	
	// TESTDATA = A Rectangle with a hole	
	pntCnt=0;
	TESSreal* thisPoints= (TESSreal*)malloc(sizeof(TESSreal)*8);
	thisPoints[pntCnt++]=-100.0;
	thisPoints[pntCnt++]=-100.0;
	thisPoints[pntCnt++]=100.0;
	thisPoints[pntCnt++]=-100.0;
	thisPoints[pntCnt++]=100.0;
	thisPoints[pntCnt++]=100.0;
	thisPoints[pntCnt++]=-100.0;
	thisPoints[pntCnt++]=100.0;
	tessAddContour(tess, 2, &thisPoints[0], sizeof(TESSreal)*2, 4);	
	pntCnt=0;
	TESSreal* thisPoints3= (TESSreal*)malloc(sizeof(TESSreal)*8);
	thisPoints3[pntCnt++]=120.0;
	thisPoints3[pntCnt++]=-100.0;
	thisPoints3[pntCnt++]=200.0;
	thisPoints3[pntCnt++]=-100.0;
	thisPoints3[pntCnt++]=200.0;
	thisPoints3[pntCnt++]=100.0;
	thisPoints3[pntCnt++]=120.0;
	thisPoints3[pntCnt++]=100.0;
	tessAddContour(tess, 2, &thisPoints3[0], sizeof(TESSreal)*2, 4);
	pntCnt=0;
	TESSreal* thisPoints2= (TESSreal*)malloc(sizeof(TESSreal)*8);
	thisPoints2[pntCnt++]=-50;
	thisPoints2[pntCnt++]=-50;
	thisPoints2[pntCnt++]=150;
	thisPoints2[pntCnt++]=-50;
	thisPoints2[pntCnt++]=150;
	thisPoints2[pntCnt++]=50;
	thisPoints2[pntCnt++]=-50;
	thisPoints2[pntCnt++]=50;
	tessAddContour(tess, 2, &thisPoints2[0], sizeof(float)*2, 4);
	*/
	
	int hasTriangulated=tessTesselate(tess, TESS_WINDING_ODD, TESS_POLYGONS, 3, 2, NULL);
	
	//AwayJS::Utils::Trace(m_pCallback, "hasTriangulated  = %d\n",hasTriangulated);
	const float* verts = tessGetVertices(tess);
	const int* vinds = tessGetVertexIndices(tess);
	const int* elems = tessGetElements(tess);
	const int nverts = tessGetVertexCount(tess);
	const int nelems = tessGetElementCount(tess);		

	ShapePoint * resultPoint;
	AWDPathSegment* check_seg;
	AWDPathSegment* seg;
	bool removed_overlapping=false;
	segCnt=0;
	segCnt2=0;
	int segCnt_prev=0;
	int segCnt_next=0;
	
	AWDMergedSubShape* mergerSubShape = new AWDMergedSubShape((AWDBlock*)awd_shape->get_fill());
		/*
	vector<AWDMergedSubShape*> mergershapes=this->awd_shape->get_mergerSubShapes();
	AWDMergedSubShape* mergerSubShape = mergershapes.back();
	if(mergerSubShape==NULL){
		mergerSubShape=new AWDMergedSubShape((AWDBlock*)awd_shape->get_fill());
		this->awd_shape->add_sub_merger_mesh(mergerSubShape);
	}*/
	
	//mergerSubShape->checkForRessourceLimits_extern(nverts, nelems);
	int vertexOffset=mergerSubShape->getAllSubsVerts().back().size();
	ShapePoint* new_point_1;
	ShapePoint* new_point_2;
	ShapePoint* new_point_3;
	for (int i = 0; i < nverts; ++i){
		new_point_1=new ShapePoint();
		new_point_1->x=verts[i*2];
		new_point_1->y=verts[i*2+1];
		//AwayJS::Utils::Trace(m_pCallback, "Triangulated vert: = %f, %f\n",verts[i*2], verts[i*2+1]);
		mergerSubShape->addShapePoint(new_point_1);
	}
	
	for (int i = 0; i < nelems; ++i)
	{
		const int* p = &elems[i*3];
		bool addTri=true;	
		
		if(addTri){
			mergerSubShape->addFillFaceIDX(vertexOffset+p[0], vertexOffset+p[1], vertexOffset+p[2]);
		}
		else{
			AwayJS::Utils::Trace(m_pCallback, "Did not add tri because it is in place needed for convex curve ");
		}

	}
	
	this->awd_shape->add_sub_merger_mesh(mergerSubShape);
	if (tess) tessDeleteTess(tess);	
	for(TESSreal* thisPoints : tesPoints){
		free(thisPoints);
	}

	for(vector<AWDPathSegment*> path:all_segments){
		int curveCnt=0;
		int lineCnt=0;
		for(AWDPathSegment* pathSeg:path){		
	
			if(pathSeg->get_edgeType()==CONCAVE_EDGE){
				curveCnt++;
				new_point_1=new ShapePoint();
				new_point_2=new ShapePoint();
				new_point_3=new ShapePoint();
				new_point_1->x=pathSeg->get_endPoint()->x;
				new_point_1->y=pathSeg->get_endPoint()->y;
				new_point_2->x=pathSeg->get_controlPoint()->x;
				new_point_2->y=pathSeg->get_controlPoint()->y;
				new_point_3->x=pathSeg->get_startPoint()->x;
				new_point_3->y=pathSeg->get_startPoint()->y;
				mergerSubShape->addConcaveFace(new_point_1, new_point_2, new_point_3);
			}
			else if(pathSeg->get_edgeType()==CONVEX_EDGE){
				curveCnt++;
				new_point_1=new ShapePoint();
				new_point_2=new ShapePoint();
				new_point_3=new ShapePoint();
				new_point_1->x=pathSeg->get_startPoint()->x;
				new_point_1->y=pathSeg->get_startPoint()->y;
				new_point_2->x=pathSeg->get_controlPoint()->x;
				new_point_2->y=pathSeg->get_controlPoint()->y;
				new_point_3->x=pathSeg->get_endPoint()->x;
				new_point_3->y=pathSeg->get_endPoint()->y;
				mergerSubShape->addConvexFace(new_point_1, new_point_2, new_point_3);
			}
			else {
				lineCnt++;
			}
			
			delete pathSeg;	
		}	
		path.clear();
		//AwayJS::Utils::Trace(m_pCallback, "    -> New CURVECNT=  %d LineCnt = %d\n", curveCnt, lineCnt);
	}
	all_segments.clear();
	
	vector<AWDPathSegment *> boundary_segs2;
	all_segments.push_back(boundary_segs2);
	return;
	
/*
		
			// for all triangles, we check if they have any outter-edges (not curved-edges)
			// if a tri has more than one outter edge, it gets split
			// befor a outter-edge is added to the triangle list, its size gets checked.
			// if the size is to big ("outline_threshold") the trie gets split into 2 inner and 1 outter tri
			// verticles can be shared between inner and outter triangle (not curved triangles).
				
			int lineCnt=0;		
			int lineIdx1=0;	
			int lineIdx2=0;
			//AwayJS::Utils::Trace(m_pCallback, "    number of tesselated tris: %d\n", nonCurvedTris.size());
			for(tcnt=0; tcnt<nonCurvedTris.size(); tcnt+=3){
				thisIdx1=nonCurvedTris[tcnt];
				thisIdx2=nonCurvedTris[tcnt+1];
				thisIdx3=nonCurvedTris[tcnt+2];
				p1=points[thisIdx1];
				p2=points[thisIdx2];
				p3=points[thisIdx3];
				vector<AWD_edge_type> edge_types=get_edge_styles(thisIdx1, thisIdx2, thisIdx3);
				//all inner edges
				if((edge_types[0]==INNER_EDGE)&&(edge_types[1]==INNER_EDGE)&&(edge_types[2]==INNER_EDGE)){	
					add_inner_tri(thisIdx1, thisIdx2, thisIdx3);
				}	

				//one outter edge	
				else if((edge_types[0]==OUTTER_EDGE)&&(edge_types[1]==INNER_EDGE)&&(edge_types[2]==INNER_EDGE)){
					add_outter_face(thisIdx2, thisIdx3, thisIdx1);
				}
				//one outter edge
				else if((edge_types[0]==INNER_EDGE)&&(edge_types[1]==OUTTER_EDGE)&&(edge_types[2]==INNER_EDGE)){
					add_outter_face(thisIdx3, thisIdx1, thisIdx2);
				}
				//one outter edge
				else if((edge_types[0]==INNER_EDGE)&&(edge_types[1]==INNER_EDGE)&&(edge_types[2]==OUTTER_EDGE)){
					add_outter_face(thisIdx1, thisIdx2, thisIdx3);
				}
				//three outter edges
				else if((edge_types[0]==OUTTER_EDGE)&&(edge_types[1]==OUTTER_EDGE)&&(edge_types[2]==OUTTER_EDGE)){
					//AwayJS::Utils::Trace(m_pCallback, "    SPLIT TRIANGLE 1\n");
					SimplePoint* newPoint = new SimplePoint (((p1->x+p2->x+p3->x)/3),((p1->y+p2->y+p3->y)/3));
					int newIdx=points.size();
					points.push_back(newPoint);				
					add_outter_face(thisIdx2, newIdx, thisIdx1);
					add_outter_face(thisIdx3, newIdx, thisIdx2);
					add_outter_face(thisIdx1, newIdx, thisIdx3);
				}
				// two outter edges
				else if((edge_types[0]==OUTTER_EDGE)&&(edge_types[1]==OUTTER_EDGE)&&(edge_types[2]==INNER_EDGE)){
					//AwayJS::Utils::Trace(m_pCallback, "    SPLIT TRIANGLE 2\n");
					SimplePoint* newPoint = new SimplePoint (((p1->x+p2->x+p3->x)/3),((p1->y+p2->y+p3->y)/3));
					int newIdx=points.size();
					points.push_back(newPoint);						
					add_inner_tri(thisIdx1, newIdx, thisIdx3);
					add_outter_face(thisIdx2, newIdx, thisIdx1);
					add_outter_face(thisIdx3, newIdx, thisIdx2);
				}
				// two outter edges
				else if((edge_types[0]==INNER_EDGE)&&(edge_types[1]==OUTTER_EDGE)&&(edge_types[2]==OUTTER_EDGE)){
					//AwayJS::Utils::Trace(m_pCallback, "    SPLIT TRIANGLE 3\n");
					SimplePoint* newPoint = new SimplePoint (((p1->x+p2->x+p3->x)/3),((p1->y+p2->y+p3->y)/3));
					int newIdx=points.size();
					points.push_back(newPoint);				
					add_inner_tri(thisIdx2, newIdx, thisIdx1);					
					add_outter_face(thisIdx1, newIdx, thisIdx3);
					add_outter_face(thisIdx3, newIdx, thisIdx2);
				}
				// two outter edges
				else if((edge_types[0]==OUTTER_EDGE)&&(edge_types[1]==INNER_EDGE)&&(edge_types[2]==OUTTER_EDGE)){
					//AwayJS::Utils::Trace(m_pCallback, "    SPLIT TRIANGLE 4\n");
					SimplePoint* newPoint = new SimplePoint (((p1->x+p2->x+p3->x)/3),((p1->y+p2->y+p3->y)/3));
					int newIdx=points.size();
					points.push_back(newPoint);				
					add_inner_tri(thisIdx3, newIdx, thisIdx2);					
					add_outter_face(thisIdx1, newIdx, thisIdx3);
					add_outter_face(thisIdx2, newIdx, thisIdx1);
				}				
			}	

		*/
	
}
