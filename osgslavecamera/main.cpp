#include <osgDB/ReadFile>
#include <osg/ArgumentParser>
#include <osg/ApplicationUsage>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>
#include "RenderColorToTexture.h"
#include "DisplayTexture.h"

#include "wtypes.h"
#include <iostream>
using namespace std;

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

RenderColorToTexture* createSlaveCamera(int x, int y, int width, int
	height, int totalWidth, int totalHeight)
{
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new
		osg::GraphicsContext::Traits;
	traits->screenNum = 0; // this can be changed for
	//multi-display
	traits->x = x;
	traits->y = y;
	traits->width = width;
	traits->height = height;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext:: createGraphicsContext(traits.get());
	if (!gc) return NULL;
	osg::ref_ptr<RenderColorToTexture> camera = new RenderColorToTexture();
	camera->setGraphicsContext(gc.get());
	camera->setViewport(new osg::Viewport(0, 0, totalWidth, totalHeight));
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(buffer);
	camera->setReadBuffer(buffer);
	return camera.release();
}

int main(int argc, char **argv)
{
    osg::setNotifyLevel(osg::WARN);
    osg::ArgumentParser arguments(&argc,argv);

	
    osgViewer::Viewer viewer;
	auto cameraManipulator = new osgGA::TrackballManipulator();
    viewer.setCameraManipulator(cameraManipulator);
    viewer.addEventHandler(new osgViewer::ThreadingHandler());
    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
    viewer.addEventHandler(new osgViewer::StatsHandler());

    osg::Node * scene = osgDB::readNodeFiles(arguments);
    if(!scene){
        scene = osgDB::readNodeFile("cow.osg");
    }
    viewer.setSceneData(scene);

    int screenWidth = 1024;
    int screenHeight = 768;

	GetDesktopResolution(screenWidth, screenHeight);

	bool needResetMainVP = false;
	int totalWidth = screenWidth;
	int totalHeight = screenHeight;

	int xResolution = 14096;
	int yResolution = 14096;

	int numColumns = xResolution / screenWidth;
	int colRemainder = xResolution % screenWidth;
	if (colRemainder != 0) {
		numColumns++;
	}
	int numRows = yResolution / screenHeight;
	int rowRemainder = yResolution % screenHeight;
	if (rowRemainder != 0) {
		numRows++;
	}

	totalWidth = xResolution / double(numColumns);
	totalHeight = xResolution / double(numRows);

	int tileWidth = screenWidth / numColumns;
	int tileHeight = screenHeight / numRows;
	char buf[100];	

	const int maxSlaves = 10;
	int slaves  = 0;
	std::vector<osg::Matrixd> projOffsets;
	std::vector<std::string> picNames;
	std::vector<RenderColorToTexture*> rtts;
	for (int _row = 0; _row < numRows; ++_row)
	{
		for (int _col = 0; _col < numColumns; ++_col)
		{
			
			osg::Matrix projOffset =
				osg::Matrix::scale(numColumns, numRows, 1.0) *
				osg::Matrix::translate(numColumns - 1 - 2 * _col,
					numRows - 1 - 2 * _row, 0.0);

			projOffsets.push_back(projOffset);
			sprintf(buf, "%d-%d", numRows - _row - 1, _col);
			picNames.push_back(buf);

			if (slaves < maxSlaves) {
				auto camera = createSlaveCamera(
					tileWidth*_col, screenHeight - tileHeight * (_row + 1),
					tileWidth, tileHeight, totalWidth, totalHeight);
				
				camera->setPicName(buf);
				rtts.push_back(camera);
				viewer.addSlave(camera, projOffset, osg::Matrix(), true);
				slaves++;
			}
		}
	}
    viewer.realize();

	int frameId = 0;
    while(!viewer.done())
    {		
		auto currentNum = frameId * maxSlaves;
		for (int i = 0; i < maxSlaves && currentNum < projOffsets.size(); i++, currentNum++) {				
			auto& rtt = rtts[i];
			rtt->setPicName(picNames[currentNum]);
			viewer.getSlave(i)._projectionOffset = projOffsets[currentNum];
		}

		viewer.frame();
		frameId++;
		if (currentNum >= projOffsets.size()) {			
			return 0;
		}
    }
    return 0;
}
