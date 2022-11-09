#pragma once
#include "fae.h"
namespace fae
{
	struct ActiveCamera2D
	{
		Camera2D* camera = nullptr;
	};

	void setup_camera2d(const void*, entt::registry& reg)
	{
		reg.ctx().emplace<ActiveCamera2D>();
	}

	void begin_active_camera2d(const void*, entt::registry& reg)
	{
		auto& active = reg.ctx().at<ActiveCamera2D>();
		if (!active.camera) return;
		BeginMode2D(*active.camera);
	}

	void end_camera2d(const void*, entt::registry& reg)
	{
		EndMode2D();
	}

	void camera2d_plugin(const void*, entt::registry& reg)
	{
		auto& app = reg.ctx().at<application&>();
		app.systems.preStart.emplace(setup_camera2d);
		app.systems.preUpdate.emplace(begin_active_camera2d);
		app.systems.postUpdate.emplace(end_camera2d);
	}
}