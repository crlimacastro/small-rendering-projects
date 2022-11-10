#pragma once
#include "sandbox.h"

struct ParticleGrid;
struct ParticleWorld;

struct ParticleBehavior
{
	ParticleGrid* grid;
	ParticleWorld* world;
	std::function<void(entt::registry&, entt::entity)> onUpdate = [&](auto&, auto) {};
};

struct ParticleRenderer
{
	Color color = WHITE;
};

struct ParticleTransform
{
	Vector2 position = { 0, 0 };
};

struct ParticleRigidBody
{
	float density = 1.f;
	Vector2 acceleration = { 0, 0 };
	Vector2 velocity = { 0, 0 };
};

struct ParticleWorld
{
	Vector2 gravity = { 0, -9.8f };
};

struct ParticleGrid
{
	size_t size = 256;
	std::unordered_set<entt::entity> particles;

	bool InBounds(size_t x, size_t y) const { return x < size&& y < size; }

	entt::entity GetParticleAt(size_t x, size_t y) const {
		if (!InBounds(x, y)) return entt::null;
		return posToParticle.at(x).at(y);
	}

private:
	std::vector<std::vector<entt::entity>> posToParticle;
	friend void update_grids(const void*, entt::registry& reg);
};

struct ParticleGridRenderer
{
	bool drawDebugGridLines = false;
	size_t particleSize = 1;

	Vector2 ScreenToGrid(float x, float y) const
	{
		return { x / particleSize, y / particleSize };
	}

	Vector2 GridToScreen(int x, int y) const
	{
		return { static_cast<float>(x * particleSize), static_cast<float>(y * particleSize) };
	}
};