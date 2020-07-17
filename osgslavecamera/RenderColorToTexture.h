#pragma once
#include <osg/Camera>
#include <osg/Texture2D>

class myDrawCallback;

class RenderColorToTexture : public osg::Camera
{
	friend class myDrawCallback;
public:
    RenderColorToTexture();
    virtual ~RenderColorToTexture(){}

    osg::Texture2D* getTexture()const
    {
        return _texture.get();
    }

	void setPicName(const std::string& str) { _picName = str; _done = false; }
private:
    osg::ref_ptr<osg::Texture2D> _texture;
	std::string _picName;
	bool _done = false;
};
