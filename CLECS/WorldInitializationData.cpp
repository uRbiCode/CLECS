#include "WorldInitializationData.h"
#include "EntityAdmin.h"

WorldInitializationData WorldInitializationData::Create()
{
	WorldInitializationData Data;
	Data.EntityAdminPtr = std::make_unique<EntityAdmin>();
	return Data;
}
