#pragma once
#include "fae.h"
namespace fae
{
	struct WindowDescriptor
	{
		const char* title = "";
		int width = 1280;
		int height = 720;
	};

	struct Renderer
	{
		Color clearColor = RAYWHITE;
	};

	void setup_rendering(const void*, entt::registry& reg)
	{
		WindowDescriptor descriptor;
		if (reg.ctx().contains<WindowDescriptor>())
		{
			descriptor = reg.ctx().at<WindowDescriptor>();
		}
		InitWindow(descriptor.width, descriptor.height, descriptor.title);
		reg.ctx().emplace<Renderer>();
	}

	void stop_app_on_window_close(const void*, entt::registry& reg)
	{
		if (WindowShouldClose())
		{
			auto& app = reg.ctx().at<application&>();
			app.isRunning = false;
		}
	}

	void begin_rendering(const void*, entt::registry& reg)
	{
		BeginDrawing();
		auto& renderer = reg.ctx().at<Renderer>();
		ClearBackground(renderer.clearColor);
	}

	void end_rendering(const void*, entt::registry& reg)
	{
		EndDrawing();
	}

	void cleanup_renderer(const void*, entt::registry& reg)
	{
		CloseWindow();
	}

	void rendering_plugin(const void*, entt::registry& reg)
	{
		auto& app = reg.ctx().at<application&>();
		app.systems.preStart.emplace(setup_rendering);
		app.systems.preUpdate.emplace(stop_app_on_window_close);
		app.systems.preUpdate.emplace(begin_rendering);
		app.systems.postUpdate.emplace(end_rendering);
		app.systems.postStop.emplace(cleanup_renderer);
	}
}