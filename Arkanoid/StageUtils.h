#pragma once

struct StageData;
struct SystemContext;

namespace StageUtils
{
	StageData GetCurrentStageData(const SystemContext& Context);
	int GetCurrentPlayerHealth(const SystemContext& Context);
}