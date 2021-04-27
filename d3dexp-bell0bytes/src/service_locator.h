#pragma once

#include <memory>

#include "logger.h"
#include "file_logging_policy.h"

namespace d3dexp::bell0bytes
{
	class service_locator
	{
	public:
		static void initialize();

		static logger_t<file_logging_policy>* file_logger() { return s_file_logger_p.get(); }

		static void register_file_logger(std::shared_ptr<logger_t<file_logging_policy> > logger_p);

	private:
		static std::shared_ptr<logger_t<file_logging_policy> > s_file_logger_p;
	};

}