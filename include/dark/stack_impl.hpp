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

#ifndef DARK_STACK_IMPL_HPP
#define DARK_STACK_IMPL_HPP

#include <cstdint>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

namespace dark {
    
    class stack;
    class mixer_manager;
    class whisper_manager;
    
    /**
     * The stack implementation.
     */
    class stack_impl
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack.
             */
            stack_impl(dark::stack &);
            
            /**
             * Starts the stack.
             * @param args The arguments.
             */
            void start(
                const std::map<std::string, std::string> & args
            );
        
            /**
             * Stops the stack.
             */
            void stop();
        
            /**
             * The boost::asio::io_service.
             */
            boost::asio::io_service & io_service();
        
            /**
             * The boost::asio::strand.
             */
            boost::asio::strand & strand();
        
            /**
             * The mixer_manager.
             */
            std::shared_ptr<mixer_manager> & get_mixer_manager();
        
        private:
        
            /**
             * The mixer_manager.
             */
            std::shared_ptr<mixer_manager> m_mixer_manager;
        
            /**
             * The whisper_manager.
             */
            std::shared_ptr<whisper_manager> m_whisper_manager;
        
            /**
             * The boost::asio::io_service.
             */
            boost::asio::io_service m_io_service;
        
            /**
             * The boost::asio::strand.
             */
            boost::asio::strand m_strand;
        
        protected:

            /**
             * Parses the command line args into the configuration.
             * @param args The arguments.
             */
            void parse_command_line_args(
                const std::map<std::string, std::string> & args
            );
        
            /**
             * The main loop.
             */
            void loop();

            /**
             * The stack.
             */
            dark::stack & stack_;
        
            /**
             * The boost::asio::io_service::work.
             */
            std::shared_ptr<boost::asio::io_service::work> work_;
        
            /**
             * The thread.
             */
            std::vector< std::shared_ptr<std::thread> > threads_;
    };
    
} // namespace dark

#endif // DARK_STACK_IMPL_HPP
