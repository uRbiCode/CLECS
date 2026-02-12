#pragma once
#include <nlohmann/json.hpp>
#include "UpgradeComponent.h"
#include <vector>

using Json = nlohmann::json;

class UpgradeLoader
{
public:
	static std::vector<UpgradeDefinition> LoadUpgradeDefinitions();

private:
	static UpgradeDefinition ParseUpgradeDefinition(const Json& Json);
	static Upgrade ParseUpgrade(const Json& Json);
};