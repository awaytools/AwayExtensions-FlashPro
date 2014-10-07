#include "ShapeEncoder.h"


/*
int
ShapeEncoder::get_point_idx(SimplePoint * point)
{
	int idx=0;
	for (SimplePoint * p : points){
		if((double(p->x)==double(point->x))&&(double(point->y)==double(p->y))){
			return idx;
		}
		idx++;
	}
	//AwayJS::Utils::Trace(m_pCallback, "    could not find the point: %f, %f\nADD IT", point->x, point->y);
	//points.push_back(point);
	//return points.size()-1;
	//AwayJS::Utils::Trace(m_pCallback, "ERROR:  Tesselation manipulated point-positions - could not find index for point at position: %f, %f\n", point->x, point->y);
	int idxOut=idx;
	
	// sometimes, if curves are near-interseting other lines, poly2tri seams to alter point positions
	// if we use the new positions, we have a mess
	// my workarround for this (seams to work), is to get the closed point, for a point that was not found 
	idx=0;
	double deltaX=numeric_limits<double>::max();
	double deltaY=numeric_limits<double>::max();
	for (SimplePoint * p2 : points){		
		double temp_deltaX=abs((point->x)-double(p2->x));
		double temp_deltaY=abs((point->y)-double(p2->y));
		if((temp_deltaX<=deltaX)&&(temp_deltaY<=deltaY)){
			deltaX=temp_deltaX;
			deltaY=temp_deltaY;
			idxOut= idx;
		}
		idx++;
		}
	
	//AwayJS::Utils::Trace(m_pCallback, "-Exporter tried to resolve this issue by using the index of point at position: %f, %f\n", points[idxOut]->x, points[idxOut]->y);

	return idxOut;
}
*/
vector<double>
ShapeEncoder::subDivideCurve(double startx, double starty, double cx, double cy, double endx, double endy)
  {
   double c1x = startx + (cx - startx) * 0.5;
   double c1y = starty + (cy - starty) * 0.5;
   
   double c2x = cx + (endx - cx) * 0.5;
   double c2y = cy + (endy - cy) * 0.5;
   
   double ax = c1x + (c2x - c1x) * 0.5;
   double ay = c1y + (c2y - c1y) * 0.5;
   vector<double> resultVec;
   resultVec.push_back(startx);
   resultVec.push_back(starty);
   resultVec.push_back(c1x);
   resultVec.push_back(c1y);
   resultVec.push_back(ax);
   resultVec.push_back(ay);
   resultVec.push_back(c2x);
   resultVec.push_back(c2y);
   resultVec.push_back(endx);
   resultVec.push_back(endy);
   return resultVec;
  }
