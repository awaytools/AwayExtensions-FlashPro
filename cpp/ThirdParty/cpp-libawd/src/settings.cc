#include "util.h"
#include "settings.h"
#include <sys/stat.h>


BlockSettings::BlockSettings()
{
        this->wideMatrix=false;
        this->wideGeom=false;
        this->wideProps=false;
        this->wideAttributes=false;
        this->scale=1.0;
}

BlockSettings::~BlockSettings()
{
}

bool
BlockSettings::get_wide_matrix()
{
    return this->wideMatrix;
}
void
BlockSettings::set_wide_matrix(bool wideMatrix)
{
    this->wideMatrix=wideMatrix;
}
bool
BlockSettings::get_wide_geom()
{
    return this->wideGeom;
}
void
BlockSettings::set_wide_geom(bool wideGeom)
{
    this->wideGeom=wideGeom;
}
bool
BlockSettings::get_wide_props()
{
    return this->wideProps;
}
void
BlockSettings::set_wide_props(bool wideProps)
{
    this->wideProps=wideProps;
}
bool
BlockSettings::get_wide_attributes()
{
    return this->wideAttributes;
}
void
BlockSettings::set_wide_attributes(bool wideAttributes)
{
    this->wideAttributes=wideAttributes;
}
double
BlockSettings::get_scale()
{
    return this->scale;
}
void
BlockSettings::set_scale(double scale)
{
    this->scale=scale;
}


AWDExporterSettings::AWDExporterSettings()
{
	this->globalBlockSettings= new BlockSettings();
	this->fps=25;
}

AWDExporterSettings::~AWDExporterSettings()
{
	delete this->globalBlockSettings;
}

BlockSettings*
AWDExporterSettings::get_blockSettings()
{
	return this->globalBlockSettings;
}


		

bool
AWDExporterSettings::get_resolve_intersections()
{
	return this->resolve_intersections;
}
void
AWDExporterSettings::set_resolve_intersections(bool resolve_intersections)
{
	this->resolve_intersections=resolve_intersections;
}
double
AWDExporterSettings::get_curve_threshold()
{
	if(this->curve_threshold>1){
		this->curve_threshold=1;
	}
	if(this->curve_threshold<=0){
		this->curve_threshold=0.005;
	}
	return this->curve_threshold;
}
void
AWDExporterSettings::set_curve_threshold(double curve_threshold)
{
	if(this->curve_threshold>10){
		this->curve_threshold=10;
	}
	if(this->curve_threshold<=0){
		this->curve_threshold=0;
	}
	this->curve_threshold=curve_threshold;
}
int
AWDExporterSettings::get_fps()
{
	return this->fps;
}
void
AWDExporterSettings::set_fps(int fps)
{
	this->fps=fps;
}
int
AWDExporterSettings::get_max_iterations()
{
	return this->max_iterations;
}
void
AWDExporterSettings::set_max_iterations(int max_iterations)
{
	this->max_iterations=max_iterations;
}
bool
AWDExporterSettings::get_wide_matrix()
{
	return this->globalBlockSettings->get_wide_matrix();
}
void
AWDExporterSettings::set_wide_matrix(bool wideMatrix)
{
	this->globalBlockSettings->set_wide_matrix(wideMatrix);
}
bool
AWDExporterSettings::get_wide_geom()
{
	return this->globalBlockSettings->get_wide_geom();
}
void
AWDExporterSettings::set_wide_geom(bool wideGeom)
{
	this->globalBlockSettings->set_wide_geom(wideGeom);
}
bool
AWDExporterSettings::get_wide_props()
{
	return this->globalBlockSettings->get_wide_props();
}
void
AWDExporterSettings::set_wide_props(bool wideProps)
{
	this->globalBlockSettings->set_wide_props(wideProps);
}
bool
AWDExporterSettings::get_wide_attributes()
{
	return this->globalBlockSettings->get_wide_attributes();
}
void
AWDExporterSettings::set_wide_attributes(bool wideAttributes)
{
	this->globalBlockSettings->set_wide_attributes(wideAttributes);
}
double
AWDExporterSettings::get_scale()
{
	return this->globalBlockSettings->get_scale();
}
void
AWDExporterSettings::set_scale(double scale)
{
	this->globalBlockSettings->set_scale(scale);
}
