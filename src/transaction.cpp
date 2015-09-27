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

#include <ctime>

#include <dark/transaction.hpp>

using namespace dark;

transaction::transaction()
    : data_buffer()
    , m_version(current_version)
    , m_time(static_cast<std::uint32_t> (std::time(0)))
    , m_time_lock(0)
{
    // ...
}

bool transaction::encode(data_buffer & buffer)
{
    return false;
}

bool transaction::decode(data_buffer & buffer)
{
    return false;
}

const std::vector<transaction::input_t> & transaction::inputs() const
{
    return m_inputs;
}

const std::vector<transaction::output_t> & transaction::outputs() const
{
    return m_outputs;
}