/*
SimplePoint * 
ShapeEncoder::calc_fill_point(SimplePoint * point1, SimplePoint * point2, SimplePoint * point3, double outputSize)
{
	double xmerged=point1->x+((point2->x-point1->x)*0.5);
	double ymerged=point1->y+((point2->y-point1->y)*0.5);
	double xmerged2=xmerged+((point3->x-xmerged)*outputSize);
	double ymerged2=ymerged+((point3->y-ymerged)*outputSize);
	SimplePoint * newPoint = new SimplePoint(xmerged2, ymerged2);
	return newPoint;
}
*/
void
ShapeEncoder::add_outter_face(int idx1, int idx2, int idx3)
{
	/*
	ShapePoint* p1=points[idx1];
	ShapePoint* p2=points[idx2];
	ShapePoint* p3=points[idx3];
	if(this->outline_threshold==0.0){
		if (isClockWiseXY(p1->x, p1->y, p2->x, p2->y, p3->x, p3->y)){
			triangles_straight_edge_out.push_back(idx3);
			triangles_straight_edge_out.push_back(idx2);
			triangles_straight_edge_out.push_back(idx1);
		}
		else{
			triangles_straight_edge_out.push_back(idx1);
			triangles_straight_edge_out.push_back(idx2);
			triangles_straight_edge_out.push_back(idx3);
		}
	}
	else{
		double edgeLength=edge_length(p1,p2);
		double point_Distance=abs(Sign(p1,p3, p2));
		double distance_bias=point_Distance/edgeLength;
		//AwayJS::Utils::Trace(m_pCallback, "Coutline threshold file : %f\n", this->outline_threshold);
		//AwayJS::Utils::Trace(m_pCallback, "distance_bias : %f\n", distance_bias);
		// if the bias is bigger than the threshold, the triangle needs to be splittet
		if(distance_bias>this->outline_threshold){
			//AwayJS::Utils::Trace(m_pCallback, "Split triangle because of outline threshold : %f\n", distance_bias);
			ShapePoint * newPoint = calc_fill_point(p3,p1, p2, this->outline_threshold);
			int newIdx=points.size();
			points.push_back(newPoint);
			if (isClockWiseXY(p1->x, p1->y, p2->x, p2->y, p3->x, p3->y)){
				triangles_straight_edge_out.push_back(idx3);
				triangles_straight_edge_out.push_back(newIdx);
				triangles_straight_edge_out.push_back(idx1);
			}
			else{
				triangles_straight_edge_out.push_back(idx1);
				triangles_straight_edge_out.push_back(newIdx);
				triangles_straight_edge_out.push_back(idx3);
			}
			add_inner_tri(idx1, newIdx, idx2);
			add_inner_tri(idx2, newIdx, idx3);
		}
		else{
			if (isClockWiseXY(p1->x, p1->y, p2->x, p2->y, p3->x, p3->y)){
				triangles_straight_edge_out.push_back(idx3);
				triangles_straight_edge_out.push_back(idx2);
				triangles_straight_edge_out.push_back(idx1);
			}
			else{
				triangles_straight_edge_out.push_back(idx1);
				triangles_straight_edge_out.push_back(idx2);
				triangles_straight_edge_out.push_back(idx3);
			}
		}
	}*/

}
/*
vector<AWD_edge_type>
ShapeEncoder::get_edge_styles(int thisIdx1, int thisIdx2, int thisIdx3)
{
	AWD_edge_type edge1=INNER_EDGE;
	AWD_edge_type edge2=INNER_EDGE;
	AWD_edge_type edge3=INNER_EDGE;
	int lineIdx1=0;
	int lineIdx2=0;
	int lineCnt=0;
	for (lineCnt=0; lineCnt<allLines.size();lineCnt+=2){
		lineIdx1=get_point_idx(allLines[lineCnt]);
		lineIdx2=get_point_idx(allLines[lineCnt+1]);
		if(((thisIdx1==lineIdx1)&&(thisIdx2==lineIdx2))||
			((thisIdx2==lineIdx1)&&(thisIdx1==lineIdx2))){
				edge1=OUTTER_EDGE;
		}
		if(((thisIdx3==lineIdx1)&&(thisIdx2==lineIdx2))||
			((thisIdx2==lineIdx1)&&(thisIdx3==lineIdx2))){
				edge2=OUTTER_EDGE;
		}
		if(((thisIdx3==lineIdx1)&&(thisIdx1==lineIdx2))||
			((thisIdx1==lineIdx1)&&(thisIdx3==lineIdx2))){
				edge3=OUTTER_EDGE;
		}
		if((edge1==OUTTER_EDGE)&&(edge2==OUTTER_EDGE)&&(edge3==OUTTER_EDGE)){
			lineCnt=allLines.size();
		}
	}
	vector<AWD_edge_type> edge_types;
	edge_types.push_back(edge1);
	edge_types.push_back(edge2);
	edge_types.push_back(edge3);

	return edge_types;
}

void
ShapeEncoder::get_edge_style(AWDPathSegment* pathSeg, SimplePoint* c)
{
	/*
	if(pathSeg->get_edgeType()==CURVED_EDGE){
		double isSide_ctr=Sign(pathSeg->get_startPoint(),pathSeg->get_endPoint(),pathSeg->get_controlPoint());
		double isSide_filled=Sign(pathSeg->get_startPoint(),pathSeg->get_endPoint(),c);
		if((isSide_ctr>0)==(isSide_filled>0)){
			pathSeg->set_edgeType(CONCAVE_EDGE);
		}
		else{
			pathSeg->set_edgeType(CONVEX_EDGE);
		}
	}
	*/
/*
}

double
ShapeEncoder::edge_length(SimplePoint * a, SimplePoint* b)
{

     return sqrt((b->x - a->x)*(b->x - a->x) + (b->y - a->y)*(b->y - a->y));
}
*/
double
ShapeEncoder::side_of_line(ShapePoint * a, ShapePoint * b, ShapePoint * c)
{
     return ((b->x - a->x)*(c->y - a->y) - (b->y - a->y)*(c->x - a->x));
}
double 
ShapeEncoder::Sign(ShapePoint * p1, ShapePoint * p2, ShapePoint * p3)
{
  return (p1->x - p3->x) * (p2->y - p3->y) - (p2->x - p3->x) * (p1->y - p3->y);
}

