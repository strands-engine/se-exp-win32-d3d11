#pragma once

#include <exception>
#include <typeinfo>
#include <stdexcept>

namespace d3dexp::bell0bytes
{

	// based on Alexandrescu talks: 
	//  - https://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Andrei-Alexandrescu-Systematic-Error-Handling-in-C
	// 	- https://www.youtube.com/watch?v=PH4WBuE1BHI&ab_channel=TaoZhou
	//  as interpreted by bell0bytes

	// adjustments:
	//  - get rid of from_exception - constructor enabled on classes deriving from std::exception
	//  - get rid / modify from_code
	//  - provide constructors with inplace constructible arguments -> universal references galore
	//  - free functions for raising exceptions (with inplace constructible arguments)
	//  - default constructor - placement new of default T
	//  - perf - commenting out if(invalid) rethrow / asserts? 9for *, not for value/result)
	//  - const -> version
	//  - alt returns: const RV&& fn() const &&; and RV&& fn() &&;
	//  - has value - noexcept
	//  - value_or(dflt)
	//  - condition for enabling swap (see vid #2 42:16) and changed mixed case (see vid #2 44:00)

	// final proposed version:
	//  - use int instead of bool (w/alignment little difference) to srore (context-dependent) error code
	//  - keep using exception_ptr for global catching and transfer across nothrow boundaries

	template<class T>
	class expected_t
	{
	public:
		friend class expected_t<void>;

	private:
		expected_t() {};

	public:
		expected_t(expected_t const& other) : m_has_result(other.m_has_result)
		{
			if (has_result())
			{
				new(&m_result) T(other.m_result);
			}
			else
			{
				new(&m_exception_p) std::exception_ptr(other.m_exception_p);
			}
		}
		expected_t(expected_t && other) noexcept : m_has_result(other.m_has_result)
		{
			if (has_result())
			{
				new(&m_result) T(std::move(other.m_result));
			}
			else
			{
				new(&m_exception_p) std::exception_ptr(std::move(other.m_exception_p));
			}
		}

		expected_t(T const& result) : m_result(result), m_has_result(true) {}
		expected_t(T && result) : m_result(std::move(result)), m_has_result(true) {}

		template <class E>
		expected_t<T>(E const& exception) : m_exception_p(std::make_exception_ptr(exception)) {}

		expected_t& operator=(expected_t const& rhs)
		{
			m_has_result = rhs.m_has_result;
			if (m_has_result)
			{
				new(&m_result) T(rhs.m_result);
			}
			else
			{
				new(&m_exception_p) std::exception_ptr(rhs.m_exception_p);
			}
			return *this;
		}
		expected_t& operator=(expected_t&& rhs) noexcept
		{
			m_has_result = rhs.m_has_result;
			if (m_has_result)
			{
				new(&m_result) T(std::move(rhs.m_result));
			}
			else
			{
				new(&m_exception_p) std::exception_ptr(std::move(rhs.m_exception_p));
			}
			return *this;
		}

		~expected_t() 
		{
			using std::exception_ptr;
			if (has_result())
			{
				m_result.~T();
			}
			else
			{
				m_exception_p.~exception_ptr();
			}
		};

	public:
		template<class E>
		static [[nodiscard]] expected_t from_exception(E const& exception)
		{
			if (typeid(exception) != typeid(E))
			{
				throw std::invalid_argument("slicing detected!\n");
			}
			return from_exception(std::make_exception_ptr(exception));
		}
		static [[nodiscard]] expected_t from_exception(std::exception_ptr exception_p)
		{
			auto exp = expected_t<T>{};
			new(&exp.m_exception_p) std::exception_ptr(std::move(exception_p));
			return exp;
		}
		static [[nodiscard]] expected_t from_exception()
		{
			return from_exception(std::current_exception());
		}

		// necessary ???
		template <class Func>
		static [[nodiscard]] expected_t from_code(Func fn)
		{
			try
			{
				return expected_t{ fn() };
			}
			catch (...)
			{
				return from_exception();
			}
		}

