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

#include <chrono>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <database/stack.hpp>

#include <dark/whirlpool.hpp>

namespace dark {

    class stack_impl;
    class whisper_message;
    
    /**
     * Implements a whisper manager.
     */
    class whisper_manager : public database::stack
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
        
            /**
             * Called when a search result is received.
             * @param transaction_id The transaction id.
             * @param query The query.
             */
            virtual void on_find(
                const std::uint16_t & transaction_id,
                const std::string & query
            );
            
        private:
        
            /**
             * The whisper messages.
             */
            std::map<
                std::string,
                std::vector<std::shared_ptr<whisper_message> > >
                m_whisper_messages
            ;
        
            /**
             * The hash of our public key.
             */
            whirlpool m_hash_public_key;
        
        protected:
        
            /**
             * The tick handler.
             * @param interval The interval.
             */
            void do_tick(const std::uint32_t & interval);
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
        
            /**
             * The timer.
             */
            boost::asio::basic_waitable_timer<
                std::chrono::steady_clock
            > timer_;
        
            /**
             * The transacation id's used to prevent duplicate messages.
             */
            std::set<std::uint32_t> m_tids;
    };
    
} // namespace dark

#endif // DARK_WHISPER_MANAGER_HPP
