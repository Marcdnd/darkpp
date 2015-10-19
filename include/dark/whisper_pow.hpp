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

#ifndef DARK_WHISPER_POW_HPP
#define DARK_WHISPER_POW_HPP

#include <cstdint>
#include <vector>

namespace dark {

    /**
     * Implements Proof-of-Work.
     */
    class whisper_pow
    {
        public:
        
            /**
             * The extra work.
             */
            enum { extra_work = 18000 };
        
            /**
             * The nonce fudge.
             */
            enum { nonce_fudge = 320 };
            
            /**
             * Generates a nonce.
             * @param val The value.
             * @param target The target.
             */
            static std::uint64_t generate_nonce(
                const std::vector<std::uint8_t> & val,
                const std::uint64_t & target
            );
    
            /**
             * Validates a nonce.
             * @param nonce The nonce.
             * @param val The value.
             */
            static bool validate_nonce(
                const std::uint64_t & nonce,
                const std::vector<std::uint8_t> & val
            );
        
            /**
             * Runs test case.
             */
            static int run_test();
        
        private:
        
            // ...

        protected:
        
            // ...
    };
    
} // namespace dark

#endif // DARK_WHISPER_POW_HPP
