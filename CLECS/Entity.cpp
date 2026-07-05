#include "Entity.h"

Entity::Entity(EntityId Id) : Identifier(Id)
{
}

EntityId Entity::GetId() const
{
	return Identifier;
}

bool Entity::operator==(Entity Other) const
{
	return Identifier == Other.Identifier;
}

bool Entity::operator!=(Entity Other) const
{
	return Identifier != Other.Identifier;
}