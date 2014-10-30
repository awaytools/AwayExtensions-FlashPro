#include "FontPathShape.h"

FontPathShape::FontPathShape(FCM::PIFCMCallback* pCallback, vector<AWDPathSegment*> main_path, int pathID)
{
	m_pCallback=*pCallback;
	this->pathID=pathID;
	this->parentID=-1;
	this->main_path=main_path;
	this->startX=0;
	this->startY=0;
	this->endX=0;
	this->endY=0;
	if(main_path.size()>0){
		this->startX=main_path[0]->get_startPoint()->x;
		this->startY=main_path[0]->get_startPoint()->y;
		this->endX=main_path.back()->get_endPoint()->x;
		this->endY=main_path.back()->get_endPoint()->y;
	}
}

FontPathShape::~FontPathShape()
{
}

vector<vector<AWDPathSegment*> > FontPathShape::remove_intersecting()
{
	SimplePoint * start;
	SimplePoint * end;
	SimplePoint * lstart;
	SimplePoint * lend;
	SimplePoint * resultPoint;
	AWDPathSegment* check_seg;
	AWDPathSegment* seg;
	bool removed_overlapping=true;
	bool removed_overlapping_path=true;
	int pathCnt=0;
	int segCnt=0;
	int segCnt2=0;
	int tmpCnt=0;
	int next=0;
	int prev=0;
	vector<vector<AWDPathSegment*> > returned_paths;
	vector<vector<AWDPathSegment*> > tmp_paths;
	vector<SimplePoint *> point;
	vector<int> idx_list;
	int pntCnt=0;
	int hasIdx=0;
	returned_paths.push_back(this->main_path);
	int pathCnter=0;
	return returned_paths;
	/*
	//AwayJS::Utils::Trace(m_pCallback, "RETURNED PATH OUTPUT %d\n", returned_paths[0].size());
	while(removed_overlapping){
		removed_overlapping=false;
		for(pathCnt=0; pathCnt<returned_paths.size(); pathCnt++){
			point.clear();
			idx_list.clear();
			point.push_back(returned_paths[pathCnt][0]->get_startPoint());
			idx_list.push_back(0);
			removed_overlapping_path=false;
			for(segCnt=0; segCnt<returned_paths[pathCnt].size(); segCnt++){
				seg=returned_paths[pathCnt][segCnt];
				start=seg->get_startPoint();
				end=seg->get_endPoint();
				for(pntCnt=0; pntCnt<point.size(); pntCnt++){
					SimplePoint * thispoint = point[pntCnt];
					if((end->x==thispoint->x)&&(end->y==thispoint->y)){				
						//AwayJS::Utils::Trace(m_pCallback, "POINS EQUAL\n");
						//AwayJS::Utils::Trace(m_pCallback, "SEGCNT %d\n", segCnt);
						//AwayJS::Utils::Trace(m_pCallback, "MAIN PATH OUTPUT %d\n", returned_paths[0].size());
						hasIdx=-1;
						hasIdx=idx_list[pntCnt];
						if((hasIdx==0)&&(segCnt==returned_paths[pathCnt].size()-1)){
							//AwayJS::Utils::Trace(m_pCallback, "NO SPLITTING NEEDED\n");
						}
						else{
							//AwayJS::Utils::Trace(m_pCallback, "found nsplit idx at count = %d %d\n", hasIdx, segCnt);
							removed_overlapping=true;
							removed_overlapping_path=true;
							vector<AWDPathSegment*> path_1;
							vector<AWDPathSegment*> path_2;
							for(segCnt2=0; segCnt2<returned_paths[pathCnt].size(); segCnt2++){
								AWDPathSegment* thisSeg=returned_paths[pathCnt][segCnt2];
								if((segCnt2>hasIdx)&&(segCnt2<=segCnt)){
									path_1.push_back(thisSeg);
								}
								else if (segCnt2==hasIdx){
									path_1.push_back(thisSeg);
									path_2.push_back(thisSeg);
								}
								else{
									path_2.push_back(thisSeg);
								}
							}
							tmp_paths.push_back(path_1);
							if(path_2.size()>0){
								
								//AwayJS::Utils::Trace(m_pCallback, "path_2 count = %d\n", path_2.size());
								tmp_paths.push_back(path_2);
							}
						}
						segCnt=returned_paths[pathCnt].size();
						pntCnt=point.size();

					}
				}
				point.push_back(end);
				idx_list.push_back(segCnt+1);
			}
			if(!removed_overlapping_path){
				tmp_paths.push_back(returned_paths[pathCnt]);
			}
		}
		returned_paths.clear();
		for(vector<AWDPathSegment*> path : tmp_paths){
			returned_paths.push_back(path);
		}
		this->main_path=returned_paths[0];
		tmp_paths.clear();
	}	
	//AwayJS::Utils::Trace(m_pCallback, "MAIN PATH OUTPUT %d\n\n",this->main_path.size());
	
	
	removed_overlapping=false;
	while(removed_overlapping){
		removed_overlapping=false;
		for(pathCnt=0; pathCnt<returned_paths.size(); pathCnt++){
			removed_overlapping_path=false;
			
			for(segCnt=0; segCnt<returned_paths[pathCnt].size(); segCnt++){
				seg=returned_paths[pathCnt][segCnt];
				start=seg->get_startPoint();
				end=seg->get_endPoint();
				next=segCnt+1;
				if(next>returned_paths[pathCnt].size()){
					next=0;
				}
				prev=segCnt-1;
				if(prev<0){
					prev=returned_paths[pathCnt].size()-1;
				}
				// check if segment intersects with any segment
				for(segCnt2=0; segCnt2<returned_paths[pathCnt].size(); segCnt2++){
					if((segCnt2!=segCnt)&&(segCnt2!=next)&&(segCnt2!=prev)){
						check_seg=returned_paths[pathCnt][segCnt2];
						lstart=check_seg->get_startPoint();
						lend=check_seg->get_endPoint();
						//AwayJS::Utils::Trace(m_pCallback, "Check POINS\n");
						if((start->x==lstart->x)&&(start->y==lstart->y)){
							//AwayJS::Utils::Trace(m_pCallback, "POINS EQUAL 1\n");
							#if 0
							removed_overlapping=true;
							removed_overlapping_path=true;
							vector<AWDPathSegment*> path_1;
							for(tmpCnt=0;tmpCnt<segCnt; tmpCnt++){
								path_1.push_back(returned_paths[pathCnt][tmpCnt]);
							}
							for(tmpCnt=segCnt2;tmpCnt<returned_paths[pathCnt].size(); tmpCnt++){
								path_1.push_back(returned_paths[pathCnt][tmpCnt]);
							}
							tmp_paths.push_back(path_1);

							vector<AWDPathSegment*> path_2;
							for(tmpCnt=segCnt;tmpCnt<segCnt2; tmpCnt++){
								path_2.push_back(returned_paths[pathCnt][tmpCnt]);
							}
                            #endif
						//}
						//else if((end->x==lstart->x)&&(end->y==lstart->y)){
							//AwayJS::Utils::Trace(m_pCallback, "POINS EQUAL 2\n");
                            #if 0
							removed_overlapping=true;
							removed_overlapping_path=true;
							vector<AWDPathSegment*> path_1;
							for(tmpCnt=0;tmpCnt<=segCnt; tmpCnt++){
								path_1.push_back(returned_paths[pathCnt][tmpCnt]);
							}
							for(tmpCnt=segCnt2;tmpCnt<returned_paths[pathCnt].size(); tmpCnt++){
								path_1.push_back(returned_paths[pathCnt][tmpCnt]);
							}
							tmp_paths.push_back(path_1);

							vector<AWDPathSegment*> path_2;
							for(tmpCnt=segCnt+1;tmpCnt<segCnt2; tmpCnt++){
								path_2.push_back(returned_paths[pathCnt][tmpCnt]);
							}
                            #endif
						//}
						//else if((end->x==lend->x)&&(end->y==lend->y)){
							//AwayJS::Utils::Trace(m_pCallback, "POINS EQUAL 3\n");
							#if 0
							removed_overlapping=true;
							removed_overlapping_path=true;
							vector<AWDPathSegment*> path_1;
							for(tmpCnt=0;tmpCnt<=segCnt; tmpCnt++){
								path_1.push_back(returned_paths[pathCnt][tmpCnt]);
							}
							for(tmpCnt=segCnt2+1;tmpCnt<returned_paths[pathCnt].size(); tmpCnt++){
								path_1.push_back(returned_paths[pathCnt][tmpCnt]);
							}
							tmp_paths.push_back(path_1);
							vector<AWDPathSegment*> path_2;
							for(tmpCnt=segCnt+1;tmpCnt<=segCnt2; tmpCnt++){
								path_2.push_back(returned_paths[pathCnt][tmpCnt]);
							}
                            #endif
						//}

						//else if((start->x==lend->x)&&(start->y==lend->y)){
							//AwayJS::Utils::Trace(m_pCallback, "POINS EQUAL 4\n");
							#if 0
							removed_overlapping=true;
							removed_overlapping_path=true;
							vector<AWDPathSegment*> path_1;
							for(tmpCnt=0;tmpCnt<segCnt; tmpCnt++){
								path_1.push_back(returned_paths[pathCnt][tmpCnt]);
							}
							for(tmpCnt=segCnt2+1;tmpCnt<returned_paths[pathCnt].size(); tmpCnt++){
								path_1.push_back(returned_paths[pathCnt][tmpCnt]);
							}
							tmp_paths.push_back(path_1);
							vector<AWDPathSegment*> path_2;
							for(tmpCnt=segCnt;tmpCnt<=segCnt2; tmpCnt++){
								path_2.push_back(returned_paths[pathCnt][tmpCnt]);
							}
                            #endif
						}

						if(test_bounding_box_lines(start, end, lstart, lend)){
							//AwayJS::Utils::Trace(m_pCallback, "BoundingBox Intersects\n");
							
							resultPoint=line_intersection_point(start, end, lstart, lend);
							if(resultPoint!=NULL){
								//AwayJS::Utils::Trace(m_pCallback, "control line_intersection_point %f, %f\n", resultPoint->x, resultPoint->y);
								//m_pCallback
								removed_overlapping=true;
								removed_overlapping_path=true;
								vector<AWDPathSegment*> path_1;
								for(tmpCnt=0;tmpCnt<segCnt; tmpCnt++){
									path_1.push_back(returned_paths[pathCnt][tmpCnt]);
								}
								seg->set_endPoint(resultPoint);
								seg->set_edgeType(OUTTER_EDGE);
								path_1.push_back(seg);
								AWDPathSegment* newSeg1=new AWDPathSegment();
								newSeg1->set_edgeType(OUTTER_EDGE);
								newSeg1->set_startPoint(new SimplePoint(resultPoint->x, resultPoint->y));
								newSeg1->set_endPoint(lend);
								path_1.push_back(newSeg1);
								for(tmpCnt=segCnt2+1;tmpCnt<returned_paths[pathCnt].size(); tmpCnt++){
									path_1.push_back(returned_paths[pathCnt][tmpCnt]);
								}
								tmp_paths.push_back(path_1);

								vector<AWDPathSegment*> path_2;
								newSeg1->set_startPoint(new SimplePoint(resultPoint->x, resultPoint->y));
								check_seg->set_endPoint(end);									
								check_seg->set_edgeType(OUTTER_EDGE);
								path_2.push_back(check_seg);
								for(tmpCnt=segCnt+1;tmpCnt<segCnt2; tmpCnt++){
									path_2.push_back(returned_paths[pathCnt][tmpCnt]);
								}
								AWDPathSegment* newSeg2=new AWDPathSegment();
								newSeg2->set_edgeType(OUTTER_EDGE);
								newSeg1->set_startPoint(new SimplePoint(lstart->x, lstart->y));
								newSeg2->set_endPoint(resultPoint);
								path_2.push_back(newSeg2);
								tmp_paths.push_back(path_2);
										
								segCnt2=returned_paths[pathCnt].size();
								segCnt=returned_paths[pathCnt].size();
							}
						}
					}
				}
			}
			
			if(!removed_overlapping_path){
				tmp_paths.push_back(returned_paths[pathCnt]);
			}
		}
		returned_paths.clear();
		for(vector<AWDPathSegment*> path : tmp_paths){
			returned_paths.push_back(path);
		}
		this->main_path=returned_paths[0];
		tmp_paths.clear();

	}*/
	
	return returned_paths;
}

