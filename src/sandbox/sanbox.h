#pragma once
#include "../fae/fae.h"

/// <summary>
/// The classic sandbox simulation with cellular automata
/// </summary>
struct sandbox : public fae::application
{
	// components
public:
	struct Position
	{
		int x, y;
	};

	struct Particle
	{
		Color color = WHITE;
		float density = 1.f;
		std::function<void(entt::registry&, entt::entity)> onUpdate;
		bool operator ==(const Particle& rhs) const { return color.r == rhs.color.r && color.g == rhs.color.g && color.b == rhs.color.b && color.a == rhs.color.a; }
	};

	const Particle STONE{ GRAY, 9999.f, [&](auto&, auto) {} };
	const Particle SAND{ BEIGE, 2.f, [&](entt::registry& reg, entt::entity entity) {
		auto& grid = reg.ctx().at<Grid>();
		auto& position = reg.get<Position>(entity);
		auto& particle = reg.get<Particle>(entity);
		auto inBounds = [&](int dx, int dy)
		{
			return grid.InBounds(position.x + dx, position.y + dy);
		};
		auto canMove = [&](int dx, int dy)
		{
			if (!inBounds(dx, dy)) return false;
			entt::entity entity = grid.posToParticle[position.x + dx][position.y + dy];
			if (entity == entt::null || !reg.valid(entity)) return true;
			return particle.density > reg.get<Particle>(entity).density;
		};

		int dx = 0;
		int dy = 1;
		// if can't go straight down
		if (!canMove(dx, dy))
		{
			dx = -1;
		}
		// if can't go in diagonals
		if (!canMove(dx, dy))
		{
			dx = -dx;
		}
		if (!canMove(dx, dy))
		{
			dx = 0;
			dy = 0;
		}

		position.x += dx;
		position.y += dy;
	}
	};
	const Particle WATER{ BLUE, 1.f, [&](entt::registry& reg, entt::entity entity) {
		auto& grid = reg.ctx().at<Grid>();
		auto& position = reg.get<Position>(entity);
		auto& particle = reg.get<Particle>(entity);
		auto inBounds = [&](int dx, int dy)
		{
			return grid.InBounds(position.x + dx, position.y + dy);
		};
		auto canMove = [&](int dx, int dy)
		{
			if (!inBounds(dx, dy)) return false;
			entt::entity entity = grid.posToParticle[position.x + dx][position.y + dy];
			if (entity == entt::null || !reg.valid(entity)) return true;
			return particle.density > reg.get<Particle>(entity).density;
		};

		int dx = 0;
		int dy = 1;
		// if can't go straight down
		if (!canMove(dx, dy))
		{
			dx = -1;
		}
		// if can't go in diagonals
		if (!canMove(dx, dy))
		{
			dx = -dx;
		}
		if (!canMove(dx, dy))
		{
			dx = -1;
			dy = 0;
		}
		// if can't go horizontally
		if (!canMove(dx, dy))
		{
			dx = -dx;
		}
		if (!canMove(dx, dy))
		{
			dx = 0;
		}

		position.x += dx;
		position.y += dy;
	} };

	// resources
public:
	struct Grid
	{
		bool debugDrawGrid = false;
		int particleSize = 1;
		int gridSize = 256;
		std::vector<std::vector<entt::entity>> posToParticle;

		Vector2 ScreenToGrid(int x, int y) const
		{
			return { (float)x / particleSize, (float)y / particleSize };
		}

		Vector2 GridToScreen(int x, int y) const
		{
			return { (float)x * particleSize, (float)y * particleSize };
		}

		bool InBounds(int x, int y) const
		{
			return x >= 0 &&
				x < posToParticle.size()
				&& y >= 0 &&
				y < posToParticle[x].size();
		}

		entt::entity GetParticle(int x, int y) const
		{
			if (!InBounds(x, y)) return entt::null;
			return posToParticle[x][y];
		}
	};

public:
	Camera2D camera;
	Particle selectedParticle = STONE;
	size_t selectionSize = 1;

	// systems

	void update_particle_selection(sandbox& app, entt::registry& reg)
	{
		if (IsKeyReleased(KEY_ONE))
		{
			app.selectedParticle = app.STONE;
		}
		else if (IsKeyReleased(KEY_TWO))
		{
			app.selectedParticle = app.SAND;
		}
		else if (IsKeyReleased(KEY_THREE))
		{
			app.selectedParticle = app.WATER;
		}
	}

	void create_selected_particle_with_mouse(sandbox& app, entt::registry& reg)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			auto& grid = reg.ctx().at<Grid>();
			auto mouseScreenPos = GetMousePosition();
			auto mouseGridPos = grid.ScreenToGrid(mouseScreenPos.x, mouseScreenPos.y);
			if (!grid.InBounds(mouseGridPos.x, mouseGridPos.y)) return;

