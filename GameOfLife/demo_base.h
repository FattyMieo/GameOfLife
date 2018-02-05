#ifndef _DEMO_BASE_H
#define _DEMO_BASE_H

//! Demo base class.
class DemoBase
{
public:
	virtual void init(int windowWidth, int windowHeight) = 0;
	virtual void deinit() = 0;
	virtual void draw() = 0;
};

#endif
