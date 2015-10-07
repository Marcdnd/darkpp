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

#ifndef DARK_WHISPER_MESSAGE_HPP
#define DARK_WHISPER_MESSAGE_HPP

#include <cstdint>
#include <ctime>
#include <map>
#include <string>
#include <vector>

namespace dark {

    /**
     * Implements a whisper message.
     */
    class whisper_message
    {
        public:
        
            /**
             * Constructor
             */
            whisper_message();
        
            /**
             * Constructor
             * @param query_string The query string.
             */
            whisper_message(const std::string & query_string);
        
            /**
             * Encodes
             * @param shared_secret The shared secret.
             */
            bool encode(const std::string & shared_secret);
        
            /**
             * Decodes
             * @param shared_secret The shared secret.
             */
            bool decode(const std::string & shared_secret);
        
            /**
             * The transaction id (encoded in network byte order).
             */
            const std::uint64_t & tid() const;
        
        private:
        
            /**
             * Parses a query.
             * @param val The value.
             * @param pairs_all The pairs.
             * @param pairs_public The public pairs.
             */
            void parse_query(
                const std::string & val,
                std::map<std::string, std::string> & pairs_all,
                std::map<std::string, std::string> & pairs_public
            );

            /**
             * The transaction id (encoded in network byte order).
             */
            std::uint64_t m_tid;
        
            /**
             * The timestamp (encoded in network byte order).
             */
            std::int64_t m_timestamp;
        
            /**
             * The flags (encoded in network byte order).
             */
            std::uint64_t m_flags;
        
            /**
             * The public key (of the recipient).
             */
            std::vector<std::uint8_t> m_public_key_recipient;
        
            /**
             * The public key (of the sender).
             */
            std::vector<std::uint8_t> m_public_key_sender;
        
            /**
             * The text.
             */
            std::string m_text;
        
            /**
             * The signature.
             */
            std::vector<std::uint8_t> m_signature;
            
            /**
             * The encoded query string.
             */
            std::string m_query_string;
        
        protected:
        
            // ...
    };
    
} // namespace dark

#endif // DARK_WHISPER_MESSAGE_HPP
