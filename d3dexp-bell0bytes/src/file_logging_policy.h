#pragma once

#include <fstream>

#include "i_logging_policy.h"

namespace d3dexp::bell0bytes
{
	class file_logging_policy : public i_logging_policy
	{
	public:
		file_logging_policy() noexcept = default;

		file_logging_policy(file_logging_policy const&) = delete;
		file_logging_policy(file_logging_policy &&) = delete;

		file_logging_policy& operator=(file_logging_policy const&) = delete;
		file_logging_policy& operator=(file_logging_policy &&) = delete;

		~file_logging_policy() noexcept = default;

	public:
		bool open(std::wstring const& filename) override;
		void close() override;

		void write(std::string const& message) override;

	private:
		std::ofstream m_os = std::ofstream{};
	};
}