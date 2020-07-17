#pragma once
#include <osg/Camera>
#include <osg/Texture2D>

class DisplayTexture : public osg::Camera
{
public:
    DisplayTexture(osg::Texture2D * tex);

    virtual ~DisplayTexture(){}

	//void setTexture(osg::Texture2D * tex);
	osg::Texture2D * _tex;

};
