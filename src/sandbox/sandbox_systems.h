#pragma once
#include "sandbox.h"

void update_particles(const void*, entt::registry& reg)
{
	for (auto&& [entity, behavior] : reg.view<ParticleBehavior>().each())
	{
		behavior.onUpdate(reg, entity);
	}
}

void update_grids(const void*, entt::registry& reg)
{
	for (auto&& [entity, grid] : reg.view<ParticleGrid>().each())
	{
		// resize grid particle positions if necessary
		grid.posToParticle.resize(grid.N);
		for (auto& col : grid.posToParticle)
		{
			// clear particle positions
			col.clear();
			col.resize(grid.N, entt::null);
		}

		// update_controlled_gameobject positions
		for (auto& particle : grid.particles)
		{
			if (particle == entt::null || !reg.valid(particle)) continue;
			auto& transform = reg.get<const ParticleTransform>(particle);
			grid.posToParticle[transform.position.x][transform.position.y] = particle;
		}
	}
}

void draw_grids(const void*, entt::registry& reg)
{
	for (auto&& [entity, grid, gridRenderer] : reg.view<const ParticleGrid, const ParticleGridRenderer>().each())
	{
		for (auto& particle : grid.particles)
		{
			if (particle == entt::null || !reg.valid(particle)) continue;
			auto& transform = reg.get<const ParticleTransform>(particle);
			auto& particleRenderer = reg.get<const ParticleRenderer>(particle);
			auto screenPos = gridRenderer.GridToScreen(transform.position.x, transform.position.y);
			DrawRectangle(screenPos.x, screenPos.y, gridRenderer.particleSize, gridRenderer.particleSize, particleRenderer.color);
		}

		if (!gridRenderer.drawDebugGridLines) continue;
		rlPushMatrix();
		rlTranslatef(gridRenderer.particleSize * grid.N * 0.5f, gridRenderer.particleSize * grid.N * 0.5f, 0);
		rlRotatef(90, 1, 0, 0);
		DrawGrid(grid.N, gridRenderer.particleSize);
		rlPopMatrix();
	}
}



void sandbox_plugin(const void*, entt::registry& reg)
{
	auto& app = reg.ctx().at<application&>();
	app.systems.update_controlled_gameobject.emplace<update_particles>();
	app.systems.update_controlled_gameobject.emplace<update_grids>();
	app.systems.update_controlled_gameobject.emplace<draw_grids>();
}