			// destroy particle if any existing there
			auto particle = grid.GetParticle(mouseGridPos.x, mouseGridPos.y);
			if (particle != entt::null && reg.valid(particle)) reg.destroy(particle);
			// create particle at mouse position
			// TODO make diamond shape around selection size
			//for (size_t i = 0; i < selectionSize; i++)
			//{
			//	for (size_t j = 0; j < selectionSize; j++)
			//	{
			//		// TODO
			//	}
			//}
			auto particleEntity = reg.create();
			reg.emplace<Position>(particleEntity, (int)mouseGridPos.x, (int)mouseGridPos.y);
			reg.emplace<Particle>(particleEntity, app.selectedParticle);
		}
	}

	void delete_particle_with_mouse(sandbox& app, entt::registry& reg)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
			auto& grid = reg.ctx().at<Grid>();
			auto mouseScreenPos = GetMousePosition();
			auto mouseGridPos = grid.ScreenToGrid(mouseScreenPos.x, mouseScreenPos.y);
			if (!grid.InBounds(mouseGridPos.x, mouseGridPos.y)) return;
			entt::entity particle = grid.GetParticle(mouseGridPos.x, mouseGridPos.y);
			if (particle == entt::null || !reg.valid(particle)) return;
			reg.destroy(particle);
		}
	}

	void update_particles_and_grid(sandbox& app, entt::registry& reg)
	{
		auto& grid = reg.ctx().at<Grid>();
		for (auto&& [entity, position, particle] : reg.view<const Position, const Particle>().each())
		{
			grid.posToParticle[position.x][position.y] = entt::null;
			particle.onUpdate(reg, entity);
			grid.posToParticle[position.x][position.y] = entity;
		}
	}

	void resize_grid(sandbox& app, entt::registry& reg)
	{
		auto& grid = reg.ctx().at<Grid>();
		grid.posToParticle.resize(grid.gridSize);
		for (auto& col : grid.posToParticle)
		{
			col.resize(grid.gridSize, entt::null);
		}

	}

	void draw_particles(sandbox& app, entt::registry& reg)
	{
		auto& grid = reg.ctx().at<Grid>();
		for (auto&& [entity, position, particle] : reg.view<const Position, const Particle>().each())
		{
			auto screenPos = grid.GridToScreen(position.x, position.y);
			DrawRectangle(screenPos.x, screenPos.y, grid.particleSize, grid.particleSize, particle.color);
		}
	}

	void draw_debug_grid(sandbox& app, entt::registry& reg)
	{
		auto& grid = reg.ctx().at<Grid>();
		if (!grid.debugDrawGrid) return;
		rlPushMatrix();
		rlTranslatef(grid.particleSize * grid.gridSize * 0.5f, grid.particleSize * grid.gridSize * 0.5f, 0);
		rlRotatef(90, 1, 0, 0);
		DrawGrid(grid.gridSize, grid.particleSize);
		rlPopMatrix();
		rlPushMatrix();
		rlTranslatef(grid.particleSize * grid.gridSize * 0.5f, grid.particleSize * grid.gridSize, 0);
		rlRotatef(90, 1, 0, 0);
		DrawGrid(grid.gridSize, grid.particleSize);
		rlPopMatrix();
	}

	void setup(sandbox& app, entt::registry& reg)
	{
		// set clear color to black
		auto& renderer = reg.ctx().at<fae::Renderer>();
		renderer.clearColor = BLACK;

		// setup camera
		app.camera.zoom = 1.f;
		auto& active = reg.ctx().at<fae::ActiveCamera2D>();
		active.camera = &app.camera;

		// setup resources
		reg.ctx().emplace<Grid>(false, 8, 256);
	}

	sandbox() : camera()
	{
		registry.ctx().emplace<WindowDescriptor>("sandbox (cellular automata)");
		plugins.emplace(fae::rendering_plugin);
		plugins.emplace(fae::camera2d_plugin);
		systems.start.emplace<&sandbox::setup>(*this);
		systems.update.emplace<&sandbox::update_particle_selection>(*this);
		systems.update.emplace<&sandbox::create_selected_particle_with_mouse>(*this);
		systems.update.emplace<&sandbox::delete_particle_with_mouse>(*this);
		systems.update.emplace<&sandbox::update_particles_and_grid>(*this);
		systems.update.emplace<&sandbox::resize_grid>(*this);
		systems.update.emplace<&sandbox::draw_particles>(*this);
		systems.update.emplace<&sandbox::draw_debug_grid>(*this);
	}
};