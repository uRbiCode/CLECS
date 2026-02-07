#include "WorldInitializationData.h"
#include "EntityManager.h"

WorldInitializationData WorldInitializationData::Create()
{
	WorldInitializationData Data;
	Data.EntityManagerPtr = std::make_unique<EntityManager>();
	return Data;
}
