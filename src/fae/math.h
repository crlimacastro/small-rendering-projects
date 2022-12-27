#pragma once
#include <raylib.h>

namespace fae::math
{
	float lerp(float a, float b, float x) { return a + x * (b - a); }
	Vector2 lerp(Vector2 a, Vector2 b, float t) { return { lerp(a.x, b.x, t), lerp(a.y, b.y, t) }; }
	Vector3 lerp(Vector3 a, Vector3 b, float t) { return { lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t) }; }
}