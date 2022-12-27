#pragma once
#include "../fae/fae.h"
#include <box2d/box2d.h>
#include <box2d/b2_rope.h>

struct rope_simulation : public fae::application
{
	struct Physics
	{
		b2World world{ { 0.1f, 1.0f } };
	};

	struct Rope
	{
		std::vector<b2Body*> bodies;
		std::vector<b2Body*> staticBodies;
		std::vector<b2Body*> dynamicBodies;
		std::vector<b2Joint*> joints;
	};

	void setup(rope_simulation& app, entt::registry& reg)
	{
		reg.ctx().at<fae::Renderer>().clearColor = BLACK;
		reg.ctx().emplace<Physics>();
	}

	void setup_rope_grid(rope_simulation& app, entt::registry& reg)
	{
		auto& physics = reg.ctx().at<Physics>();

		for (size_t i = 0; i < 4; i++)
		{
			std::vector<b2Body*> bodies;
			std::vector<b2Body*> staticBodies;
			std::vector<b2Body*> dynamicBodies;

			float padTop = 128;
			float padLeft = 256;
			float spacing = 128;
			for (size_t j = 0; j < 4; j++)
			{
				b2BodyDef staticBodyDef;
				staticBodyDef.position.Set(padLeft + spacing * i, padTop + spacing * j);
				auto body = physics.world.CreateBody(&staticBodyDef);
				b2PolygonShape staticBodyShape;
				staticBodyShape.SetAsBox(1, 1);
				body->CreateFixture(&staticBodyShape, 0.0f);
				bodies.push_back(body);
				staticBodies.push_back(body);

				for (size_t k = 0; k < 3; k++)
				{
					b2BodyDef dynamicBodyDef;
					dynamicBodyDef.type = b2_dynamicBody;
					dynamicBodyDef.position.Set(padLeft + spacing * i, padTop + spacing / 4 + spacing * j + spacing / 4 * k);
					auto body = physics.world.CreateBody(&dynamicBodyDef);
					b2PolygonShape dynamicBodyShape;
					dynamicBodyShape.SetAsBox(1, 1);
					b2FixtureDef dynamicBodyFixtureDef;
					dynamicBodyFixtureDef.shape = &dynamicBodyShape;
					dynamicBodyFixtureDef.density = 1.0f;
					dynamicBodyFixtureDef.friction = 0.1f;
					body->CreateFixture(&dynamicBodyFixtureDef);
					bodies.push_back(body);
					dynamicBodies.push_back(body);
				}
			}

			auto ropeEntity = reg.create();
			reg.emplace<Rope>(ropeEntity, bodies, staticBodies, dynamicBodies, std::vector<b2Joint*>{});
		}
	}

	void setup_rope_joints(rope_simulation& app, entt::registry& reg)
	{
		auto& physics = reg.ctx().at<Physics>();
		for (auto&& [entity, rope] : reg.view<Rope>().each())
		{
			for (size_t i = 1; i < rope.bodies.N(); i++)
			{
				b2RevoluteJointDef jointDef;
				jointDef.Initialize(rope.bodies[i - 1], rope.bodies[i], rope.bodies[i - 1]->GetWorldCenter());
				//jointDef.enableMotor = true;
				auto joint = physics.world.CreateJoint(&jointDef);
				rope.joints.push_back(joint);
			}
		}
	}

	void update_physics(rope_simulation& app, entt::registry& reg)
	{
		auto& physics = reg.ctx().at<Physics>();
		float timeStep = 1.0f / 60.0f;
		physics.world.Step(timeStep, 6, 2);
	}

	void draw_ropes(rope_simulation& app, entt::registry& reg)
	{
		auto& physics = reg.ctx().at<Physics>();
		for (auto&& [entity, rope] : reg.view<const Rope>().each())
		{
			for (auto& body : rope.staticBodies)
			{
				DrawCircle(body->GetPosition().x, body->GetPosition().y, 16, WHITE);
			}
			for (auto& joint : rope.joints)
			{
				DrawLineEx({ joint->GetBodyA()->GetPosition().x, joint->GetBodyA()->GetPosition().y }, { joint->GetBodyB()->GetPosition().x, joint->GetBodyB()->GetPosition().y }, 8, WHITE);
			}
			/*for (auto& body : rope.dynamicBodies)
			{
				DrawRectangle(body->GetPosition().x - 4, body->GetPosition().y - 4, 8, 8, BLUE);
			}*/
		}
	}

	void destroy_ropes_with_mouse(rope_simulation& app, entt::registry& reg)
	{
		auto& physics = reg.ctx().at<Physics>();

		for (auto&& [entity, rope] : reg.view<Rope>().each())
		{
			std::vector<b2Body*> deletedBodies;
			for (auto& staticBody : rope.staticBodies)
			{
				Vector2 bodyPosition = { staticBody->GetPosition().x, staticBody->GetPosition().y };
				if (CheckCollisionPointCircle(GetMousePosition(), bodyPosition, 16))
				{
					DrawCircle(bodyPosition.x, bodyPosition.y, 16, RED);
					if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
					{
						deletedBodies.push_back(staticBody);
					}
				}
			}

			std::vector<b2Joint*> deletedJoints;

			for (auto& body : deletedBodies)
			{
				for (auto& joint : rope.joints)
				{
					if (joint->GetBodyA() == body || joint->GetBodyB() == body) {
						deletedJoints.push_back(joint);
					}
				}
				rope.staticBodies.erase(std::remove(rope.staticBodies.begin(), rope.staticBodies.end(), body), rope.staticBodies.end());
			}

			for (auto& joint : deletedJoints)
			{
				rope.joints.erase(std::remove(rope.joints.begin(), rope.joints.end(), joint), rope.joints.end());
			}

			for (auto& joint : deletedJoints)
			{
				physics.world.DestroyJoint(joint);
			}

			for (auto& body : deletedBodies)
			{
				physics.world.DestroyBody(body);
			}
		}
	}

	rope_simulation()
	{
		registry.ctx().emplace<fae::WindowDescriptor>("Rope Simulation (Box2D)");
		plugins.emplace(fae::rendering_plugin);
		systems.start.emplace<&rope_simulation::setup>(*this);
		systems.start.emplace<&rope_simulation::setup_rope_grid>(*this);
		systems.start.emplace<&rope_simulation::setup_rope_joints>(*this);
		systems.update_controlled_gameobject.emplace<&rope_simulation::update_physics>(*this);
		systems.update_controlled_gameobject.emplace<&rope_simulation::draw_ropes>(*this);
		systems.update_controlled_gameobject.emplace<&rope_simulation::destroy_ropes_with_mouse>(*this);
	}
};