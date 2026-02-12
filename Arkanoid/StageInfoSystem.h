#pragma once
#include "System.h"

// Displays information about current stage
class StageInfoSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void InitializeStageInfo(const SystemContext& Context) const;
	void RefreshStageInfo(const SystemContext& Context) const;
};