#ifndef SETTINGS_H
#define SETTINGS_H


class BlockSettings
{
    private:
        // File header fields
        bool wideMatrix;
        bool wideGeom;
        bool wideProps;
        bool wideAttributes;


        double scale;

    public:
        BlockSettings();
        ~BlockSettings();
		
        bool get_wide_matrix();
        void set_wide_matrix(bool);
        bool get_wide_geom();
        void set_wide_geom(bool);
        bool get_wide_props();
        void set_wide_props(bool);
        bool get_wide_attributes();
        void set_wide_attributes(bool);
        double get_scale();
        void set_scale(double);
};
class AWDExporterSettings 
{
    private:
		BlockSettings* globalBlockSettings;
		
		int fps;
		bool resolve_intersections;
		double curve_threshold;
		int max_iterations;
		
    public:
        AWDExporterSettings();
        ~AWDExporterSettings();
		BlockSettings* get_blockSettings();
        bool get_resolve_intersections();
        void set_resolve_intersections(bool);
        double get_curve_threshold();
        void set_curve_threshold(double);
        int get_max_iterations();
        void set_max_iterations(int);
        int get_fps();
        void set_fps(int);
		
        bool get_wide_matrix();
        void set_wide_matrix(bool);
        bool get_wide_geom();
        void set_wide_geom(bool);
        bool get_wide_props();
        void set_wide_props(bool);
        bool get_wide_attributes();
        void set_wide_attributes(bool);
        double get_scale();
        void set_scale(double);
		

};

#endif
