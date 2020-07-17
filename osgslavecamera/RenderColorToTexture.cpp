#include "RenderColorToTexture.h"

#include <osg/Camera>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/ReaderWriter>

//namespace {
	using namespace osg;
	class myDrawCallback : public osg::Camera::DrawCallback
	{
	public:
		myDrawCallback(RenderColorToTexture* rtt)
			: _tex(rtt)
		{

		}
		virtual void operator () (osg::RenderInfo& renderInfo) const override
		{
			if (!_tex->_done) {
				osg::State * state = renderInfo.getState();
				state->applyAttribute(_tex->_texture);
				osg::Image *image = new Image();
				image->readImageFromCurrentTexture(renderInfo.getContextID(), false, GL_UNSIGNED_BYTE);
				char buf[150];
				snprintf(buf, 150, "%s.png", _tex->_picName.c_str());

				osgDB::writeImageFile(*image, buf);
				//_tex->_picNumber++;
				_tex->_done = true;
			}
		}

		RenderColorToTexture * _tex;
	};
//}

RenderColorToTexture::RenderColorToTexture()
{
    setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    setRenderOrder(osg::Camera::PRE_RENDER);
    setName("RenderColor");

    _texture = new osg::Texture2D();
    _texture->setSourceFormat(GL_RGBA);
    _texture->setInternalFormat(GL_RGBA32F_ARB);
    _texture->setSourceType(GL_FLOAT);
    attach(osg::Camera::COLOR_BUFFER0, _texture);

	setPostDrawCallback(new myDrawCallback(this));
}
