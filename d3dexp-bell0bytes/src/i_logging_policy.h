#pragma once

#include <string>

namespace d3dexp::bell0bytes
{
	class i_logging_policy
	{
	public:
		virtual ~i_logging_policy() noexcept = default;

		virtual bool open(std::wstring const& name) = 0;
		virtual void close() = 0;
		virtual void write(std::string const& message) = 0;
	};
}