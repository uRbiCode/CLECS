#pragma once

struct SystemContext;

/* Displays UI information about current stage.
 * So basically the stage number at the bottom right.
 */
namespace StageInfoSystem
{
	void Initialize(const SystemContext& Context);
}