void ShapeEncoder::add_inner_tri(int idx1, int idx2, int idx3){
	/*SimplePoint* p1=points[idx1];
	SimplePoint* p2=points[idx2];
	SimplePoint* p3=points[idx3];
	if (isClockWiseXY(p1->x, p1->y, p2->x, p2->y, p3->x, p3->y)){
		triangles_inner.push_back(idx3);
		triangles_inner.push_back(idx2);
		triangles_inner.push_back(idx1);
	}
	else{
		triangles_inner.push_back(idx1);
		triangles_inner.push_back(idx2);
		triangles_inner.push_back(idx3);
	}*/
}

double ShapeEncoder::maximum(double x, double y, double z) {
	double max_val = x; 
	if (y > max_val) max_val = y;
	if (z > max_val) max_val = z;
	return max_val; 
} 
double ShapeEncoder::minimum(double x, double y, double z) {
	double min_val = x; 
	if (y < min_val) min_val = y;
	if (z < min_val) min_val = z;
	return min_val; 
} 
/* Check whether P and Q lie on the same side of line AB */
float Side(float px, float py,float qx,float qy, float ax,float ay,  float bx, float by)
{
    float z1 = (bx - ax) * (py - ay) - (px - ax) * (by - ay);
    float z2 = (bx - ax) * (qy - ay) - (qx - ax) * (by - ay);
    return z1 * z2;
}

