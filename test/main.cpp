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

#include <map>
#include <string>

#include <boost/asio.hpp>

#include <dark/configuration.hpp>
#include <dark/stack.hpp>

#define USE_TEST_ECDHE 1

#if (defined USE_TEST_ECDHE && USE_TEST_ECDHE)
#include <dark/ecdhe.hpp>
#endif // USE_TEST_ECDHE

int main(int argc, const char * argv[])
{
#if (defined USE_TEST_ECDHE && USE_TEST_ECDHE)
    return dark::ecdhe::run_test();
#else
    std::map<std::string, std::string> args;
    
    for (auto i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == '-')
        {
            auto arg = std::string(argv[i]).substr(2, strlen(argv[i]));
            
            std::string key, value;
            
            auto i = arg.find("=");

            if (i != std::string::npos)
            {
                key = arg.substr(0, i);
                
                i = arg.find("=");
                
                if (i != std::string::npos)
                {
                    value = arg.substr(i + 1, arg.length());
                    
                    args[key] = value;
                }
            }
        }
    }
    
    boost::asio::io_service ios;

    /**
     * Allocate the dark::stack.
     */
    dark::stack s;
    
    /**
     * Set the service.
     */
    args["service"] = "whisper";
    
    /**
     * Start the dark::stack.
     */
    s.start(args);
    
    boost::asio::signal_set signals(ios, SIGINT, SIGTERM);
    signals.async_wait(std::bind(&boost::asio::io_service::stop, &ios));
    ios.run();

    /**
     * Stop the dark::stack.
     */
    s.stop();
#endif
    return 0;
}

