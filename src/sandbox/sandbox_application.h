#pragma once
#include "sandbox.h"

/// <summary>
/// The classic sandbox simulation with cellular automata
/// </summary>
struct sandbox_application : public fae::application
{
	// fields
public:
	Camera2D camera;
	ParticleGrid* grid = nullptr;
	ParticleWorld* world = nullptr;

	// resources
public:
	struct Selection
	{
		std::function<entt::entity(entt::registry& reg)> selectedParticleFactory = stone;
		size_t selection = 1;
	};

	// systems
public:
	void setup(sandbox_application& app, entt::registry& reg)
	{
		// set clear color to black
		auto& renderer = reg.ctx().at<fae::Renderer>();
		renderer.clearColor = BLACK;

		// setup camera
		/*app.camera.zoom = 1.f;
		auto& active = reg.ctx().at<fae::ActiveCamera2D>();
		active.camera = &app.camera;*/

		// setup grid & world
		auto gridEntity = reg.create();
		auto& grid = reg.emplace<ParticleGrid>(gridEntity);
		app.grid = &grid;
		reg.emplace<ParticleGridRenderer>(gridEntity, false, (size_t)8);

		auto worldEntity = reg.create();
		auto& world = reg.emplace <ParticleWorld>(worldEntity);
		app.world = &world;

		// setup resources
		reg.ctx().emplace<Selection>();
	}

	void update_particle_selection(sandbox_application& app, entt::registry& reg)
	{
		auto& selection = reg.ctx().at<Selection>();
		if (IsKeyReleased(KEY_ONE))
		{
			selection.selectedParticleFactory = stone;
		}
		else if (IsKeyReleased(KEY_TWO))
		{
			selection.selectedParticleFactory = sand;
		}
		else if (IsKeyReleased(KEY_THREE))
		{
			selection.selectedParticleFactory = water;
		}
	}

	void create_particle_on_selection(sandbox_application& app, entt::registry& reg)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			for (auto&& [entity, grid, gridRenderer] : reg.view<ParticleGrid, const ParticleGridRenderer>().each())
			{
				auto mouseScreenPos = GetMousePosition();
				auto mouseGridPos = gridRenderer.ScreenToGrid(mouseScreenPos.x, mouseScreenPos.y);
				if (!grid.InBounds(mouseGridPos.x, mouseGridPos.y)) continue;

				// destroy particle if any existing there
				auto particleToDelete = grid.GetParticleAt(mouseGridPos.x, mouseGridPos.y);
				if (particleToDelete != entt::null && reg.valid(particleToDelete))
				{
					grid.particles.erase(particleToDelete);
					reg.destroy(particleToDelete);
				}

				// create particle at mouse position
				// TODO make diamond shape around selection size
				auto& selection = reg.ctx().at<Selection>();
				auto particleEntity = selection.selectedParticleFactory(reg);
				auto& transform = reg.get<ParticleTransform>(particleEntity);
				transform.position = mouseGridPos;
				grid.particles.insert(particleEntity);
				auto particle = reg.try_get<ParticleBehavior>(particleEntity);
				if (particle)
				{
					particle->grid = app.grid;
					particle->world = app.world;
				}
			}
		}
	}

	void delete_particle_on_selection(sandbox_application& app, entt::registry& reg)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			for (auto&& [entity, grid, gridRenderer] : reg.view<ParticleGrid, const ParticleGridRenderer>().each())
			{
				auto mouseScreenPos = GetMousePosition();
				auto mouseGridPos = gridRenderer.ScreenToGrid(mouseScreenPos.x, mouseScreenPos.y);
				if (!grid.InBounds(mouseGridPos.x, mouseGridPos.y)) continue;
				auto particle = grid.GetParticleAt(mouseGridPos.x, mouseGridPos.y);
				if (particle == entt::null && !reg.valid(particle)) continue;
				grid.particles.erase(particle);
				reg.destroy(particle);
			}
		}
	}

	sandbox_application()
	{
		registry.ctx().emplace<WindowDescriptor>("Sandbox (Cellular Automata)");
		plugins.emplace(fae::rendering_plugin);
		//plugins.emplace(fae::camera2d_plugin);
		plugins.emplace(sandbox_plugin);
		systems.start.emplace<&sandbox_application::setup>(*this);
		systems.update.emplace<&sandbox_application::update_particle_selection>(*this);
		systems.update.emplace<&sandbox_application::create_particle_on_selection>(*this);
		systems.update.emplace<&sandbox_application::delete_particle_on_selection>(*this);
	}
};