#pragma once
#include <vector>

template <typename Component>
struct Column
{
	void Reserve(size_t EntityCount)
	{
		Data.reserve(EntityCount);
	}

	template<typename... Args>
	Component& EmplaceBack(Args&&... args)
	{
		return Data.emplace_back(std::forward<Args>(args)...);
	}

	void SwapRemove(size_t Row)
	{
		std::swap(Data[Row], Data.back());
		Data.pop_back();
	}

	std::vector<Component>& AccessData() { return Data; }
	const std::vector<Component>& GetData() const { return Data; }

private:
	std::vector<Component> Data;
};