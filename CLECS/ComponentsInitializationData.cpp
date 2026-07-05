#include "ComponentsInitializationData.h"

const std::unordered_set<std::type_index>& ComponentsInitializationData::GetRegisteredComponents() const
{
	return Components;
}