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

#ifndef DARK_CRYPTO_HPP
#define DARK_CRYPTO_HPP

#include <string>

namespace dark {

    class crypto
    {
        public:
        
            /**
             * Performs base64 encoding.
             * @param buf
             * @param len
             */
            static std::string base64_encode(
                const char * buf, const std::size_t & len
            );

            /**
             * Performs base64 decoding.
             * @param buf
             * @param len
             */
            static std::string base64_decode(
                const char * buf, const std::size_t & len
            );
        
        private:
        
            // ...
        
        protected:
        
            // ...
    };
    
} // namespace dark

#endif // DARK_CRYPTO_HPP
