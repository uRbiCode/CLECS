#pragma once
#include "System.h"

struct Entity;

/* Responsible for presenting player with waiting for input message during run.
 * This state gives player a chance to overview level and prepare themselves.
 */
class PlayerPrepareSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	void AddPrepareMessage(const SystemContext& Context) const;
	void CleanupPrepareMessage(const SystemContext& Context) const;

	void ChangeMessageVisibility(const SystemContext& Context, const Entity& Entity) const;
	bool ShouldCleanupPrepareMessage(const SystemContext& Context) const;
};