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
#include <future>
#include <thread>

#include <dark/logger.hpp>
#include <dark/stack_impl.hpp>
#include <dark/status_manager.hpp>
#include <dark/whisper.hpp>
#include <dark/whisper_message.hpp>
#include <dark/whisper_manager.hpp>

using namespace dark;

whisper_manager::whisper_manager(stack_impl & owner)
    : stack_impl_(owner)
{
    // ...
}

void whisper_manager::start()
{
    auto public_key = whisper::instance().get_ecdhe().public_key();
    
    log_info("Whisper manager generated public key = " << public_key << ".");
    
    assert(public_key.size());
    
    /**
     * Allocate the pairs.
     */
    std::map<std::string, std::string> pairs;
    
    /**
     * Set the pairs type.
     */
    pairs["type"] = "whisper";
    
    /**
     * Set the pairs value.
     */
    pairs["value"] = "public_key";
    
    /**
     * Set the pairs whisper.public_key.
     */
    pairs["whisper.public_key"] = public_key;
    
    /**
     * Callback
     */
    stack_impl_.get_status_manager()->insert(pairs);
}

void whisper_manager::stop()
{
    for (auto & i : m_whisper_messages)
    {
        i.second.clear();
    }
    
    m_whisper_messages.clear();
}

void whisper_manager::compose(const std::map<std::string, std::string> & pairs)
{
    std::thread([this, pairs]()
    {
        std::async(std::launch::async, [this, pairs]()
        {
            auto success = false;
            
            auto it1 = pairs.find("to");
            auto it2 = pairs.find("body");

            if (it1 != pairs.end() && it2 != pairs.end())
            {
                auto to = it1->second;
                auto body = it2->second;
                
                whisper_message msg;
                
                msg.set_public_key_recipient(to.data(), to.size());
                
                msg.set_public_key_sender(
                    whisper::instance().get_ecdhe().public_key().data(),
                    whisper::instance().get_ecdhe().public_key().size()
                );
                msg.set_text(body);
                
                auto shared_secret =
                    whisper::instance().get_ecdhe().derive_secret_key(to)
                ;
                
                success = msg.encode(
                    whisper::instance().get_ecdhe(), shared_secret
                );
            }
        
            /**
             * Allocate the pairs.
             */
            std::map<std::string, std::string> pairs;
            
            /**
             * Set the pairs type.
             */
            pairs["type"] = "whisper";
            
            /**
             * Set the pairs value.
             */
            pairs["value"] = "compose";
            
            if (success == true)
            {
                pairs["error.code"] = "0";
                pairs["error.message"] = "success";
            }
            else
            {
                pairs["error.code"] = "-1";
                pairs["error.message"] = "failed";
            }
            
            /**
             * Callback
             */
            stack_impl_.get_status_manager()->insert(pairs);
        });
    
    }).detach();
}