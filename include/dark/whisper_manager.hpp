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

#ifndef DARK_WHISPER_MANAGER_HPP
#define DARK_WHISPER_MANAGER_HPP

#include <map>
#include <string>
#include <vector>

namespace dark {

    class stack_impl;
    class whisper_message;
    
    /**
     * Implements a whisper manager.
     */
    class whisper_manager
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack_impl.
             */
            explicit whisper_manager(stack_impl & owner);
        
            /**
             * Starts
             */
            void start();
        
            /**
             * Stops
             */
            void stop();
        
            /**
             * Composes a message.
             * @param pairs The pairs.
             */
            void compose(const std::map<std::string, std::string> & pairs);
        
        private:
        
            /**
             * The whisper messages.
             */
            std::map<
                std::string,
                std::vector<std::shared_ptr<whisper_message> > >
                m_whisper_messages
            ;
        
        protected:
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
    };
    
} // namespace dark

#endif // DARK_WHISPER_MANAGER_HPP
