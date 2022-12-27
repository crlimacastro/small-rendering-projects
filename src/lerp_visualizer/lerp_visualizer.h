#pragma once
#include "../fae/fae.h"
#include <string>
#include <format>

struct lerp_visualizer : public fae::application
{
	struct Slider
	{
		Vector2 position = { 0, 0 };
		float width = 128.f;
		Rectangle rectangle = { 0, 0, 8.f, 16.f };
		float value = 0.f;
	};

	struct Point
	{
		Vector3 position = { 0, 0, 0 };
		Color color = BLACK;
	};

	struct LerpPoint
	{
		Point* pointA;
		Point* pointB;
		float amount = 0;
	};

	Slider* slider;

	void setup(lerp_visualizer& app, entt::registry& reg)
	{
		auto pointAEntity = reg.create();
		auto& pointA = reg.emplace<Point>(pointAEntity, 256.f, 312.f);

		auto pointBEntity = reg.create();
		auto& pointB = reg.emplace<Point>(pointBEntity, 1024.f, 200.f);

		auto lerpPointEntity = reg.create();
		reg.emplace<Point>(lerpPointEntity, 256.f, 312.f, 1.f, RED);
		reg.emplace <LerpPoint>(lerpPointEntity, &pointA, &pointB);

		auto& window = reg.ctx().at<fae::WindowDescriptor>();
		auto sliderEntity = reg.create();
		auto& slider = reg.emplace<Slider>(sliderEntity, Vector2{ window.width * 0.5f, window.height - window.height * 0.25f }, window.width * 0.8f, Rectangle{ 0, 0, 32.f, 64.f });
		app.slider = &slider;
	}

	void draw_points(lerp_visualizer& app, entt::registry& reg)
	{
		for (auto&& [entity, point] : reg.view<const Point>().each())
		{
			DrawCircle(point.position.x, point.position.y, 10, point.color);
			auto text = std::format("({:.2f},{:.2f})", point.position.x, point.position.y);
			DrawText(text.c_str(), point.position.x - MeasureText(text.c_str(), 32) * .5f, point.position.y + 8, 32, point.color);
		}
	}

	void draw_sliders(lerp_visualizer& app, entt::registry& reg)
	{
		for (auto&& [entity, slider] : reg.view<const Slider>().each())
		{
			DrawLine(slider.position.x - slider.width * .5f, slider.position.y, slider.position.x + slider.width * .5f, slider.position.y, BLACK);
			Color rectangleColor = GRAY;
			if (CheckCollisionPointRec(GetMousePosition(), slider.rectangle))
			{
				rectangleColor = RAYWHITE;
				SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
			}
			DrawRectangleRec(slider.rectangle, rectangleColor);
			DrawRectangleLinesEx(slider.rectangle, 1.0f, BLACK);

			DrawText("0", slider.position.x - slider.width * .5f - 8, slider.position.y + slider.rectangle.height * .5f, 32, BLACK);
			DrawText("1", slider.position.x + slider.width * .5f + 8, slider.position.y + slider.rectangle.height * .5f, 32, BLACK);
			if (slider.value > 0.04f && slider.value < 0.96f)
			{
				DrawText(std::format("{:.2f}", slider.value).c_str(), slider.rectangle.x, slider.position.y + slider.rectangle.height * .5f, 32, BLACK);
			}
		}
	}

	void update_sliders(lerp_visualizer& app, entt::registry& reg)
	{
		for (auto&& [entity, slider] : reg.view<Slider>().each())
		{
			slider.rectangle.x = slider.position.x - slider.width * .5f + slider.width * slider.value - slider.rectangle.width * .5f;
			slider.rectangle.y = slider.position.y - slider.rectangle.height * .5f;

			if (CheckCollisionPointRec(GetMousePosition(), slider.rectangle) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{
				float mouseDX = GetMouseDelta().x;
				float mouseDY = GetMouseDelta().y;
				float sliderDX = mouseDX / slider.width;
				float newSliderValue = slider.value + sliderDX;
				float clampedSliderValue = Clamp(newSliderValue, 0, 1);
				slider.value = clampedSliderValue;
				SetMousePosition(GetMousePosition().x - (newSliderValue - clampedSliderValue) * slider.width, GetMousePosition().y - mouseDY);
			}
		}
	}

	void update_lerp_point(lerp_visualizer& app, entt::registry& reg)
	{
		for (auto&& [entity, point, lerp] : reg.view<Point, LerpPoint>().each())
		{
			lerp.amount = app.slider->value;
			point.position = fae::math::lerp(lerp.pointA->position, lerp.pointB->position, lerp.amount);
		}
	}

	lerp_visualizer()
	{
		registry.ctx().emplace<fae::WindowDescriptor>("Lerp Visualizer");
		plugins.emplace(fae::rendering_plugin);
		systems.start.emplace<&lerp_visualizer::setup>(*this);
		systems.update_controlled_gameobject.emplace<&lerp_visualizer::update_sliders>(*this);
		systems.update_controlled_gameobject.emplace<&lerp_visualizer::update_lerp_point>(*this);
		systems.update_controlled_gameobject.emplace<&lerp_visualizer::draw_points>(*this);
		systems.update_controlled_gameobject.emplace<&lerp_visualizer::draw_sliders>(*this);
	}
};