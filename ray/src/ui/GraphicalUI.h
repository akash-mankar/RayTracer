//
// GraphicalUI.h
//
// The header file for the graphical UI
//

#ifndef __GraphicalUI_h__
#define __GraphicalUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/Fl_File_Chooser.H>		// FLTK file chooser

#include "TraceUI.h"
#include "TraceGLWindow.h"
#include "debuggingWindow.h"

class ModelerView;

class GraphicalUI : public TraceUI {
public:
	GraphicalUI();

	int run();

	void		alert( const string& msg );

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Window*          m_cubeMapWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider* 			m_TreeDepthSlider;
	Fl_Slider* 			m_leafSizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_antiAliasSlider;
	Fl_Slider* 			m_noiseAlphaSlider;
	Fl_Slider*  		m_noiseSpacingSlider;

	Fl_Check_Button*	m_debuggingDisplayCheckButton;
	Fl_Check_Button* 	m_SmoothShadeCheckButton;
	Fl_Check_Button*	m_ProcTextCheckButton;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	Fl_Button*			m_PosX;
	Fl_Button*			m_NegX;
	Fl_Button*			m_PosY;
	Fl_Button*			m_NegY;
	Fl_Button*			m_PosZ;
	Fl_Button*			m_NegZ;


	TraceGLWindow*		m_traceGlWindow;

	DebuggingWindow*	m_debuggingWindow;
	// member functions
	void		setRayTracer(RayTracer *tracer);

	static void stopTracing();
private:
	struct param {
		int id;
		int num_threads;
		GraphicalUI* pUI;

	};

	static void* thread_func(void *arg);

	void run_thread(int id,int num_threads);

	// static class members
	static Fl_Menu_Item menuitems[];

	static GraphicalUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);

	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);
	static void cb_exit3(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_treeDepthSlides(Fl_Widget* o, void* v);
	static void cb_leafSizeSlides(Fl_Widget* o, void* v);
	static void cb_antiAliasSlides(Fl_Widget* o, void* v);
	static void cb_NoiseAlphaSlides(Fl_Widget* o, void* v);
	static void cb_NoiseSpacingSlides(Fl_Widget* o, void* v) ;

	static void cb_load_cubemap(Fl_Menu_* o, void* v);
	static void cb_load_cubemapPosX(Fl_Widget* o, void* v);
	static void cb_load_cubemapNegX(Fl_Widget* o, void* v);
	static void cb_load_cubemapPosY(Fl_Widget* o, void* v);
	static void cb_load_cubemapNegY(Fl_Widget* o, void* v);
	static void cb_load_cubemapPosZ(Fl_Widget* o, void* v);
	static void cb_load_cubemapNegZ(Fl_Widget* o, void* v);


	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
	static void cb_debuggingDisplayCheckButton(Fl_Widget* o, void* v);
	static void cb_smoothShadeCheckButton(Fl_Widget* o, void* v);
	static void cb_procTextCheckButton(Fl_Widget* o, void* v);

	static bool doneTrace;		// Flag that gets set when the trace is done
	static bool stopTrace;		// Flag that gets set when the trace should be stopped
};

#endif
