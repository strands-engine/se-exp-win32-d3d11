#pragma once

#include <mutex>
#include <chrono>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <Windows.h>

// HORRIBLE IN SO MANY WAYS - DO NOT USE !!!

namespace d3dexp::bell0bytes
{
	enum class severity_t
	{
		info,
		debug,
		warning,
		error
	};


	template <typename LoggingPolicy>
	class logger_t;

	template <typename LoggingPolicy>
	void logging_daemon(logger_t<LoggingPolicy>* logger)
	{
		const auto delay = std::chrono::milliseconds{ 50 };
		std::unique_lock<std::timed_mutex> lock(logger->write_mutex(), std::defer_lock);

		do
		{
			std::this_thread::sleep_for(delay);
			if (!logger->is_buffer_empty())
			{
				if (!lock.try_lock_for(delay))
				{
					continue;
				}
				for (auto& msg : logger->buffer())
				{
					logger->write(msg);
				}
				logger->clear_buffer();
				lock.unlock();
			}
		} while (logger->is_running().test_and_set() || logger->is_buffer_empty());
	}

	template <typename LoggingPolicy>
	class logger_t
	{
	public:
		logger_t(std::wstring const& name)
		{
			if (m_logging_policy.open(name))
			{
				m_is_running.test_and_set();
				m_daemon_thread = std::move(std::thread{ logging_daemon<LoggingPolicy>, this });
			}
			else
			{
				throw std::runtime_error("Unable to open file for writing for file logger.");
			}
		}

		logger_t(logger_t const&) = delete;
		logger_t(logger_t &&) = delete;

		logger_t& operator=(logger_t const&) = delete;
		logger_t& operator=(logger_t &&) = delete;

		~logger_t() 
		{
			// daemon thread termnation
			m_is_running.clear();
			m_daemon_thread.join();

			// closing output stream
			m_logging_policy.close();
		}

	public:
		[[nodiscard]] std::vector<std::string> const& buffer() { return m_buffer; }
		[[nodiscard]] bool is_buffer_empty() const noexcept { return m_buffer.empty(); }
		[[nodiscard]] std::timed_mutex& write_mutex() noexcept { return m_write_mutex; }
		[[nodiscard]] std::atomic_flag& is_running() noexcept { return m_is_running; }

		void set_thread_name(std::string const& thread_name) { m_thread_names[std::this_thread::get_id()] = thread_name; }

		void write(std::string const& message) { m_logging_policy.write(message); }

		template <severity_t SEVERITY>
		void log(std::stringstream ss) 
		{
			auto log_ss = std::stringstream{};

			// get time
			auto sys_time = SYSTEMTIME{};
			GetLocalTime(&sys_time);

			// header: line number and date (x: xx/xx/xxxx xx:xx:xx)
			if (m_line_number != 0)
			{
				log_ss << "\r\n";
			}
			log_ss << m_line_number++ << ": " 
				   << sys_time.wDay << "/" << sys_time.wMonth << "/" << sys_time.wYear << " " 
				   << sys_time.wHour << ":" << sys_time.wMinute << ":" << sys_time.wSecond << "\t";

			// write down warning level
			switch (SEVERITY)
			{
			case severity_t::info:
				log_ss << "INFO:    ";
				break;
			case severity_t::debug:
				log_ss << "DEBUG:   ";
				break;
			case severity_t::warning:
				log_ss << "WARNING: ";
				break;
			case severity_t::error:
				log_ss << "ERROR:   ";
				break;
			default:
				log_ss << "         ";			
				break;
			};

			// write thread name
			log_ss << m_thread_names[std::this_thread::get_id()] << ":\t";

			// write the actual message
			log_ss << ss.str();
			std::lock_guard<std::timed_mutex> lock(m_write_mutex);
			m_buffer.push_back(log_ss.str());
		}

		template <severity_t SEVERITY>
		void log(std::string const& msg)
		{
			auto ss = std::stringstream{};
			ss << msg;
			log<SEVERITY>(std::stringstream{ ss.str() });
		}

		void clear_buffer()
		{
			m_buffer.clear();
		}

	private:
		std::unordered_map<std::thread::id, std::string> m_thread_names;
		std::vector<std::string> m_buffer;
		std::timed_mutex m_write_mutex;
		LoggingPolicy m_logging_policy;
		std::thread m_daemon_thread;
		std::atomic_flag m_is_running{ ATOMIC_FLAG_INIT };
		unsigned int m_line_number = 0u;
	};
}
