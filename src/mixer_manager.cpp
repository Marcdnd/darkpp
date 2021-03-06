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

#include <dark/mixer.hpp>
#include <dark/mixer_manager.hpp>

using namespace dark;

mixer_manager::mixer_manager(stack_impl & owner)
    : stack_impl_(owner)
{
    // ...
}

void mixer_manager::start()
{
    /**
     * Allocate the mixer.
     */
    m_mixer.reset(new mixer(stack_impl_));
    
    /**
     * Start the mixer (mixer::type_cj01).
     */
    m_mixer->start(mixer::type_cj01, 44444);
}

void mixer_manager::stop()
{
    if (m_mixer)
    {
        m_mixer->stop();
    }
    
    m_mixer.reset();
}
