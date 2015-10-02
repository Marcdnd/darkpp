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

#include <dark/whisper_message.hpp>

using namespace dark;

bool whisper_message::encode()
{
    m_query_string = "tid=" + std::to_string(m_tid);
    m_query_string = "timestamp=" + std::to_string(m_timestamp);
    m_query_string = "flags=" + std::to_string(m_flags);
    //m_query_string = "recipient=" + base64_encode(m_public_key_recipient);

    // m_query_string = "sender=" + base64_encode(m_public_key_sender);
    // m_query_string = "text=" + base64_encode(ecdhe_encrypt(m_text));

   // m_query_string = "signature=" + m_signature.to_string();
    
    m_query_string += "&_l=900";
    
    return true;
}

bool whisper_message::decode()
{
    /**
     * :TODO:
     */
     
    return true;
}

const std::uint64_t & whisper_message::tid() const
{
    return m_tid;
}