bool
FontPathShape::test_bounding_box_lines(SimplePoint * a1, SimplePoint* a2, SimplePoint * b1, SimplePoint * b2)
{
	if(max(a1->x, a2->x) <= min(b1->x, b2->x)) {return false;}
	if(max(a1->y, a2->y) <= min(b1->y, b2->y)) {return false;}
	if(max(b1->x, b2->x) <= min(a1->x, a2->x)) {return false;}
	if(max(b1->y, b2->y) <= min(a1->y, a2->y)) {return false;}

	return true;
}

SimplePoint*
FontPathShape::line_intersection_point(SimplePoint * p1, SimplePoint * p2, SimplePoint * p3, SimplePoint * p4)
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
	SimplePoint* ret = new SimplePoint(x, y);
	return ret;
}
double FontPathShape::get_endX()
{
	return this->startX;
}
void FontPathShape::set_endX(double endX)
{
	this->endX=endX;
}
double FontPathShape::get_endY()
{
	return this->endY;
}
void FontPathShape::set_endY(double endY)
{
	this->endY=endY;
}
double FontPathShape::get_startX()
{
	return this->startX;
}
void FontPathShape::set_startX(double startX)
{
	this->startX=startX;
}
double FontPathShape::get_startY()
{
	return this->startY;
}
void FontPathShape::set_startY(double startY)
{
	this->startY=startY;
}

int FontPathShape::get_pathID()
{
	return this->pathID;
}
void FontPathShape::set_pathID(int pathID)
{
	this->pathID=pathID;
}

int FontPathShape::get_parentID()
{
	return this->parentID;
}
void FontPathShape::set_parentID(int parentID)
{
	this->parentID=parentID;
}
vector<AWDPathSegment*> FontPathShape::get_main_path()
{
	return this->main_path;
}
void FontPathShape::set_main_path(vector<AWDPathSegment*> main_path)
{
	this->main_path=main_path;
}

void FontPathShape::add_hole(FontPathShape* new_hole)
{
	hole_paths.push_back(new_hole);
}
vector<FontPathShape* > FontPathShape::get_holes()
{
	return this->hole_paths;
}
int FontPathShape::tested_aginst(int id)
{
	return 0;
}
void FontPathShape::set_test_result(int)
{
}