#pragma once
#include <vector>

template <typename Component>
struct Column
{
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

	const std::vector<Component>& GetData() { return Data; }

private:
	std::vector<Component> Data;
};