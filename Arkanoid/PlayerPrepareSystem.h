#pragma once

struct SystemContext;

/* Responsible for presenting player with waiting for input message during run.
 * This state gives player a chance to overview level and prepare themselves.
 */
namespace PlayerPrepareSystem
{
	void Initialize(const SystemContext& Context);
	void Update(const SystemContext& Context, float DeltaTime);
}