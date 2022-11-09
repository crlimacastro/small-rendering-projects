#pragma once
#include "fae.h"
namespace fae
{
	struct application
	{
		struct systems
		{
			entt::organizer preStart;
			entt::organizer start;
			entt::organizer postStart;

			entt::organizer preUpdate;
			entt::organizer update;
			entt::organizer postUpdate;

			entt::organizer preStop;
			entt::organizer stop;
			entt::organizer postStop;
		};

		bool isRunning = false;
		entt::registry registry;
		entt::organizer plugins;
		systems systems;

		application& start()
		{
			isRunning = true;
			registry.ctx().emplace<application&>(*this);
			for (auto&& node : plugins.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			for (auto&& node : systems.preStart.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			for (auto&& node : systems.start.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			for (auto&& node : systems.postStart.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			return *this;
		}
		application& update()
		{
			for (auto&& node : systems.preUpdate.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			for (auto&& node : systems.update.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			for (auto&& node : systems.postUpdate.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			return *this;
		}
		application& stop()
		{
			isRunning = false;
			for (auto&& node : systems.preStop.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			for (auto&& node : systems.stop.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			for (auto&& node : systems.postStop.graph())
			{
				node.prepare(registry);
				node.callback()(NULL, registry);
			}
			return *this;
		}

		application& run()
		{
			start();
			while (isRunning)
			{
				update();
			}
			stop();
			return *this;
		}
	};
}