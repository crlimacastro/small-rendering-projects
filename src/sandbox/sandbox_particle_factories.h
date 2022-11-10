#pragma once
#include "sandbox.h"

entt::entity base_particle(entt::registry& reg)
{
	auto particleEntity = reg.create();
	reg.emplace<ParticleRenderer>(particleEntity);
	reg.emplace<ParticleTransform>(particleEntity);
	reg.emplace<ParticleRigidBody>(particleEntity);
	return particleEntity;
}

entt::entity stone(entt::registry& reg)
{
	auto particleEntity = base_particle(reg);
	reg.replace<ParticleRenderer>(particleEntity, GRAY);
	reg.replace<ParticleRigidBody>(particleEntity, 10.f);
	return particleEntity;
}

bool canMove(entt::registry& reg, entt::entity particle, int dx, int dy)
{
	auto& behavior = reg.get<const ParticleBehavior>(particle);
	auto& transform = reg.get<ParticleTransform>(particle);
	auto& rb = reg.get<const ParticleRigidBody>(particle);

	if (!behavior.grid->InBounds(transform.position.x + dx, transform.position.y + dy)) return false;
	auto other = behavior.grid->GetParticleAt(transform.position.x + dx, transform.position.y + dy);
	if (other == entt::null || !reg.valid(other)) return true;
	return rb.density > reg.get<ParticleRigidBody>(other).density;
}

entt::entity sand(entt::registry& reg)
{
	auto particle = base_particle(reg);
	reg.get<ParticleRenderer>(particle).color = BEIGE;
	reg.get<ParticleRigidBody>(particle).density = 2.f;
	reg.emplace<ParticleBehavior>(particle).onUpdate = [&](auto& reg, auto entity)
	{
		int dx = 0;
		int dy = 0;
		// if can go straight down
		if (canMove(reg, entity, 0, 1))
		{
			dy = 1;
		}
		// if can go in diagonals
		else if (canMove(reg, entity, -1, 1))
		{
			dx = -1;
			dy = 1;
		}
		else if (canMove(reg, entity, 1, 1))
		{
			dx = 1;
			dy = 1;
		}


		auto& transform = reg.get<ParticleTransform>(entity);
		auto& behavior = reg.get<const ParticleBehavior>(entity);
		auto other = behavior.grid->GetParticleAt(transform.position.x + dx, transform.position.y + dy);
		if (other != entt::null && reg.valid(other))
		{
			auto& otherTransform = reg.get<ParticleTransform>(other);
			otherTransform.position.x = transform.position.x;
			otherTransform.position.y = transform.position.y;
		}
		transform.position.x += dx;
		transform.position.y += dy;
	};
	return particle;
}

entt::entity water(entt::registry& reg)
{
	auto particle = base_particle(reg);
	reg.get<ParticleRenderer>(particle).color = BLUE;
	reg.get<ParticleRigidBody>(particle).density = 1.f;
	reg.emplace<ParticleBehavior>(particle).onUpdate = [&](auto& reg, auto entity)
	{
		int dx = 0;
		int dy = 0;
		// if can go straight down
		if (canMove(reg, entity, 0, 1))
		{
			dy = 1;
		}
		// if can go in diagonals
		else if (canMove(reg, entity, -1, 1))
		{
			dx = -1;
			dy = 1;
		}
		else if (canMove(reg, entity, 1, 1))
		{
			dx = 1;
			dy = 1;
		}
		// if can go horizontally
		else if (canMove(reg, entity, -1, 0))
		{
			dx = -1;
			dy = 0;
		}
		else if (canMove(reg, entity, 1, 0))
		{
			dx = 1;
			dy = 0;
		}


		auto& transform = reg.get<ParticleTransform>(entity);
		auto& behavior = reg.get<const ParticleBehavior>(entity);
		auto other = behavior.grid->GetParticleAt(transform.position.x, transform.position.y);
		if (other != entt::null && reg.valid(other))
		{
			auto& otherTransform = reg.get<ParticleTransform>(other);
			otherTransform.position.x = transform.position.x;
			otherTransform.position.y = transform.position.y;
		}
		transform.position.x += dx;
		transform.position.y += dy;
	};
	return particle;
}