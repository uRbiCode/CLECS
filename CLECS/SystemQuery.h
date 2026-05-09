#pragma once
#include "System.h"
#include "EntityAdmin.h"

template<typename WritesTag, typename ReadsTag>
class SystemQuery;

template<typename... Ts> struct Writes {};
template<typename... Ts> struct Reads {};

template<typename... WriteTypes, typename... ReadTypes>
class SystemQuery<Writes<WriteTypes...>, Reads<ReadTypes...>>
{
public:
	explicit SystemQuery(EntityAdmin& Admin) : AdminPtr(&Admin) {}

	template<typename Func>
	void ForEach(Func&& Function)
	{
		AdminPtr->GetGroup<WriteTypes..., ReadTypes...>().ForEach(std::forward<Func>(Function));
	}

	static std::vector<ComponentAccess> GetAccess()
	{
		std::vector<ComponentAccess> Result;

		(void)std::initializer_list<int>
		{
			(Result.push_back(ComponentAccess{ typeid(WriteTypes), ComponentAccessMode::Write }), 0)...
		};

		(void)std::initializer_list<int>
		{
			(Result.push_back(ComponentAccess{ typeid(ReadTypes), ComponentAccessMode::Read }), 0)...
		};

		return Result;
	}

private:
	EntityAdmin* AdminPtr;
};