/* Check whether segment P0P1 intersects with triangle t0t1t2 */
int Intersecting(float p0x,float p0y, float p1x, float p1y, float t0x, float t0y, float t1x, float t1y, float t2x,float t2y)
{
    /* Check whether segment is outside one of the three half-planes
     * delimited by the triangle. */
    float f1 = Side(p0x, p0y, t2x, t2y, t0x, t0y, t1x, t1y), f2 = Side(p1x, p1y, t2x, t2y, t0x, t0y, t1x, t1y);
    float f3 = Side(p0x, p0y, t0x, t0y, t1x, t1y, t2x, t2y), f4 = Side(p1x, p1y, t0x, t0y, t1x, t1y, t2x, t2y);
    float f5 = Side(p0x, p0y, t1x, t1y, t2x, t2y, t0x, t0y), f6 = Side(p1x, p1y, t1x, t1y, t2x, t2y, t0x, t0y);
    /* Check whether triangle is totally inside one of the two half-planes
     * delimited by the segment. */
    float f7 = Side(t0x, t0y, t1x, t1y, p0x, p0y, p1x, p1y);
    float f8 = Side(t1x, t1y, t2x, t2y, p0x, p0y, p1x, p1y);

    /* If segment is strictly outside triangle, or triangle is strictly
     * apart from the line, we're not intersecting */
    if ((f1 < 0 && f2 < 0) || (f3 < 0 && f4 < 0) || (f5 < 0 && f6 < 0)
          || (f7 > 0 && f8 > 0))
        return -1;

    /* If segment is aligned with one of the edges, we're overlapping */
    if ((f1 == 0 && f2 == 0) || (f3 == 0 && f4 == 0) || (f5 == 0 && f6 == 0))
        return 1;

    /* If segment is outside but not strictly, or triangle is apart but
     * not strictly, we're touching */
    if ((f1 <= 0 && f2 <= 0) || (f3 <= 0 && f4 <= 0) || (f5 <= 0 && f6 <= 0)
          || (f7 >= 0 && f8 >= 0))
        return 0;

    /* If both segment points are strictly inside the triangle, we
     * are not intersecting either */
    if (f1 > 0 && f2 > 0 && f3 > 0 && f4 > 0 && f5 > 0 && f6 > 0)
        return -1;

    /* Otherwise we're intersecting with at least one edge */
    return 1;
}
bool
ShapeEncoder::remove_overlapping()
{
			
	bool removed_overlapping_all=false;
	vector<vector<AWDPathSegment*> > newAllPathes;
	for(vector<AWDPathSegment*> path:all_segments){		
		
		//AwayJS::Utils::Trace(m_pCallback, "\nCheck for overlapping triangles %d \n\n", path.size() );
		bool removed_overlapping=false;
		vector<AWDPathSegment*> newPath;
		int segCnt=0;
		int segCnt2=0;
		int pathCnt=0;
		int pathCnt2=0;
		AWDPathSegment* check_seg;
		ShapePoint * start;
		ShapePoint * end;
		ShapePoint * control;
		ShapePoint * lstart;
		ShapePoint * lend;
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
		pathCnt=0;
		for(AWDPathSegment* seg : path){
			if(seg->get_edgeType()!=OUTTER_EDGE){
				start=seg->get_startPoint();
				end=seg->get_endPoint();
				control=seg->get_controlPoint();
				//with this we only count the curved-segments-list (to reduce lookup in curved-segments-list
				// not subDivided yet. check if subdividing is needed.
				if(!seg->get_subdivide()){
					for(pathCnt2=0; pathCnt2<all_segments.size(); pathCnt2++){
						vector<AWDPathSegment*> checkPath=all_segments[pathCnt2];		
						// check if segment overlaps with any straight segment
						for(segCnt2=0; segCnt2<checkPath.size(); segCnt2++){

							if(((pathCnt==pathCnt2)&&(segCnt2!=segCnt))||(pathCnt!=pathCnt2)){
								check_seg=checkPath[segCnt2];
								if(!check_seg->get_subdivide()){
									if(check_seg->get_edgeType()==OUTTER_EDGE){
										lstart=check_seg->get_startPoint();
										lend=check_seg->get_endPoint();
										int intersects=Intersecting(lstart->x, lstart->y, lend->x, lend->y, start->x, start->y, control->x, control->y,end->x, end->y);
										if(intersects==1){
												AwayJS::Utils::Trace(m_pCallback, "Line start<->control intersects\n");
												seg->set_subdivide(true);
												segCnt2=checkPath.size();
												pathCnt2=all_segments.size();
										}
										/*
										if(test_bounding_box_lines(start, control, lstart, lend)){
											AwayJS::Utils::Trace(m_pCallback, "Line start<->control intersects\n");
											if(line_intersect(start, control, lstart, lend)){
												AwayJS::Utils::Trace(m_pCallback, "Line start<->control intersects\n");
												seg->set_subdivide(true);
												segCnt2=checkPath.size();
												pathCnt2=all_segments.size();
											}
										}
										else if(test_bounding_box_lines(end, control, lstart, lend)){
											AwayJS::Utils::Trace(m_pCallback, "Line start<->control intersects\n");
											if(line_intersect(end, control, lstart, lend)){
												AwayJS::Utils::Trace(m_pCallback, "Line end<->control intersects\n");
												seg->set_subdivide(true);
												segCnt2=checkPath.size();
												pathCnt2=all_segments.size();
											}
										}
										else if(test_bounding_box_lines(end, start, lstart, lend)){
											AwayJS::Utils::Trace(m_pCallback, "Line start<->control intersects\n");
											if(line_intersect(end, start, lstart, lend)){// think its not needed, but to make sure
												AwayJS::Utils::Trace(m_pCallback, "Line end<->start intersects\n");
												seg->set_subdivide(true);
												segCnt2=checkPath.size();
												pathCnt2=all_segments.size();
											}
										}
										*/
									}
									else{// this must be a Convecx or Concave
										if(!check_seg->get_subdivide()){
											lstart=check_seg->get_startPoint();
											lend=check_seg->get_endPoint();
											ShapePoint * lcontrol=check_seg->get_controlPoint();
											int intersects=Intersecting(lstart->x, lstart->y, lend->x, lend->y, start->x, start->y, control->x, control->y,end->x, end->y);
											if(intersects==1){
													AwayJS::Utils::Trace(m_pCallback, "Line start<->control intersects\n");
													seg->set_subdivide(true);
													segCnt2=checkPath.size();
													pathCnt2=all_segments.size();}
											else{
												intersects=Intersecting(lcontrol->x, lcontrol->y, lend->x, lend->y, start->x, start->y, control->x, control->y,end->x, end->y);
												if(intersects==1){
													AwayJS::Utils::Trace(m_pCallback, "Line start<->control intersects\n");
													seg->set_subdivide(true);
													segCnt2=checkPath.size();
													pathCnt2=all_segments.size();}
												else{
													intersects=Intersecting(lstart->x, lstart->y, lcontrol->x, lcontrol->y, start->x, start->y, control->x, control->y,end->x, end->y);
													if(intersects==1){
														AwayJS::Utils::Trace(m_pCallback, "Line start<->control intersects\n");
														seg->set_subdivide(true);
														segCnt2=checkPath.size();
														pathCnt2=all_segments.size();
													}
												}
											}
										/*	
											//AwayJS::Utils::Trace(m_pCallback, "check triangles intersect\n");
											if(tris_intersecting(start, control, end, check_seg->get_startPoint(), check_seg->get_controlPoint(), check_seg->get_endPoint())){
												AwayJS::Utils::Trace(m_pCallback, "Curve triangles intersect %d, %d\n", pathCnt, pathCnt2);
												AwayJS::Utils::Trace(m_pCallback, "Curve triangles intersect %d, %d\n", segCnt, segCnt2);
												seg->set_subdivide(true);
												check_seg->set_subdivide(true);
												segCnt2=checkPath.size();
												pathCnt2=all_segments.size();
											}
											else{
												//AwayJS::Utils::Trace(m_pCallback, "Curve is good %d %d\n");
								
											}
											*/
										}
									}
								}
							}
						}
					}
				}
				// if still no need to subdivide, just add to output...
				if(!seg->get_subdivide()){
					newPath.push_back(seg);
					//AwayJS::Utils::Trace(m_pCallback, "Added Segment without dividing\n");
				}
				// need to subDivide
				else{
					//AwayJS::Utils::Trace(m_pCallback, "Subdivide triangle\n");

					// if the control-point is  nearly on the ancor1<->ancor2 line, we add the seg as line-segment
					if(false){//abs(Sign(control, start, end))<0.01){
						seg->set_edgeType(OUTTER_EDGE);
						newPath.push_back(seg);
					}
					else{
						newPoints=subDivideCurve(start->x, start->y, control->x, control->y, end->x, end->y);					
					
						if(!this->double_subdivide){
							//AwayJS::Utils::Trace(m_pCallback, "Single Subdivide Curve\n");

							//AwayJS::Utils::Trace(m_pCallback, "tpoint1 %f %f\n", tpoint1->x, tpoint1->y);
							//AwayJS::Utils::Trace(m_pCallback, "tpoint2 %f %f\n", tpoint2->x, tpoint2->y);
							//AwayJS::Utils::Trace(m_pCallback, "tpoint3 %f %f\n", tpoint3->x, tpoint3->y);
							pointlength[pathCnt]+=1;
							if(seg->get_edgeType()==CONVEX_EDGE)
								pointlength[pathCnt]+=1;
							seg->get_controlPoint()->x=newPoints[2];
							seg->get_controlPoint()->y=newPoints[3];

							tpoint2=new ShapePoint();
							tpoint2->x=newPoints[4];
							tpoint2->y=newPoints[5];

							seg->get_endPoint()->x=tpoint2->x;
							seg->get_endPoint()->y=tpoint2->y;

							newSeg1= new AWDPathSegment();
							newSeg1->set_edgeType(seg->get_edgeType());
							newSeg1->set_startPoint(tpoint2);

							tpoint3=new ShapePoint();
							tpoint3->x=newPoints[6];
							tpoint3->y=newPoints[7];
							newSeg1->set_controlPoint(tpoint3);

							tpoint4=new ShapePoint();
							tpoint4->x=newPoints[8];
							tpoint4->y=newPoints[9];
							newSeg1->set_endPoint(tpoint4);

							newSeg1->set_hole_idx(seg->get_this_hole_idx());
							seg->set_originalSegment(1);
							newSeg1->set_originalSegment(3);
							newPath.push_back(seg);
							newPath.push_back(newSeg1);
							newPoints.clear();
						}
						else{
							// DOUBLE SUBDIVIDE:
							//AwayJS::Utils::Trace(m_pCallback, "Double Subdivide\n");
							newPoints_1=subDivideCurve(newPoints[0], newPoints[1], newPoints[2], newPoints[3], newPoints[4], newPoints[5]);
							newPoints_2=subDivideCurve(newPoints[4], newPoints[5], newPoints[6], newPoints[7], newPoints[8], newPoints[9]);
				
							pointlength[pathCnt]+=3;
							if(seg->get_edgeType()==CONVEX_EDGE)
								pointlength[pathCnt]+=3;
														
							seg->get_controlPoint()->x=newPoints_1[2];
							seg->get_controlPoint()->y=newPoints_1[3];
							tpoint2=new ShapePoint();
							tpoint2->x=newPoints_1[4];
							tpoint2->y=newPoints_1[5];
							seg->get_endPoint()->x=tpoint2->x;
							seg->get_endPoint()->y=tpoint2->y;
							newSeg1= new AWDPathSegment();
							newSeg1->set_edgeType(seg->get_edgeType());
							newSeg1->set_startPoint(tpoint2);
							tpoint3=new ShapePoint();
							tpoint3->x=newPoints_1[6];
							tpoint3->y=newPoints_1[7];
							newSeg1->set_controlPoint(tpoint3);
							tpoint4=new ShapePoint();
							tpoint4->x=newPoints_1[8];
							tpoint4->y=newPoints_1[9];
							newSeg1->set_endPoint(tpoint4);
							newSeg1->set_hole_idx(seg->get_this_hole_idx());
							newSeg2= new AWDPathSegment();
							newSeg2->set_edgeType(seg->get_edgeType());
							tpoint5=new ShapePoint();
							tpoint5->x=newPoints_1[8];
							tpoint5->y=newPoints_1[9];
							newSeg2->set_startPoint(tpoint5);
							tpoint6=new ShapePoint();
							tpoint6->x=newPoints_2[2];
							tpoint6->y=newPoints_2[3];
							newSeg2->set_controlPoint(tpoint6);
							tpoint7=new ShapePoint();
							tpoint7->x=newPoints_2[4];
							tpoint7->y=newPoints_2[5];
							newSeg2->set_endPoint(tpoint7);
							newSeg2->set_hole_idx(seg->get_this_hole_idx());
							newSeg3= new AWDPathSegment();
							newSeg3->set_edgeType(seg->get_edgeType());
							tpoint8=new ShapePoint();
							tpoint8->x=newPoints_2[4];
							tpoint8->y=newPoints_2[5];
							newSeg3->set_startPoint(tpoint8);
							tpoint9=new ShapePoint();
							tpoint9->x=newPoints_2[6];
							tpoint9->y=newPoints_2[7];
							newSeg3->set_controlPoint(tpoint9);
							newSeg3->set_hole_idx(seg->get_this_hole_idx());
							tpoint10=new ShapePoint();
							tpoint10->x=newPoints_2[8];
							tpoint10->y=newPoints_2[9];
							newSeg3->set_endPoint(tpoint10);
							seg->set_originalSegment(1);
							newSeg1->set_originalSegment(2);
							newSeg2->set_originalSegment(2);
							newSeg3->set_originalSegment(3);

							newPath.push_back(seg);
							newPath.push_back(newSeg1);
							newPath.push_back(newSeg2);
							newPath.push_back(newSeg3);
							newPoints_1.clear();
							newPoints_2.clear();
						}
					}
					// set output to true, so we know that overlapping had existed...
					removed_overlapping=true;
				}
			}
			// this is a linear Path...add to output...
			else{
				newPath.push_back(seg);
			}
			segCnt++;
		}

		if(removed_overlapping){
			removed_overlapping_all=true;
		}
		newAllPathes.push_back(newPath);
		pathCnt++;
	}
	// update the lists
	if(!removed_overlapping_all){		
		newAllPathes.clear();
		return false;
	}
	all_segments.clear();
	for(vector<AWDPathSegment*> pathSeg : newAllPathes){
		for (AWDPathSegment* newSeg : pathSeg){
			newSeg->set_subdivide(false);
		}
		all_segments.push_back(pathSeg);
	}
	newAllPathes.clear();
	return true;
}

