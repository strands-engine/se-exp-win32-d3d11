#include "service_locator.h"

namespace d3dexp::bell0bytes
{
	std::shared_ptr<logger_t<file_logging_policy> > service_locator::s_file_logger_p{};

	void service_locator::initialize()
	{
		// create and register file logger (and set current thread name
		//auto logger_p = std::make_shared<logger_t<file_logging_policy> >(L"d3dexp_bell0bytes.log");
		//logger_p->set_thread_name("main thread");
		//register_file_logger(logger_p);
#ifdef _DEBUG
		//file_logger()->log<severity_t::info>("File logger initialized!");
#endif //_DEBUG

	}

	void service_locator::register_file_logger(std::shared_ptr<logger_t<file_logging_policy>> logger_p)
	{
		s_file_logger_p = logger_p;
	}
}