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
#include <dark/mixer_tcp_acceptor.hpp>

using namespace dark;

mixer::mixer(stack_impl & owner)
    : m_type(type_none)
    , stack_impl_(owner)
{
    // ...
}

void mixer::start(const mixer::type_t & type)
{
    /**
     * Allocate the handler per type.
     */
    if (type == mixer::type_cj01)
    {
        /**
         * Allocate the mixer_tcp_acceptor.
         */
        m_mixer_tcp_acceptor = std::make_shared<mixer_tcp_acceptor> (
            stack_impl_.io_service()
        );
    }
    else
    {
        throw std::runtime_error("invalid type");
    }
    
    /**
     * Start the mixer_tcp_acceptor.
     */
    if (m_mixer_tcp_acceptor)
    {
        m_mixer_tcp_acceptor->start(type);
    }
}

void mixer::stop()
{
    // ...
}