bool ShapeEncoder::PointInPath(double x, double y)
    {
		
        FCM::Result res;		
        bool oddNodes = false;
        int j = boundaryPoints.size() - 1;
		for (int i = 0; i < boundaryPoints.size(); i++)
        {
			//Utils::Trace(GetCallback(), "check points %f vs %f\n", newPoints[i]->x, newPoints[i]->y);
            if (((boundaryPoints[i]->y < y) && (boundaryPoints[j]->y >= y))||
                ((boundaryPoints[j]->y < y) && (boundaryPoints[i]->y >= y)))
            {
                if ((boundaryPoints[i]->x + (y - boundaryPoints[i]->y)/(boundaryPoints[j]->y - boundaryPoints[i]->y)*(boundaryPoints[j]->x - boundaryPoints[i]->x)) < x)
                {
                    oddNodes = !oddNodes;
                }
            }
            j = i;
        }
   return oddNodes;
}
bool ShapeEncoder::PointInHole(int holeIdx, double x, double y)
{
		
	FCM::Result res;		
	bool oddNodes = false;
	int j = allPoints[holeIdx].size() - 1;
	for (int i = 0; i < allPoints[holeIdx].size(); i++)
     {
			//Utils::Trace(GetCallback(), "check points %f vs %f\n", newPoints[i]->x, newPoints[i]->y);
            if (((allPoints[holeIdx][i]->y < y) && (allPoints[holeIdx][j]->y >= y))||
                ((allPoints[holeIdx][j]->y < y) && (allPoints[holeIdx][i]->y >= y)))
            {
                if ((allPoints[holeIdx][i]->x + (y - allPoints[holeIdx][i]->y)/(allPoints[holeIdx][j]->y - allPoints[holeIdx][i]->y)*(allPoints[holeIdx][j]->x - allPoints[holeIdx][i]->x)) < x)
                {
                    oddNodes = !oddNodes;
                }
            }
            j = i;
        }
   return oddNodes;
}
bool
ShapeEncoder::tris_intersecting(ShapePoint * a1, ShapePoint* a2, ShapePoint * a3, ShapePoint * b1, ShapePoint * b2, ShapePoint * b3)
{
	
	if(!test_bounding_box(a1, a2, a3, b1, b2, b3))	return false;	
	//AwayJS::Utils::Trace(m_pCallback, "		triangle BoundingBox intersect!!\n");
	
	// we only want to check if the control-point of a curve is inside another curve
	if(point_in_tr(a2, b1, b2, b3)) return true;
	//AwayJS::Utils::Trace(m_pCallback, "		Point 1 not in tri!!\n");
	if(point_in_tr(b2, a1, a2, a3)) return true;
	//AwayJS::Utils::Trace(m_pCallback, "		Point not in tri!!\n");
	/*
	if(point_in_tr(a1, b1, b2, b3)) return true;
	if(point_in_tr(b1, a1, a2, a3)) return true;
	if(point_in_tr(a3, b1, b2, b3)) return true;
	if(point_in_tr(b3, a1, a2, a3)) return true;*/
	if(line_intersect(a1, a2, b1, b2)) return true;
	//AwayJS::Utils::Trace(m_pCallback, "		line1 not in tri!!\n");
	if(line_intersect(a1, a2, b2, b3)) return true;
	//AwayJS::Utils::Trace(m_pCallback, "		line1 not in tri!!\n");
	if(line_intersect(a1, a2, b3, b1)) return true;
	//AwayJS::Utils::Trace(m_pCallback, "		line1 not in tri!!\n");
	if(line_intersect(a2, a3, b1, b2)) return true;
	if(line_intersect(a2, a3, b2, b3)) return true;
	if(line_intersect(a2, a3, b3, b1)) return true;
	if(line_intersect(a3, a1, b1, b2)) return true;
	if(line_intersect(a3, a1, b2, b3)) return true;
	if(line_intersect(a3, a1, b3, b1)) return true;
	//AwayJS::Utils::Trace(m_pCallback, "		No lines intersect\n");
	return false;

}

