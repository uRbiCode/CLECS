#pragma once
#include "System.h"

/* Displays UI information about current stage.
 * So basically the stage number at the bottom right.
 */
class StageInfoSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	// Stage info management
	void InitializeStageInfo(const SystemContext& Context) const;
	void RefreshStageInfo(const SystemContext& Context) const;
};