//
// GraphicalUI.cpp
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <stdlib.h>

#ifndef COMMAND_LINE_ONLY

#include <FL/fl_ask.H>
#include "debuggingView.h"

#include "GraphicalUI.h"
#include "../RayTracer.h"
#include <iostream>
#include "../fileio/bitmap.h"

using namespace std;

bool GraphicalUI::stopTrace = false;
bool GraphicalUI::doneTrace = true;

//------------------------------------- Help Functions --------------------------------------------
GraphicalUI* GraphicalUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (GraphicalUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void GraphicalUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	GraphicalUI* pUI=whoami(o);
	
	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			stopTracing();	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
		pUI->m_debuggingWindow->m_debuggingView->setDirty();

		if( lastFile != 0 && strcmp(newfile, lastFile) != 0 )
			pUI->m_debuggingWindow->m_debuggingView->resetCamera();

		pUI->m_debuggingWindow->redraw();
	}
}

void GraphicalUI::cb_load_cubemap(Fl_Menu_* o, void* v)
{
	GraphicalUI* pUI=whoami(o);
	pUI->m_cubeMapWindow->show();
}

void GraphicalUI::cb_load_cubemapPosX(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI *)(o->user_data());

	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Cubemap", "*.bmp", NULL );
	int width, height;
	unsigned char* data;
	if (newfile != NULL) {

		data =  readBMP(newfile, width, height);
		pUI->setBackBufPosX(data);
		pUI->setBackWidthPosX(width);
		pUI->setBackHeightPosX(height);

        if ( pUI->getBackBufPosX()==NULL )
        {
                fl_alert("Can't load bitmap file");
        } else {
                pUI->setUseCubemap(0x1);
        }
	}
}
void GraphicalUI::cb_load_cubemapNegX(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI *)(o->user_data());

	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Cubemap", "*.bmp", NULL );
	int width, height;
	unsigned char* data;
	if (newfile != NULL) {

		data =  readBMP(newfile, width, height);
		pUI->setBackBufNegX(data);
		pUI->setBackWidthNegX(width);
		pUI->setBackHeightNegX(height);

        if ( pUI->getBackBufNegX()==NULL )
        {
                fl_alert("Can't load bitmap file");
        } else {
                pUI->setUseCubemap(0x2);
        }
	}
}
void GraphicalUI::cb_load_cubemapPosY(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI *)(o->user_data());

	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Cubemap", "*.bmp", NULL );
	int width, height;
	unsigned char* data;
	if (newfile != NULL) {

		data =  readBMP(newfile, width, height);
		pUI->setBackBufPosY(data);
		pUI->setBackWidthPosY(width);
		pUI->setBackHeightPosY(height);

        if ( pUI->getBackBufPosY()==NULL )
        {
                fl_alert("Can't load bitmap file");
        } else {
                pUI->setUseCubemap(0x4);
        }
	}
}
void GraphicalUI::cb_load_cubemapNegY(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI *)(o->user_data());

	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Cubemap", "*.bmp", NULL );
	int width, height;
	unsigned char* data;
	if (newfile != NULL) {

		data =  readBMP(newfile, width, height);
		pUI->setBackBufNegY(data);
		pUI->setBackWidthNegY(width);
		pUI->setBackHeightNegY(height);

        if ( pUI->getBackBufNegY()==NULL )
        {
                fl_alert("Can't load bitmap file");
        } else {
                pUI->setUseCubemap(0x8);
        }
	}
}
void GraphicalUI::cb_load_cubemapPosZ(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI *)(o->user_data());

	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Cubemap", "*.bmp", NULL );
	int width, height;
	unsigned char* data;
	if (newfile != NULL) {

		data =  readBMP(newfile, width, height);
		pUI->setBackBufPosZ(data);
		pUI->setBackWidthPosZ(width);
		pUI->setBackHeightPosZ(height);

        if ( pUI->getBackBufPosZ()==NULL )
        {
                fl_alert("Can't load bitmap file");
        } else {
                pUI->setUseCubemap(0x10);
        }
	}
}
void GraphicalUI::cb_load_cubemapNegZ(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI *)(o->user_data());

	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Scene?", "*.bmp", NULL );
	int width, height;
	unsigned char* data;
	if (newfile != NULL) {

		data =  readBMP(newfile, width, height);
		pUI->setBackBufNegZ(data);
		pUI->setBackWidthNegZ(width);
		pUI->setBackHeightNegZ(height);

        if ( pUI->getBackBufNegZ()==NULL )
        {
                fl_alert("Can't load bitmap file");
        } else {
                pUI->setUseCubemap(0x20);
        }
	}
}

void GraphicalUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	GraphicalUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void GraphicalUI::cb_exit(Fl_Menu_* o, void* v)
{
	GraphicalUI* pUI=whoami(o);

	// terminate the rendering
	stopTracing();

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
	pUI->m_cubeMapWindow->hide();
	pUI->m_debuggingWindow->hide();
}

void GraphicalUI::cb_exit2(Fl_Widget* o, void* v) 
{
	GraphicalUI* pUI=(GraphicalUI *)(o->user_data());
	
	// terminate the rendering
	stopTracing();

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
	pUI->m_cubeMapWindow->hide();
	pUI->m_debuggingWindow->hide();
}

void GraphicalUI::cb_exit3(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI *)(o->user_data());

	pUI->m_cubeMapWindow->hide();
}

void GraphicalUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS384g Fall 2005.");
}

void GraphicalUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());

	// terminate the rendering so we don't get crashes
	stopTracing();

	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
	// Need to call traceSetup before trying to render
	pUI->raytracer->setReady(false);
}

void GraphicalUI::cb_treeDepthSlides(Fl_Widget* o, void* v) {
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	pUI->treeHeight =int( ((Fl_Slider *)o)->value() );
}

void GraphicalUI::cb_leafSizeSlides(Fl_Widget* o, void* v) {

	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	pUI->leafSize =int( ((Fl_Slider *)o)->value() );
}


void GraphicalUI::cb_antiAliasSlides(Fl_Widget* o, void* v) {

	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	pUI->m_antiAlias =int( ((Fl_Slider *)o)->value() );
}

void GraphicalUI::cb_NoiseAlphaSlides(Fl_Widget* o, void* v) {

	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	pUI->m_noiseAlpha =float( ((Fl_Slider *)o)->value() );
}
void GraphicalUI::cb_NoiseSpacingSlides(Fl_Widget* o, void* v) {

	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	pUI->m_noiseSpacing =float( ((Fl_Slider *)o)->value() );
}
void GraphicalUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	stopTracing();
	pUI->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
	pUI->raytracer->setReady(false);
	pUI->raytracer->setDepth(pUI->m_nDepth);

}

void GraphicalUI::cb_debuggingDisplayCheckButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	pUI->m_displayDebuggingInfo = (((Fl_Check_Button*)o)->value() == 1);
	if( pUI->m_displayDebuggingInfo )
		pUI->m_debuggingWindow->show();
	else
		pUI->m_debuggingWindow->hide();
}



