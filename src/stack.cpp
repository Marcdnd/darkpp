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

#include <dark/stack.hpp>
#include <dark/stack_impl.hpp>

using namespace dark;

stack::stack()
    : stack_impl_(0)
{
    // ...
}

void stack::start(const std::map<std::string, std::string> & args)
{
    if (stack_impl_)
    {
        throw std::runtime_error("Stack is already allocated");
    }
    else
    {
        /**
         * Allocate the stack implementation.
         */
        stack_impl_ = new stack_impl(*this);

        /**
         * Start the stack implementation.
         */
        stack_impl_->start(args);
    }
}

void stack::stop()
{
    if (stack_impl_)
    {
        /**
         * Stop the stack implementation.
         */
        stack_impl_->stop();
        
        /**
         * Deallocate the stack implementation.
         */
        delete stack_impl_, stack_impl_ = 0;
    }
    else
    {
        throw std::runtime_error("Stack is not allocated");
    }
}

void stack::whisper_compose(const std::map<std::string, std::string> & pairs)
{
    if (stack_impl_)
    {
        stack_impl_->whisper_compose(pairs);
    }
}

void stack::on_status(const std::map<std::string, std::string> & pairs)
{
    // ...
}
