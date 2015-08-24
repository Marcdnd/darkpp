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

#ifndef DARK_MIXER_HANDLER_HPP
#define DARK_MIXER_HANDLER_HPP

#include <dark/mixer.hpp>

namespace dark {

    class stack_impl;
    
    /**
     * Implements a mixer handler (base class).
     */
    class mixer_handler
    {
        public:
        
            /**
             * Constructor
             * @param owner The stack_impl.
             */
            explicit mixer_handler(stack_impl & owner);
        
            /**
             * Starts
             */
            virtual void start() = 0;
        
            /**
             * Stops
             */
            virtual void stop() = 0;
        
            /**
             * Performs mixing.
             */
            template <class T1, class T2>
            virtual bool mix(T1 & t1, T2 & t2) = 0;
        
        private:
        
            /**
             * The mixer::type.
             */
            mixer::type_t m_type;
        
        protected:
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
    };
    
} // namespace dark

#endif // DARK_MIXER_HANDLER_HPP
