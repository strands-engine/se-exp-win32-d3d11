#include "file_logging_policy.h"

namespace d3dexp::bell0bytes
{
	bool file_logging_policy::open(std::wstring const& filename)
	{
		// skipped bell0bytes windows directory shite
		m_os = std::ofstream{ filename.c_str() };
		return static_cast<bool>(m_os);
	}

	void file_logging_policy::close()
	{
		m_os.close();
	}

	void file_logging_policy::write(std::string const& message)
	{
		m_os << message;
	}
}