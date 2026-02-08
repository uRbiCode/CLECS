#pragma once

namespace CLECS
{
	/* ResultType defines the possible outcomes of an operation in CLECS.
	 * Can be extended to include additional result states beyond success/failure.
	 */
	enum class ResultType
	{
		Success,
		Failure,
		Quit
	};
}