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

#include <stdexcept>

#include <dark/configuration.hpp>
#include <dark/mixer.hpp>
#include <dark/stack_impl.hpp>
#include <dark/tcp_acceptor.hpp>

using namespace dark;

mixer::mixer(stack_impl & owner)
    : m_type(type_none)
    , stack_impl_(owner)
{
    // ...
}

void mixer::start(const mixer::type_t & type, const std::uint16_t & port)
{
    /**
     * Set the type.
     */
    m_type = type;
    
    /**
     * Allocate the handler per type.
     */
    if (m_type == mixer::type_cj01)
    {
        /**
         * Allocate the tcp_acceptor.
         */
        m_tcp_acceptor = std::make_shared<tcp_acceptor> (
            stack_impl_.io_service(), stack_impl_.strand()
        );
    }
    else
    {
        throw std::runtime_error("invalid type");
    }
    
    /**
     * Start the tcp_acceptor.
     */
    if (m_tcp_acceptor)
    {
        m_tcp_acceptor->set_on_accept(
            [this] (std::shared_ptr<tcp_transport> t)
            {
                if (t)
                {
                    // :TODO:
                }
            }
        );
    
        m_tcp_acceptor->open(port);
    }
}

void mixer::stop()
{
    /**
     * Stop the tcp_acceptor.
     */
    if (m_tcp_acceptor)
    {
        m_tcp_acceptor->close();
    }
    
    m_tcp_acceptor.reset();
}
