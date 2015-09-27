/*
 * Copyright (c) 2013-2015 John Connor (BM-NC49AxAjcqVcF5jNPu85Rb8MJ2d9JqZt)
 *
 * This file is part of Dark++.
 *
 * Dark++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef DARK_LOGGER_HPP
#define DARK_LOGGER_HPP
  
#if (defined _WIN32 || defined WIN32) || (defined _WIN64 || defined WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // (defined _WIN32 || defined WIN32) || (defined _WIN64 || defined WIN64)

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>

#include <mutex>

namespace dark {

    /**
     * Implements a logger.
     */
    class logger
    {
        public:
            
			/**
			 * @param severity_none
			 * @param severity_debug
			 * @param severity_error
			 * @param severity_info
			 * @param severity_warning
			 */
			typedef enum severity
			{
				severity_none,
				severity_debug,
				severity_error,
				severity_info,
				severity_warning,
			} severity_t;
			
            /**
             * Singleton accessor.
             */
			static logger & instance()
			{
			    static logger g_logger;

			    return g_logger;
			}
            
            /**
             * operator <<
             */
            template <class T>
            logger & operator << (T const & val)
            {
                std::stringstream ss;
                
                ss << val;
                
                log(ss);
                
                ss.str(std::string());
                
                return logger::instance();
            }

            /**
             * Perform the actual logging.
             * @param val
             */
			void log(std::stringstream & val)
			{
				std::lock_guard<std::recursive_mutex> lock(mutex_);

			    static bool use_cout = true;

			    if (use_cout)
			    {
#if (defined _WIN32 || defined WIN32) || (defined _WIN64 || defined WIN64)
#if defined(_UNICODE)
			        DWORD len = MultiByteToWideChar(
			            CP_ACP, 0, val.str().c_str(), -1, NULL, 0
			        );

			        std::unique_ptr<wchar_t> buf(new wchar_t[len]);

			        MultiByteToWideChar(
			            CP_ACP, 0, val.str().c_str(), -1, buf.get(), len
			        );

			        OutputDebugString(buf.get());
			        OutputDebugString(L"\n");

			        std::cerr << val.str() << std::endl;
#else
			        OutputDebugString(val.str().c_str());
			        OutputDebugString(L"\n");

			        std::cerr << val.str() << std::endl;
#endif // _UNICODE
#else // Not Windows.
#endif // defined _WIN32 || defined WIN32) || (defined _WIN64 || defined WIN64
			    }
			}
            
        private:
        
			// ...
            
        protected:
        
			/**
			 * The mutex.
			 */
			std::recursive_mutex mutex_;
    };
    
    #define log_xx(severity, strm) \
    { \
        std::time_t time_now = std::chrono::system_clock::to_time_t( \
            std::chrono::system_clock::now()); \
        std::string time_str = std::ctime(&time_now); \
        time_str.pop_back(), time_str.pop_back(); \
        time_str.pop_back(), time_str.pop_back(); \
        time_str.pop_back(), time_str.pop_back(); \
        std::stringstream __ss; \
        switch (severity) \
        { \
            case dark::logger::severity_debug: \
                __ss << time_str << " database[DEBUG] - "; \
            break; \
            case dark::logger::severity_error: \
                __ss << time_str << " database[ERROR] - "; \
            break; \
            case dark::logger::severity_info: \
                __ss << time_str << " database[INFO] - "; \
            break; \
            case dark::logger::severity_warning: \
                __ss << time_str << " database[WARNING] - "; \
            break; \
            default: \
                __ss << std::ctime(&time_now) << " database[UNKNOWN] - "; \
        } \
		__ss << __FUNCTION__ << ": "; \
        __ss << strm; \
        dark::logger::instance() << __ss.str(); \
        __ss.str(std::string()); \
    } \
	
#define log_none(strm) /** */
#if (defined NDEBUG && !defined DEBUG)
#define log_debug(strm) log_none(strm)
#else
#define log_debug(strm) log_xx(dark::logger::severity_debug, strm)
#endif
#define log_error(strm) log_xx(dark::logger::severity_error, strm)
#define log_info(strm) log_xx(dark::logger::severity_info, strm)
#define log_warn(strm) log_xx(dark::logger::severity_warning, strm)

} // namespace dark

#endif // DARK_LOGGER_HPP

        