bool
ShapeEncoder::test_bounding_box(ShapePoint * a1, ShapePoint* a2, ShapePoint * a3, ShapePoint * b1, ShapePoint * b2, ShapePoint * b3)
{
	if(maximum(a1->x, a2->x, a3->x) <= minimum(b1->x, b2->x, b3->x)) {return false;}
	if(maximum(a1->y, a2->y, a3->y) <= minimum(b1->y, b2->y, b3->y)) {return false;}
	if(maximum(b1->x, b2->x, b3->x) <= minimum(a1->x, a2->x, a3->x)) {return false;}
	if(maximum(b1->y, b2->y, b3->y) <= minimum(a1->y, a2->y, a3->y)) {return false;}
	//AwayJS::Utils::Trace(m_pCallback, "		A maximumy = %f maxX %f\n", maximum(a1->x, a2->x, a3->x), maximum(a1->y, a2->y, a3->y));
	//AwayJS::Utils::Trace(m_pCallback, "		B maximumy = %f maxX %f\n", maximum(b1->x, b2->x, b3->x), maximum(b1->y, b2->y, b3->y));
	//AwayJS::Utils::Trace(m_pCallback, "		A minimumy = %f maxX %f\n", minimum(a1->x, a2->x, a3->x), minimum(a1->y, a2->y, a3->y));
	//AwayJS::Utils::Trace(m_pCallback, "		B minimumy = %f maxX %f\n", minimum(b1->x, b2->x, b3->x), minimum(b1->y, b2->y, b3->y));

	return true;
}
bool
ShapeEncoder::test_bounding_box_lines(ShapePoint* a1, ShapePoint* a2, ShapePoint * b1, ShapePoint * b2)
{
	if(max(a1->x, a2->x) <= min(b1->x, b2->x)) {return false;}
	if(max(a1->y, a2->y) <= min(b1->y, b2->y)) {return false;}
	if(max(b1->x, b2->x) <= min(a1->x, a2->x)) {return false;}
	if(max(b1->y, b2->y) <= min(a1->y, a2->y)) {return false;}

	return true;
}