	public:
		[[nodiscard]] bool has_result() const noexcept { return m_has_result; }
		[[nodiscard]] bool is_valid() const noexcept { return m_has_result; }
		[[nodiscard]] operator bool() const noexcept { return m_has_result; }

		template<class E>
		[[nodiscard]] bool has_exception() const
		{
			try
			{
				if (!m_has_result)
				{
					std::rethrow_exception(m_exception_p);
				}
			}
			catch (const E& e)
			{
				(void)e;
				return true;
			}
			catch (...)
			{

			}
			return false;
		}

		[[nodiscard]] T& result()
		{
			if (!has_result())
			{
				std::rethrow_exception(m_exception_p);
			}
			return m_result;
		}
		[[nodiscard]] T const& result() const
		{
			if (!has_result())
			{
				std::rethrow_exception(m_exception_p);
			}
			return m_result;
		}
		[[nodiscard]] T& get() { return result(); }
		[[nodiscard]] T const& get() const { return result(); }
		[[nodiscard]] T& operator*() { return result(); }
		[[nodiscard]] T const& operator*() const { return result(); }
		[[nodiscard]] T* operator->()
		{
			if (!has_result())
			{
				std::rethrow_exception(m_exception_p);
			}
			return &m_result;
		}

		void throw_if_failed() const
		{
			if (!m_has_result)
			{
				std::rethrow_exception(m_exception_p);
			}
		}

		void swap(expected_t& other)
		{
			if (m_has_result)
			{
				if (other.m_has_result)
				{
					std::swap(m_result, other.m_result);
				}
				else
				{
					auto&& tmp = std::move(other.m_exception_p);
					new(&other.m_result) T(std::move(m_result));
					new(&m_exception_p) std::exception_ptr(tmp);
					std::swap(m_has_result, other.m_has_result);
				}
			}
			else
			{
				if (other.m_has_result)
				{
					other.swap(*this);
				}
				else
				{
					m_exception_p.swap(other.m_exception_p);
				}
			}
		}

	protected:
		union
		{
			T m_result = T{};
			std::exception_ptr m_exception_p;
		};
		bool m_has_result = false;
	};

	template<>
	class expected_t<void>
	{
	public:
		expected_t() = default;

		expected_t(expected_t const&) = default;
		expected_t(expected_t&&) = default;

		template <typename E>
		expected_t(E const& exception) : m_exception_p(std::make_exception_ptr(exception)) { }
		expected_t(std::exception_ptr exception_p) : m_exception_p(exception_p) {}

		expected_t& operator=(expected_t const&) = default;
		expected_t& operator=(expected_t&&) = default;

		~expected_t() = default;

	public:
		template<class E>
		static [[nodiscard]] expected_t<void> from_exception(E const& exception) { return expected_t<void>{exception}; }
		static [[nodiscard]] expected_t<void> from_exception(std::exception_ptr exception_p) { return expected_t<void>{exception_p}; }
		static [[nodiscard]] expected_t<void> from_exception() { return expected_t<void>{std::exception{"Unknown error!"}}; }

	public:
		[[nodiscard]] bool has_result() const noexcept { return !m_exception_p; }
		[[nodiscard]] bool is_valid() const noexcept { return!m_exception_p; }
		[[nodiscard]] operator bool() const noexcept { return !m_exception_p;}

		[[nodiscard]] void result() const
		{
			if (m_exception_p)
			{
				std::rethrow_exception(m_exception_p);
			}
		}
		[[nodiscard]] void get() const { result(); }
		[[nodiscard]] void operator*() const { result(); }
		
		template<class E>
		[[nodiscard]] bool has_exception() const noexcept
		{
			try
			{
				if (m_exception_p)
				{
					std::rethrow_exception(m_exception_p);
				}
			}
			catch (const E& e)
			{
				(void)e;
				return true;
			}
			catch (...)
			{

			}
			return false;
		}
		
		void throw_if_failed() const
		{
			result();
		}

		void swap(expected_t<void>& other)
		{
			std::swap(m_exception_p, other.m_exception_p);
		}

	private:
		std::exception_ptr m_exception_p = nullptr;

	};

}