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

#include <cassert>

#include <boost/algorithm/string.hpp>

#include <dark/crypto.hpp>
#include <dark/network.hpp>
#include <dark/sha256.hpp>
#include <dark/whisper.hpp>
#include <dark/whisper_message.hpp>

using namespace dark;

whisper_message::whisper_message()
{
    // ...
}

whisper_message::whisper_message(const std::string & query_string)
    : m_query_string(query_string)
{
    // ...
}

bool whisper_message::encode(const std::string & shared_secret)
{
    if (shared_secret.size() > 0)
    {
        /**
         * Set the id.
         */
        m_query_string = "id=" + std::to_string(m_tid);
        
        /**
         * Set the timestamp.
         */
        m_query_string += "&timestamp=" + std::to_string(m_timestamp);
        
        /**
         * Set the flags.
         */
        m_query_string += "&flags=" + std::to_string(m_flags);
        
        /**
         * Set the recipient.
         */
        m_query_string +=
            "&recipient=" + network::uri_encode(crypto::base64_encode(
            reinterpret_cast<const char *> (&m_public_key_recipient[0]),
            m_public_key_recipient.size()))
        ;
        
        /**
         * Set the sender.
         */
        m_query_string +=
            "&sender=" + network::uri_encode(crypto::base64_encode(
            reinterpret_cast<const char *> (&m_public_key_sender[0]),
            m_public_key_sender.size()))
        ;

        // :TODO: encrypt w/ hc256

        /**
         * Set the text.
         */
        m_query_string +=
            "&text=" + network::uri_encode(
            crypto::base64_encode(m_text.data(), m_text.size()))
        ;

        /**
         * Hash the query string.
         */
         auto hash = sha256(
            reinterpret_cast<const std::uint8_t *> (m_query_string.data()),
            m_query_string.size()
        ).to_string();
        
        /** 
         * Sign the hash.
         */
        if (
            whisper::instance().get_ecdhe().sign(
            reinterpret_cast<const std::uint8_t *> (hash.data()), hash.size(),
            m_signature)
            )
        {
            m_query_string +=
                "&signature=" + network::uri_encode(crypto::base64_encode(
                reinterpret_cast<const char *> (&m_signature[0]),
                m_signature.size()))
            ;
        
            /**
             * Set the lifetime.
             */
            m_query_string += "&_l=900";

            return true;
        }
    }
    
    return false;
}

bool whisper_message::decode(const std::string & shared_secret)
{
    assert(m_query_string.size());
    
    /**
     * :TODO:
     */
    std::map<std::string, std::string> pairs_all;
    std::map<std::string, std::string> pairs_public;
    
    parse_query(m_query_string, pairs_all, pairs_public);
    
    if (pairs_all.size() > 0 && pairs_public.size() > 0)
    {
    
    }
    
    return false;
}

const std::uint64_t & whisper_message::tid() const
{
    return m_tid;
}

void whisper_message::parse_query(
    const std::string & val, std::map<std::string, std::string> & pairs_all,
    std::map<std::string, std::string> & pairs_public
    )
{
    auto i = val.find("&");
    
    if (i != std::string::npos)
    {
        std::vector<std::string> pairs1;
        
        boost::split(pairs1, val, boost::is_any_of("&"));
        
        for (auto & i : pairs1)
        {
            std::vector<std::string> pairs2;
            
            boost::split(pairs2, i, boost::is_any_of("="));
            
            if (pairs2.size() != 2)
            {
                continue;
            }
            
            pairs_all[pairs2[0]] = network::uri_decode(pairs2[1]);
        }
    }
    else
    {
        std::vector<std::string> pairs2;
        
        boost::split(pairs2, val, boost::is_any_of("="));
        
        if (pairs2.size() == 2)
        {
            pairs_all[pairs2[0]] = network::uri_decode(pairs2[1]);
        }
    }
    
    for (auto & i : pairs_all)
    {
        if (i.first.size() == 0 || i.second.size() == 0)
        {
            continue;
        }

        /**
         * Skip "private" terms.
         */
        if (i.first.compare(0, 1, "_") == 0)
        {
            continue;
        }

        /**
         * Insert the pair.
         */
        pairs_public.insert(std::make_pair(i.first, i.second));
    }
}
