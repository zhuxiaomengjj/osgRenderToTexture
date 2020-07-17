#include "DisplayTexture.h"
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Camera>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>
#include <osgDB/ReaderWriter>
#include <osgDB/PluginQuery>

using namespace osg;

osg::Geode * createFullscreenQuad() {
     osg::Geometry* geom = osg::createTexturedQuadGeometry(osg::Vec3(), osg::Vec3(1, 0.0f, 0.0f), osg::Vec3(0.0f, 1, 0.0f), 0.0f, 0.0f, 1.0f, 1.0f);
     osg::Geode *quad = new osg::Geode;
     quad->addDrawable(geom);
     return quad;
 }

class myDrawCallback : public osg::Camera::DrawCallback
{
public:
	myDrawCallback(osg::Texture2D *tex)
		: _tex(tex)
	{

	}
	virtual void operator () (osg::RenderInfo& renderInfo) const override
	{
		osg::State * state = renderInfo.getState();
		state->applyAttribute(_tex);
		osg::Image *image = new Image();
		image->readImageFromCurrentTexture(renderInfo.getContextID(), false, GL_UNSIGNED_BYTE);
		osgDB::writeImageFile(*image, "snap.png");
	}

	osg::Texture2D * _tex;
	bool _haveDone = false;
};



DisplayTexture::DisplayTexture(osg::Texture2D * tex)
	: _tex(tex)
{
    setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    setRenderOrder(osg::Camera::POST_RENDER);
    setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setViewMatrix(osg::Matrix());
    setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
    getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF);
    addChild(createFullscreenQuad());

    setName("Display");

	//setPostDrawCallback(new myDrawCallback(_tex));

	getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
}

//void DisplayTexture::setTexture(osg::Texture2D *tex)
//{
//     getOrCreateStateSet()->setTextureAttributeAndModes(0, tex,osg::StateAttribute::ON);
//}
