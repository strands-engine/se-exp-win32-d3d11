#include "error_logger.h"

#include <comdef.h>

#include "string_converter.h"

namespace d3dexp::jpres
{
	void error_logger::log(std::string const& message)
	{
		auto errmsg = std::string("Error: ") + message;
		MessageBoxA(NULL, errmsg.c_str(), "Error", MB_ICONERROR);
	}

	void error_logger::log(HRESULT hr, std::string const& message)
	{
		auto err = _com_error{ hr };
		auto errmsg = std::wstring(L"Error: ") + string_converter::to_wide(message) + L"\n" + err.ErrorMessage();
		MessageBoxW(NULL, errmsg.c_str(), L"Error", MB_ICONERROR);
	}

	void error_logger::log(HRESULT hr, std::wstring const& message)
	{
		auto err = _com_error{ hr };
		auto errmsg = std::wstring(L"Error: ") + message + L"\n" + err.ErrorMessage();
		MessageBoxW(NULL, errmsg.c_str(), L"Error", MB_ICONERROR);
	}
}