void GraphicalUI::cb_smoothShadeCheckButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	pUI->m_smoothShade = (((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_procTextCheckButton(Fl_Widget* o, void* v)
{
	GraphicalUI* pUI=(GraphicalUI*)(o->user_data());
	pUI->m_procText = (((Fl_Check_Button*)o)->value() == 1);

	if(!pUI->m_procText)
	{
		pUI->m_noiseAlphaSlider->deactivate();
		pUI->m_noiseSpacingSlider->deactivate();
	}
	else
	{
		pUI->m_noiseAlphaSlider->activate();
		pUI->m_noiseSpacingSlider->activate();
	}
}


void* GraphicalUI::thread_func(void *arg) {
	param *p = (param*)arg;
	int id = p->id;
	int num_threads = p->num_threads;
	GraphicalUI *obj = (GraphicalUI*)p->pUI;
	obj->run_thread(id,num_threads);
}

void GraphicalUI::run_thread(int id,int num_threads) {
	int width=getSize();
	int	height = (int)(width / raytracer->aspectRatio() + 0.5);

	for (int y=id; y<height; y+=num_threads) {
		for (int x=0; x<width; x++) {
		/*	if (stopTrace) break;


				// refresh
				m_traceGlWindow->refresh();
				Fl::check();

				if (Fl::damage()) {
					Fl::flush();
				}
*/

			raytracer->tracePixel( x, y );
			m_debuggingWindow->m_debuggingView->setDirty();
		}
		if (stopTrace) break;

		// refresh at end of row
	//	m_traceGlWindow->refresh();
	//	Fl::check();

	//	if (Fl::damage()) {
			Fl::flush();
	//	}

		// update the window label
		//sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
		//pUI->m_traceGlWindow->label(buffer);

	}
}

void GraphicalUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	GraphicalUI* pUI=((GraphicalUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();
		pthread_t *threads;
		pthread_attr_t pthread_custom_attr;
		pthread_attr_init(&pthread_custom_attr);
		int num_threads = 1;
        threads=(pthread_t *)malloc(num_threads*sizeof(*threads));
        param *p = new param[num_threads];
        doneTrace = false;
        stopTrace = false;

        pUI->raytracer->emitPhotons();
        cout<<"done";
    	for(int i=0;i<num_threads;i++) {
                    p[i].id=i;
                    p[i].num_threads = num_threads;
                    p[i].pUI = pUI;
                    pthread_create(&threads[i], &pthread_custom_attr, thread_func,(void *)(p+i));
            }
            for (int i=0; i<num_threads; i++)
            {
                    void *thread_change;
                    pthread_join(threads[i],&thread_change);
            }


		doneTrace=true;
		stopTrace=false;

		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void GraphicalUI::cb_stop(Fl_Widget* o, void* v)
{
	stopTrace = true;
}

int GraphicalUI::run()
{
	Fl::visual(FL_DOUBLE|FL_INDEX);

	m_mainWindow->show();

	return Fl::run();
}

void GraphicalUI::alert( const string& msg )
{
	fl_alert( "%s", msg.c_str() );
}

void GraphicalUI::setRayTracer(RayTracer *tracer)
{
	TraceUI::setRayTracer( tracer );
	m_traceGlWindow->setRayTracer(tracer);
	m_debuggingWindow->m_debuggingView->setRayTracer(tracer);
}

// menu definition
Fl_Menu_Item GraphicalUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)GraphicalUI::cb_load_scene },
		{ "&Load Cubemap...",FL_ALT + 'c', (Fl_Callback *)GraphicalUI::cb_load_cubemap},
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)GraphicalUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)GraphicalUI::cb_exit },
		{ 0 },

		{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)GraphicalUI::cb_about },
		{ 0 },

	{ 0 }
};

void GraphicalUI::stopTracing()
{
	if( stopTrace ) return;			// Only one person can be waiting at a time

	stopTrace = true;

	// Wait for the trace to finish (simple synchronization)
	while( !doneTrace )	Fl::wait();
}

