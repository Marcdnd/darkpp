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

#ifndef DARK_MIXER_TCP_ACCEPTOR_HPP
#define DARK_MIXER_TCP_ACCEPTOR_HPP

#include <boost/asio.hpp>

#include <dark/mixer.hpp>

namespace dark {

    class stack_impl;
    
    /**
     * Implements a mixer_tcp_acceptor.
     */
    class mixer_tcp_acceptor
    {
        public:
        
            /**
             * Constructor
             * @param ios The boost::asio::io_service.
             */
            explicit mixer_tcp_acceptor(boost::asio::io_service & ios);
        
            /**
             * Starts
             * @param type The type_t.
             */
            void start(const mixer::type_t & type);
        
            /**
             * Stops
             */
            void stop();
        
        private:
        
            /**
             * The type.
             */
            mixer::type_t m_type;
        
        protected:
        
            /**
             * The boost::asio::io_service.
             */
            boost::asio::io_service & io_service_;
        
            /**
             * The boost::asio::strand.
             */
            boost::asio::strand & strand_;
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
    };
    
} // namespace dark

#endif // DARK_MIXER_TCP_ACCEPTOR_HPP