bool
ShapeEncoder::point_in_tr(ShapePoint * P, ShapePoint * A, ShapePoint * B, ShapePoint * C)
{
	// Compute vectors      
	double v0x = (C->x - A->x);
	double v0y = (C->y - A->y);
	double v1x =(B->x - A->x);
	double v1y =(B->y - A->y);
	double v2x =(P->x - A->x);
	double v2y =(P->y - A->y);

	// Compute vectors        

	
	// Compute dot products
	double dot00 = (v0x*v0x + v0y*v0y);
	double dot01 = (v0x*v1x + v0y*v1y);
	double dot02 = (v0x*v2x + v0y*v2y);
	double dot11 = (v1x*v1x + v1y*v1y);
	double dot12 = (v1x*v2x + v1y*v2y);


	// Compute barycentric coordinates
	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return ((u >= 0) && (v >= 0) && (u + v < 1));

}

bool
ShapeEncoder::line_intersect(ShapePoint * p1, ShapePoint * p2, ShapePoint * p3, ShapePoint * p4)
{
	
	// Store the values for fast access and easy
	// equations-to-code conversion
	float x1 = p1->x, x2 = p2->x, x3 = p3->x, x4 = p4->x;
	float y1 = p1->y, y2 = p2->y, y3 = p3->y, y4 = p4->y;
 
	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (d == 0) return false;
	
	 AwayJS::Utils::Trace(m_pCallback, "aha\n");
												
	// Get the x and y
	float pre = (x1*y2 - y1*x2);
	float post = (x3*y4 - y3*x4);
	float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
	// Check if the x and y coordinates are within both lines
	if ((( x < min(x1, x2)) || (x > max(x1, x2)) ||	(x < min(x3, x4)) || (x > max(x3, x4)))) return false;
	if ((( y < min(y1, y2)) || (y > max(y1, y2)) ||	(y < min(y3, y4)) || (y > max(y3, y4)))) return false;
 /* // Return the point of intersection
	Point* ret = new Point();
	ret->x = x;
	ret->y = y;
	return ret;*/
	return true;
}
ShapePoint*
ShapeEncoder::line_intersection_point(ShapePoint * p1, ShapePoint * p2, ShapePoint * p3, ShapePoint * p4)
{
	
	// Store the values for fast access and easy
	// equations-to-code conversion
	double x1 = p1->x, x2 = p2->x, x3 = p3->x, x4 = p4->x;
	double y1 = p1->y, y2 = p2->y, y3 = p3->y, y4 = p4->y;
 
	double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (d == 0) return NULL;
	
 
	// Get the x and y
	double pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
	double x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	double y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
	// Check if the x and y coordinates are within both lines
	if ( x < min(x1, x2) || x > max(x1, x2) ||
	x < min(x3, x4) || x > max(x3, x4) ) return NULL;
	if ( y < min(y1, y2) || y > max(y1, y2) ||
	y < min(y3, y4) || y > max(y3, y4) ) return NULL;
	ShapePoint* ret = new ShapePoint();
	ret->x=x;
	ret->y=y;
	return ret;
}