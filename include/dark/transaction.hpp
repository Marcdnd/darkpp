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

#ifndef DARK_TRANSACTION_HPP
#define DARK_TRANSACTION_HPP

#include <cstdint>
#include <vector>

#include <dark/script.hpp>
#include <dark/sha256.hpp>

namespace dark {

    /**
     * Implements a transaction.
     */
    class transaction
    {
        public:
        
            /**
             * A point out.
             */
            typedef struct point_out_s
            {
                /**
                 * The hash.
                 */
                sha256 hash;
            
                /**
                 * The n.
                 */
                std::uint32_t n;
                
            } point_out_t;
        
            /**
             * An input.
             */
            typedef struct input_s
            {
                /**
                 * The previous out.
                 */
                point_out_t previous_out;
                
                /**
                 * The script signature.
                 */
                script script_signature;
                
                /**
                 * The sequence.
                 */
                std::uint32_t sequence;
            
            } input_t;
        
            /**
             * An output.
             */
            typedef struct output_s
            {
                /**
                 * The value.
                 */
                std::int64_t value;
            
                /**
                 * The script public key.
                 */
                script script_public_key;
                
            } output_t;
        
            /**
             * The inputs.
             */
            const std::vector<input_t> & inputs() const;
        
            /**
             * The outputs.
             */
            const std::vector<output_t> & outputs() const;
        
        private:
        
            /**
             * The inputs.
             */
            std::vector<input_t> m_inputs;
        
            /**
             * The outputs.
             */
            std::vector<output_t> m_outputs;
        
        protected:
        
            // ...
    };
    
} // namespace dark

#endif // DARK_TRANSACTION_HPP
