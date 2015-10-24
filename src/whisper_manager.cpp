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

#include <dark/crypto.hpp>
#include <dark/logger.hpp>
#include <dark/network.hpp>
#include <dark/stack_impl.hpp>
#include <dark/status_manager.hpp>
#include <dark/whisper.hpp>
#include <dark/whisper_message.hpp>
#include <dark/whisper_manager.hpp>

using namespace dark;

whisper_manager::whisper_manager(stack_impl & owner)
    : stack_impl_(owner)
    , timer_(owner.io_service())
{
    // ...
}

void whisper_manager::start()
{
    auto public_key = whisper::instance().get_ecdhe().public_key();
    
    m_hash_public_key = whirlpool(
        reinterpret_cast<const std::uint8_t *> (
        public_key.data()), public_key.size()
    );
    
    log_info(
        "Whisper manager generated public key = \n" << public_key <<
        "\n" << m_hash_public_key.to_string() << "."
    );
    
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
    
    database::stack::configuration stack_config;
    
    /**
     * The bootstrap contacts.
     */
    std::vector< std::pair<std::string, std::uint16_t> > contacts;
    
    /**
     * Add the hard-coded bootstrap contacts.
     */
    contacts.push_back(std::make_pair("p01.vanillacoin.net", 32809));
    contacts.push_back(std::make_pair("p02.vanillacoin.net", 40006));
    contacts.push_back(std::make_pair("p03.vanillacoin.net", 40008));
    contacts.push_back(std::make_pair("p04.vanillacoin.net", 60912));
    contacts.push_back(std::make_pair("p05.vanillacoin.net", 43355));
    contacts.push_back(std::make_pair("p06.vanillacoin.net", 52461));
    contacts.push_back(std::make_pair("p07.vanillacoin.net", 51902));
    contacts.push_back(std::make_pair("p08.vanillacoin.net", 44111));
    
    /**
     * Set the port.
     */
    stack_config.set_port(0);
    
    /**
     * Set the operation mode.
     */
    stack_config.set_operation_mode(
        database::stack::configuration::operation_mode_interface
    );
    
    /**
     * Start the database::stack.
     */
    database::stack::start(stack_config);
    
    /**
     * Join the database::stack.
     */
    database::stack::join(contacts);
    
    /**
     * Start the timer.
     */
    do_tick(8);
}

void whisper_manager::stop()
{
    timer_.cancel();
    
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
            auto it2 = pairs.find("__body");

            if (it1 != pairs.end() && it2 != pairs.end())
            {
                auto to = it1->second;
                auto body = it2->second;
                
                if (
                    to.find("-----BEGIN PUBLIC KEY-----")
                    != std::string::npos &&
                    to.find("-----END PUBLIC KEY-----")
                    != std::string::npos
                    )
                {
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
                    
                    /**
                     * Store in the decentralised database.
                     */
                    if (success == true)
                    {
                        log_debug(
                            "Whisper manager is storing " <<
                            msg.query_string() << "."
                        );
                        
                        database::stack::store(msg.query_string());
                    }
                }
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

void whisper_manager::on_find(
    const std::uint16_t & transaction_id, const std::string & query
    )
{
    stack_impl_.io_service().post(stack_impl_.strand().wrap(
        [this, query]()
    {
        log_debug("Whisper manager found " << query << ".");
        
        whisper_message msg(query);
        
        auto i = query.find("__from=");

        if (i != std::string::npos)
        {
            auto substr = query.substr(i + strlen("__from="));
            
            i = substr.find("&");

            substr = substr.substr(0, i);
            
            substr = network::uri_decode(substr);
            
            auto public_key = crypto::base64_decode(
                substr.data(), substr.size()
            );
            
            if (public_key.size() > 0)
            {
                auto shared_secret = whisper::instance().get_ecdhe(
                    ).derive_secret_key(public_key
                );
                
                if (msg.decode(shared_secret) == true)
                {
                    log_info("Whisper manager decoded message.");
                    
                    if (m_tids.count(msg.tid()) > 0)
                    {
                        log_info(
                            "Whisper manager dropping duplicate tid = " <<
                            msg.tid() << "."
                        );
                    }
                    else
                    {
                        /**
                         * Insert the transaction id.
                         */
                        m_tids.insert(msg.tid());
                        
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
                        pairs["value"] = "message";
                        
                        /**
                         * Set the pairs whisper.message.from.
                         */
                        pairs["whisper.message.from"] = msg.from();
                        
                        /**
                         * Get the address by hashinh the from.
                         */
                        auto hash_from = whirlpool(
                            reinterpret_cast<const std::uint8_t *> (
                            msg.from().data()), msg.from().size()
                        );
    
                        /**
                         * Set the pairs whisper.message.address.
                         */
                        pairs["whisper.message.address"] =
                            hash_from.to_string()
                        ;
                        
                        /**
                         * Set the pairs whisper.message.timestamp.
                         */
                        pairs["whisper.message.timestamp"] = std::to_string(
                            msg.timestamp()
                        );
                        
                        /**
                         * Set the pairs whisper.message.body.
                         */
                        pairs["whisper.message.body"] = msg.body();
                        
                        /**
                         * Callback
                         */
                        stack_impl_.get_status_manager()->insert(pairs);
                    }
                }
                else
                {
                    log_error(
                        "Whisper manager failed to decode message, dropping."
                    );
                }
            }
        }
    }));
}

void whisper_manager::do_tick(const std::uint32_t & interval)
{
    timer_.expires_from_now(std::chrono::seconds(interval));
    timer_.async_wait(stack_impl_.strand().wrap([this, interval]
        (boost::system::error_code ec)
    {
        if (ec)
        {
            // ...
        }
        else
        {
            /**
             * Find ourselves.
             */
            database::stack::find("to=" + m_hash_public_key.to_string(), 50);

            /**
             * Start the timer.
             */
            do_tick(60);
        }
    }));
}
