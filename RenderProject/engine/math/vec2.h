#pragma once
class Vec2
{
public:
	float e[2];
    Vec2() {}
    Vec2(float e0, float e1) { e[0] = e0; e[1] = e1;}
    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }

    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
};