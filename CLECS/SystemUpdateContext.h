#pragma once

class EntityManager;

struct SystemUpdateContext
{
	EntityManager& EntityManager;
	float DeltaTime = 0.f;
};
