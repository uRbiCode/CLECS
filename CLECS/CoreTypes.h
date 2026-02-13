#pragma once
#include <cstdint>

namespace CLECS
{
	// ResultType defines the possible outcomes of an operation in CLECS.
	enum class ResultType : uint8_t
	{
		Success,
		Failure,
		Quit
	};
}