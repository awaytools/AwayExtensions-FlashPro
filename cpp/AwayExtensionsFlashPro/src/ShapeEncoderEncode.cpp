#include "ShapeEncoder.h"
#include <tesselator.h>

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
	
	// check if the data is basically valid
	// it must have at least 3 segments
	// it must be closed !

	// THIS CAN BE SKIPPED, BECAUSE NO SHAPE EVER FAILED (ONLY TESTED 1 DAY NOW)
	int pathCnt=0;
	for(vector<AWDPathSegment*> path:all_segments){
		/*if((path.size()<2)){//||(path[0]->get_startPoint()->x!=path.back()->get_endPoint()->x)||(path[0]->get_startPoint()->y!=path.back()->get_endPoint()->y)){
			AwayJS::Utils::Trace(m_pCallback, "ERROR: PATH IS NOT CLOSED! HoleCnt = %d\n", pathCnt);
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
		}*/
		pathCnt++;
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
	int curveCnt, lineCnt, pntCnt;
	curveCnt = lineCnt = pntCnt = pathCnt = 0;

	for(vector<AWDPathSegment*> path:all_segments){
		vector<SimplePoint * > newPoints;
		for(AWDPathSegment* pathSeg:path){
			start=pathSeg->get_startPoint();
			newPoints.push_back(new SimplePoint(start->x, start->y));
		}
		allPoints.push_back(newPoints);
		curveCnt=lineCnt=pntCnt=0;
		for(AWDPathSegment* pathSeg:path){
			pntCnt++;
			if(pathSeg->get_edgeType()==CURVED_EDGE){
				curveCnt++;
				start=pathSeg->get_startPoint();
				control=pathSeg->get_controlPoint();
				bool isInPath=PointInHole(pathCnt, start->x+((control->x - start->x)*0.1), start->y+((control->y - start->y)*0.1));
				if(pathCnt>0) isInPath=!isInPath;
				if(!isInPath){
					pathSeg->set_edgeType(CONCAVE_EDGE);
					//AwayJS::Utils::Trace(m_pCallback, "    -> CONCAVE %d\n", pathCnt);
				}
				else{
					pathSeg->set_edgeType(CONVEX_EDGE);
					pntCnt++;// we need to check how many points we will add to tess, so count this
					//AwayJS::Utils::Trace(m_pCallback, "    -> CONVEXE %d\n", pathCnt);
				}
			}
			else{
				lineCnt++;
			}
		}
		pointlength.push_back(pntCnt);
		//AwayJS::Utils::Trace(m_pCallback, "    -> CURVECNT=  %d ALLCNT = %d\n", curveCnt, lineCnt);
		//AwayJS::Utils::Trace(m_pCallback, "    -> CURVECNT original=  %d\n", pointlength[pathCnt]);
		pathCnt++;
	}	
	
	for(vector<SimplePoint*> plist: allPoints){
		for(SimplePoint * p: plist){
			delete p;
		}
	}
	points.clear();
	allPoints.clear();
	
	// Now we know the curve type for each curve (This is a good thing)
	// Lets check if any curves are intersecting with any straight lines
	// we collected the lines for this earlier.

	//TODO: NEXT function makes stuff slow slow slow. so do it other way somehow..
	/*
	bool fixedOverlapping=this->get_save_interstect();
	//AwayJS::Utils::Trace(m_pCallback, "Fix Overlay 1\n");
	if(fixedOverlapping){
		//AwayJS::Utils::Trace(m_pCallback, "Fix Overlay 2.1\n");
		fixedOverlapping=remove_overlapping();
		while(fixedOverlapping){
			//AwayJS::Utils::Trace(m_pCallback, "Fix Overlay 3\n");
			fixedOverlapping=remove_overlapping();
			//AwayJS::Utils::Trace(m_pCallback, "Shape has no more Intersections\n");
			//AwayJS::Utils::Trace(m_pCallback, "    number of path-segments after: %d\n", allPathSegments.size());
		}
	}
	else {
		//AwayJS::Utils::Trace(m_pCallback, "Fix Overlay 2.2\n");
		remove_overlapping();
	}
	*/
	//AwayJS::Utils::Trace(m_pCallback, "Fixing intersection is done\n");	
	// by now, all Curves should have been subdivided, so they no longer intersect any other stuff

	
	TESStesselator* tess = 0;
	tess = tessNewTess(NULL);
	pathCnt=0;
	vector<TESSreal*> tesPoints;	
	for(vector<AWDPathSegment*> path:all_segments){	
		//AwayJS::Utils::Trace(m_pCallback, "    -> CURVECNT =new  %d\n", pointlength[pathCnt]);
		TESSreal* thisPoints= (TESSreal*)malloc(sizeof(TESSreal)*pointlength[pathCnt]*2);
		int pntCnt=0;
		for(AWDPathSegment* pathSeg:all_segments[pathCnt])
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

	ShapePoint * lstart;
	ShapePoint * lend;
	ShapePoint * resultPoint;
	AWDPathSegment* check_seg;
	AWDPathSegment* seg;
	bool removed_overlapping=false;
	int segCnt=0;
	int segCnt2=0;
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
		
		/*
		checking if the tesselation created a tri that overlays a convex tri.
		This seams not to be necceessary: 

		double p1x=verts[p[0]*2];
		double p1y=verts[p[0]*2+1];
		double p2x=verts[p[1]*2];
		double p2y=verts[p[1]*2+1];
		double p3x=verts[p[2]*2];
		double p3y=verts[p[2]*2+1];
		for(vector<AWDPathSegment*> path:all_segments){
			for(AWDPathSegment* pathSeg : path){
				ShapePoint* start= pathSeg->get_startPoint();
				ShapePoint* control= pathSeg->get_controlPoint();
				ShapePoint* end= pathSeg->get_endPoint();
				if(((p1x==start->x)&&(p1y==start->y))||((p1x==end->x)&&(p1y==end->y))||((p1x==control->x)&&(p1y==control->y))){
					if(((p2x==start->x)&&(p2y==start->y))||((p2x==end->x)&&(p2y==end->y))||((p2x==control->x)&&(p2y==control->y))){
						if(((p3x==start->x)&&(p3y==start->y))||((p3x==end->x)&&(p3y==end->y))||((p3x==control->x)&&(p3y==control->y))){
							addTri=false;
							break;
						}
					}
				}
			}
			if(!addTri){
				break;}
		}
		*/
	
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
	
			new_point_1=new ShapePoint();
			new_point_2=new ShapePoint();
			new_point_3=new ShapePoint();
			if(pathSeg->get_edgeType()==CONCAVE_EDGE){
				curveCnt++;
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