GraphicalUI::GraphicalUI() {
	// init.

	m_mainWindow = new Fl_Window(100, 40, 350, 310, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// install depth slider
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(1);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install size slider
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(800);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);


		// install kd tree depth slider
		m_TreeDepthSlider = new Fl_Value_Slider(10, 90, 180, 20, "Tree Depth");
		m_TreeDepthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_TreeDepthSlider->type(FL_HOR_NICE_SLIDER);
		m_TreeDepthSlider->labelfont(FL_COURIER);
		m_TreeDepthSlider->labelsize(12);
		m_TreeDepthSlider->minimum(5);
		m_TreeDepthSlider->maximum(30);
		m_TreeDepthSlider->step(1);
		m_TreeDepthSlider->value(treeHeight);
		m_TreeDepthSlider->align(FL_ALIGN_RIGHT);
		m_TreeDepthSlider->callback(cb_treeDepthSlides);



		// install kd tree depth slider
		m_leafSizeSlider = new Fl_Value_Slider(10, 120, 180, 20, "Leaf Size");
		m_leafSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_leafSizeSlider->type(FL_HOR_NICE_SLIDER);
		m_leafSizeSlider->labelfont(FL_COURIER);
		m_leafSizeSlider->labelsize(12);
		m_leafSizeSlider->minimum(5);
		m_leafSizeSlider->maximum(30);
		m_leafSizeSlider->step(1);
		m_leafSizeSlider->value(leafSize);
		m_leafSizeSlider->align(FL_ALIGN_RIGHT);
		m_leafSizeSlider->callback(cb_leafSizeSlides);


		m_antiAliasSlider = new Fl_Value_Slider(10, 150, 180, 20, "Anti Aliasing Samples");
		m_antiAliasSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_antiAliasSlider->type(FL_HOR_NICE_SLIDER);
		m_antiAliasSlider->labelfont(FL_COURIER);
		m_antiAliasSlider->labelsize(12);
		m_antiAliasSlider->minimum(1);
		m_antiAliasSlider->maximum(4);
		m_antiAliasSlider->step(1);
		m_antiAliasSlider->value(m_antiAlias);
		m_antiAliasSlider->align(FL_ALIGN_RIGHT);
		m_antiAliasSlider->callback(cb_antiAliasSlides);

		// set up debugging display checkbox
        m_debuggingDisplayCheckButton = new Fl_Check_Button(0, 280, 180, 20, "Debugging display");
		m_debuggingDisplayCheckButton->user_data((void*)(this));
		m_debuggingDisplayCheckButton->callback(cb_debuggingDisplayCheckButton);
		m_debuggingDisplayCheckButton->value(m_displayDebuggingInfo);



		m_SmoothShadeCheckButton = new Fl_Check_Button(200, 280, 180, 20, "Smooth Shade");
		m_SmoothShadeCheckButton->user_data((void*)(this));
		m_SmoothShadeCheckButton->callback(cb_smoothShadeCheckButton);
		m_SmoothShadeCheckButton->value(m_smoothShade);


		m_ProcTextCheckButton = new Fl_Check_Button(0, 200, 180, 20, "Procedural Textures");
		m_ProcTextCheckButton->user_data((void*)(this));
		m_ProcTextCheckButton->callback(cb_procTextCheckButton);
		m_ProcTextCheckButton->value(m_procText);

		m_noiseAlphaSlider = new Fl_Value_Slider(0, 220, 180, 20, "Noise Alpha");
		m_noiseAlphaSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_noiseAlphaSlider->type(FL_HOR_NICE_SLIDER);
		m_noiseAlphaSlider->labelfont(FL_COURIER);
		m_noiseAlphaSlider->labelsize(12);
		m_noiseAlphaSlider->minimum(0);
		m_noiseAlphaSlider->maximum(3);
		m_noiseAlphaSlider->step(0.1);
		m_noiseAlphaSlider->value(m_noiseAlpha);
		m_noiseAlphaSlider->align(FL_ALIGN_RIGHT);
		m_noiseAlphaSlider->callback(cb_NoiseAlphaSlides);
		m_noiseAlphaSlider->deactivate();

		m_noiseSpacingSlider = new Fl_Value_Slider(0, 240, 180, 20, "3DText Spacing");
		m_noiseSpacingSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_noiseSpacingSlider->type(FL_HOR_NICE_SLIDER);
		m_noiseSpacingSlider->labelfont(FL_COURIER);
		m_noiseSpacingSlider->labelsize(12);
		m_noiseSpacingSlider->minimum(0);
		m_noiseSpacingSlider->maximum(10);
		m_noiseSpacingSlider->step(0.5);
		m_noiseSpacingSlider->value(m_noiseSpacing);
		m_noiseSpacingSlider->align(FL_ALIGN_RIGHT);
		m_noiseSpacingSlider->callback(cb_NoiseSpacingSlides);
		m_noiseSpacingSlider->deactivate();

		// set up "render" button
		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		// set up "stop" button
		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

    // Load Cubemap
	m_cubeMapWindow = new Fl_Window(100, 40, 200, 200, "Load Cubemap");
	m_cubeMapWindow->user_data((void*)(this));	// record self to be used by static callback functions

	m_PosX = new Fl_Button (12, 50,70,50,"+X");
	m_PosX->user_data((void*)(this));
	m_PosX->callback(cb_load_cubemapPosX);

	m_NegX = new Fl_Button (92, 50,70,50,"-X");
	m_NegX->user_data((void*)(this));
	m_NegX->callback(cb_load_cubemapNegX);

	m_PosY = new Fl_Button (12, 100,70,50,"+Y");
	m_PosY->user_data((void*)(this));
	m_PosY->callback(cb_load_cubemapPosY);

	m_NegY = new Fl_Button (92, 100,70,50,"-Y");
	m_NegY->user_data((void*)(this));
	m_NegY->callback(cb_load_cubemapNegY);

	m_PosZ = new Fl_Button (12, 150,70,50,"+Z");
	m_PosZ->user_data((void*)(this));
	m_PosZ->callback(cb_load_cubemapPosZ);

	m_NegZ = new Fl_Button (92, 150,70,50,"-Z");
	m_NegZ->user_data((void*)(this));
	m_NegZ->callback(cb_load_cubemapNegZ);

	m_cubeMapWindow->callback(cb_exit3);
	m_cubeMapWindow->when(FL_HIDE);
	m_cubeMapWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);

	// debugging view
	m_debuggingWindow = new DebuggingWindow();
}

#endif

