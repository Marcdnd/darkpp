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

#include <boost/algorithm/string.hpp>

#include <dark/configuration.hpp>
#include <dark/mixer_manager.hpp>
#include <dark/stack.hpp>
#include <dark/stack_impl.hpp>
#include <dark/status_manager.hpp>
#include <dark/whisper_manager.hpp>

using namespace dark;

stack_impl::stack_impl(dark::stack & owner)
    : m_strand(m_io_service)
    , stack_(owner)
{
    // ...
}

void stack_impl::start(const std::map<std::string, std::string> & args)
{
    /**
     * Parse the command line arguments.
     */
    parse_command_line_args(args);
    
    /**
     * Make non-const copy.
     */
    auto command_line_args = args;
    
    /**
     * Reset the boost::asio::io_service.
     */
    m_io_service.reset();
    
    /**
     * Allocate the boost::asio::io_service::work.
     */
    work_.reset(new boost::asio::io_service::work(m_io_service));

    /**
     * Allocate the thread.
     */
    auto thread = std::make_shared<std::thread> (
        std::bind(&stack_impl::loop, this)
    );
    
    /**
     * Retain the thread.
     */
    threads_.push_back(thread);

    /**
     * Allocate the status manager.
     */
    m_status_manager.reset(new status_manager(*this));
    
    /**
     * Start the status manager.
     */
    m_status_manager->start();
    
    if (command_line_args["service"] == "mixer")
    {
        /**
         * Allocate the mixer_manager.
         */
        m_mixer_manager.reset(new mixer_manager(*this));
        
        /**
         * Start the mixer manager.
         */
        m_mixer_manager->start();
    }
    else if (command_line_args["service"] == "whisper")
    {
        /**
         * Allocate the m_whisper_manager.
         */
        m_whisper_manager.reset(new whisper_manager(*this));
        
        /**
         * Start the whisper manager.
         */
        m_whisper_manager->start();
    }
}

void stack_impl::stop()
{
    if (m_mixer_manager)
    {
        m_mixer_manager->stop();
    }
    
    if (m_whisper_manager)
    {
        m_whisper_manager->stop();
    }
    
    if (m_status_manager)
    {
        m_status_manager->stop();
    }
    
    /**
     * Reset the work.
     */
    work_.reset();

    /**
     * Stop the boost::asio::io_service.
     */
    m_io_service.stop();
    
    /**
     * Join the threads.
     */
    for (auto & i : threads_)
    {
        try
        {
            if (i->joinable())
            {
                i->join();
            }
        }
        catch (std::exception & e)
        {
            // ...
        }
    }
    
    /**
     * Clear the threads.
     */
    threads_.clear();
    
    m_status_manager.reset();
    m_mixer_manager.reset();
    m_whisper_manager.reset();
}

void stack_impl::whisper_compose(
    const std::map<std::string, std::string> & pairs
    )
{
    if (m_whisper_manager)
    {
        m_whisper_manager->compose(pairs);
    }
}

void stack_impl::on_status(const std::map<std::string, std::string> & pairs)
{
    stack_.on_status(pairs);
}

void stack_impl::parse_command_line_args(
    const std::map<std::string, std::string> & args
    )
{
    for (auto & i : args)
    {
        if (i.first == "mixer-type")
        {
    
        }
    }
}

void stack_impl::loop()
{
    while (work_)
    {
        try
        {
            m_io_service.run();
            
            if (work_ == 0)
            {
                break;
            }
        }
        catch (std::exception & e)
        {
            // ...
        }
    }
}

boost::asio::io_service & stack_impl::io_service()
{
    return m_io_service;
}

boost::asio::strand & stack_impl::strand()
{
    return m_strand;
}

std::shared_ptr<status_manager> & stack_impl::get_status_manager()
{
    return m_status_manager;
}

std::shared_ptr<mixer_manager> & stack_impl::get_mixer_manager()
{
    return m_mixer_manager;
}
