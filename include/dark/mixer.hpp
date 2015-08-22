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

#ifndef DARK_MIXER_HPP
#define DARK_MIXER_HPP

namespace dark {

    class mixer_tcp_acceptor;
    class stack_impl;
    
    /**
     * Implements a mixer.
     */
    class mixer
    {
        public:
        
            /**
             * The mixer types.
             */
            typedef enum type_s
            {
                type_none,
                type_cj01,
            } type_t;
        
            /**
             * Constructor
             * @param owner The stack_impl.
             */
            explicit mixer(stack_impl & owner);
        
            /**
             * Starts
             * @param type The type_t.
             */
            void start(const type_t & type);
        
            /**
             * Stops
             */
            void stop();
        
        private:
        
            /**
             * The type.
             */
            type_t m_type;
        
            /** 
             * The mixer_tcp_acceptor.
             */
            std::shared_ptr<mixer_tcp_acceptor> m_mixer_tcp_acceptor;
        
        protected:
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
    };
    
} // namespace dark

#endif // DARK_MIXER_HPP
