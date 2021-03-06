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
 
#ifndef DARK_STACK_HPP
#define DARK_STACK_HPP

#include <map>
#include <string>

namespace dark {

    class stack_impl;
    
    /**
     * The stack.
     */
    class stack
    {
        public:
        
            /**
             * Constructor
             */
            stack();
            
            /**
             * Starts the stack.
             * @param args The arguments.
             */
            void start(
                const std::map<std::string, std::string> & args =
                std::map<std::string, std::string> ()
            );
            
            /**
             * Stops the stack.
             */
            void stop();
        
            /**
             * Composes a whisper message.
             * @param pairs The pairs.
             */
            void whisper_compose(
                const std::map<std::string, std::string> & pairs
            );
        
            /**
             * Called when a status update occurs.
             * @param pairs The key/value pairs.
             */
            virtual void on_status(
                const std::map<std::string, std::string> & pairs
            );
            
        private:
        
            // ...
            
        protected:
        
            /**
             * The stack implementation.
             */
            stack_impl * stack_impl_;
    };

} // namespace dark

#endif // DARK_STACK_HPP
