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
#include <random>

#include <boost/algorithm/string.hpp>

#include <dark/crypto.hpp>
#include <dark/ecdhe.hpp>
#include <dark/hc256.hpp>
#include <dark/logger.hpp>
#include <dark/network.hpp>
#include <dark/whirlpool.hpp>
#include <dark/whisper.hpp>
#include <dark/whisper_message.hpp>
#include <dark/whisper_pow.hpp>

using namespace dark;

whisper_message::whisper_message()
    : m_tid(0)
    , m_timestamp(0)
    , m_flags(0)
{
    // ...
}

whisper_message::whisper_message(const std::string & query_string)
    : m_tid(0)
    , m_timestamp(0)
    , m_flags(0)
    , m_query_string(query_string)
{
    // ...
}

bool whisper_message::encode(
    ecdhe & ecdhe_ctx, const std::vector<std::uint8_t> & shared_secret
    )
{
    if (shared_secret.size() > 0)
    {
        std::random_device rd;
        std::mt19937_64 g(rd());
        
        std::uniform_int_distribution<std::uint32_t> dist(
            0, std::numeric_limits<std::uint32_t>::max()
        );
        
        m_tid = dist(g);
        
        log_debug("Whisper message is encoding id = " << m_tid);
        
        /**
         * Set the id.
         */
        m_query_string = "__id=" + std::to_string(m_tid);
        
        m_timestamp = std::time(0);
        
        /**
         * Set the timestamp.
         */
        m_query_string += "&__timestamp=" + std::to_string(m_timestamp);
        
        m_flags = 0;
        
        /**
         * Set the flags.
         */
        m_query_string += "&__flags=" + std::to_string(m_flags);
        
        if (m_public_key_recipient.size() > 0)
        {
            /**
             * Set the to.
             */
            m_query_string +="&to=" + whirlpool(&m_public_key_sender[0],
                m_public_key_sender.size()).to_string()
            ;
        }
        
        if (m_public_key_sender.size() > 0)
        {
            /**
             * Set the sender.
             */
            m_query_string +=
                "&__from=" + network::uri_encode(crypto::base64_encode(
                reinterpret_cast<const char *> (&m_public_key_sender[0]),
                m_public_key_sender.size()))
            ;
        }

        auto shared_secret32 = whirlpool(
            &shared_secret[0], shared_secret.size()
        ).to_string().substr(0, 32);

        /**
         * Allocate the hc256 context.
         */
        hc256 ctx(
            shared_secret32, shared_secret32,
            "n5tH9JWEuZuA96wkA747jsp4JLvXDV8j"
        );
        
        /**
         * Encrypt the body.
         */
        auto encrypted = ctx.encrypt(m_body);

        if (encrypted.size() > 0)
        {
            /**
             * Set the body.
             */
            m_query_string +=
                "&__body=" + network::uri_encode(
                crypto::base64_encode(encrypted.data(), encrypted.size()))
            ;
        }
        
        /**
         * Allocate the whisper_pow.
         */
        whisper_pow pow;
        
        /**
         * Copy the query string into the nonce buffer for target calculation.
         */
        std::vector<std::uint8_t> nonce_buf(
            m_query_string.begin(), m_query_string.end()
        );
        
        /**
         * Calculate the target.
         */
        auto target =
            std::numeric_limits<std::uint64_t>::max() / (
            (nonce_buf.size() + whisper_pow::extra_work +
            sizeof(std::uint64_t)) * whisper_pow::nonce_fudge)
        ;
        
        log_debug("Whisper message generated target = " << target << ".");
        
        /**
         * Generate the nonce.
         */
        auto nonce = pow.generate_nonce(nonce_buf, target);
        
        log_debug("Whisper message generated nonce = " << nonce << ".");
        
        /**
         * Set the nonce.
         */
        m_query_string += "&__nonce=" + std::to_string(nonce);
        
        /**
         * Set the lifetime.
         */
        m_query_string += "&_l=1800";

        /**
         * Hash the query string.
         */
         auto hash = whirlpool(
            reinterpret_cast<const std::uint8_t *> (m_query_string.data()),
            m_query_string.size()
        ).to_string();
        
        /** 
         * Sign the hash.
         */
        if (
            ecdhe_ctx.sign(reinterpret_cast<const std::uint8_t *> (
            hash.data()), hash.size(), m_signature)
            )
        {
            m_query_string +=
                "&__signature=" + network::uri_encode(crypto::base64_encode(
                reinterpret_cast<const char *> (&m_signature[0]),
                m_signature.size()))
            ;

            return true;
        }
    }
    
    return false;
}

bool whisper_message::decode(const std::vector<std::uint8_t> & shared_secret)
{
    assert(m_query_string.size());
    
    std::map<std::string, std::string> pairs_all;
    std::map<std::string, std::string> pairs_public;
    
    parse_query(m_query_string, pairs_all, pairs_public);
    
    if (pairs_all.size() > 0 && pairs_public.size() > 0)
    {
        auto it1 = pairs_all.find("__id");
        auto it2 = pairs_all.find("__timestamp");
        auto it3 = pairs_all.find("__flags");
        auto it4 = pairs_public.find("to");
        auto it5 = pairs_all.find("__from");
        auto it6 = pairs_all.find("__body");
        auto it7 = pairs_all.find("__nonce");
        auto it8 = pairs_all.find("__signature");
        
        if (
            it1 != pairs_public.end() && it2 != pairs_public.end() &&
            it3 != pairs_public.end() && it4 != pairs_public.end() &&
            it5 != pairs_public.end() && it6 != pairs_public.end() &&
            it7 != pairs_public.end() && it8 != pairs_public.end()
            )
        {
            /**
             * Get the tid.
             */
            m_tid = static_cast<std::uint32_t> (std::stol(it1->second));
            
            /**
             * Get the timestamp.
             */
            m_timestamp = std::stoll(it2->second);
            
            /**
             * Get the to.
             */
            m_to = network::uri_decode(it4->second);

            auto uri_decoded_from = network::uri_decode(it5->second);
            
            /**
             * Get the from.
             */
            m_from = crypto::base64_decode(
                uri_decoded_from.data(), uri_decoded_from.size()
            );
            
            auto uri_decoded_text = network::uri_decode(it6->second);
            
            /**
             * Get the body.
             */
            m_body = crypto::base64_decode(
                uri_decoded_text.data(), uri_decoded_text.size()
            );
            
            /**
             * Get the nonce.
             */
            auto nonce = std::stoll(it7->second);
            
            /**
             * Convert nonce to host byte order.
             */
            nonce = (std::uint64_t)(((std::uint64_t)htonl(
                (std::uint32_t)nonce)) << 32) + htonl(nonce >> 32)
            ;
    
            auto uri_decoded_signature = network::uri_decode(it8->second);
            
            /**
             * Get the signature.
             */
            auto signature = crypto::base64_decode(
                uri_decoded_signature.data(), uri_decoded_signature.size()
            );
            
            log_debug("Whisper message got __id = " << m_tid);
            log_debug("Whisper message got __timestamp = " << it2->second);
            log_debug("Whisper message got __flags = " << it3->second);
            log_debug("Whisper message got to = " << m_to);
            log_debug("Whisper message got __from = " << m_from);
            log_debug("Whisper message got __body = " << m_body);
            log_debug("Whisper message got __nonce = " << nonce);
            
            auto shared_secret32 = whirlpool(
                &shared_secret[0], shared_secret.size()
            ).to_string().substr(0, 32);

            /**
             * Allocate the hc256 context.
             */
            hc256 ctx(
                shared_secret32, shared_secret32,
                "n5tH9JWEuZuA96wkA747jsp4JLvXDV8j"
            );
            
            /**
             * Allocate the public key (from).
             */
            std::vector<std::uint8_t> public_key(m_from.begin(), m_from.end());
        
            /**
             * Set the signature.
             */
            m_signature = std::vector<std::uint8_t> (
                signature.begin(), signature.end()
            );

            /**
             * Trim from the signature until the end.
             */
            auto offset_signature = m_query_string.find("&__signature");
            
            /**
             * Get the query string up to the signature.
             */
            auto query_string_signature =
                m_query_string.substr(0, offset_signature)
            ;

            /**
             * Trim from the nonce until the end.
             */
            auto offset_nonce = m_query_string.find("&__nonce");
            
            /**
             * Get the query string up to the nonce.
             */
            auto query_string_nonce = m_query_string.substr(0, offset_nonce);
            
            /**
             * Copy the query string into the nonce buffer for validation.
             */
            std::vector<std::uint8_t> nonce_buf(
                query_string_nonce.begin(), query_string_nonce.end()
            );
        
            /**
             * Validate the nonce.
             */
            if (whisper_pow::validate_nonce(nonce, nonce_buf) == true)
            {
                /**
                 * Hash the query string.
                 */
                 auto hash = whirlpool(
                    reinterpret_cast<const std::uint8_t *> (
                    query_string_signature.data()),
                    query_string_signature.size()
                ).to_string();
                
                if (
                    ecdhe::verify(reinterpret_cast<const std::uint8_t *> (
                    hash.data()), hash.size(), public_key,
                    m_signature) == true
                    )
                {
                    /**
                     * Decrypt the body.
                     */
                    m_body = ctx.decrypt(m_body);
                    
                    log_debug(
                        "Whisper message got body (decrypted) = " << m_body
                    );
                
                    return true;
                }
            }
            else
            {
                log_debug(
                    "Whisper message got failed to validate "
                    "nonce = " << nonce << "."
                );
            }
        }
        else
        {
            return false;
        }
    }
    
    return false;
}

const std::uint32_t & whisper_message::tid() const
{
    return m_tid;
}

const std::string & whisper_message::from() const
{
    return m_from;
}

const std::int64_t & whisper_message::timestamp() const
{
    return m_timestamp;
}

void whisper_message::set_public_key_recipient(
    const char * buf, const std::size_t & len
    )
{
    m_public_key_recipient.clear();
    
    m_public_key_recipient.insert(
        m_public_key_recipient.end(), buf,  buf + len
    );
}

void whisper_message::set_public_key_sender(
    const char * buf, const std::size_t & len
    )
{
    m_public_key_sender.clear();
    
    m_public_key_sender.insert(
        m_public_key_sender.end(), buf,  buf + len
    );
}

void whisper_message::set_text(const std::string & val)
{
    m_body = val;
}

const std::string & whisper_message::body() const
{
    return m_body;
}

const std::string & whisper_message::query_string() const
{
    return m_query_string;
}

int whisper_message::run_test()
{
    ecdhe ecdhe_a, ecdhe_b;

    ecdhe_a.public_key();
    ecdhe_b.public_key();
    
    whisper_message msg;
    
    msg.set_public_key_sender(
        ecdhe_a.public_key().data(), ecdhe_a.public_key().size()
    );
    
    msg.set_public_key_recipient(
        ecdhe_b.public_key().data(), ecdhe_b.public_key().size()
    );

    msg.set_text("Hello World!");
    
    auto shared_secret = ecdhe_a.derive_secret_key(ecdhe_b.public_key());
    
    msg.encode(ecdhe_a, shared_secret);

    msg.decode(shared_secret);
    
    return 0;
}

void whisper_message::parse_query(
    const std::string & val, std::map<std::string, std::string> & pairs_all,
    std::map<std::string, std::string> & pairs_public
    )
{
    auto i = val.find("&");
    
    if (i != std::string::npos)
    {
        std::vector<std::string> pairs1;
        
        boost::split(pairs1, val, boost::is_any_of("&"));
        
        for (auto & i : pairs1)
        {
            std::vector<std::string> pairs2;
            
            boost::split(pairs2, i, boost::is_any_of("="));
            
            if (pairs2.size() != 2)
            {
                continue;
            }
            
            pairs_all[pairs2[0]] = network::uri_decode(pairs2[1]);
        }
    }
    else
    {
        std::vector<std::string> pairs2;
        
        boost::split(pairs2, val, boost::is_any_of("="));
        
        if (pairs2.size() == 2)
        {
            pairs_all[pairs2[0]] = network::uri_decode(pairs2[1]);
        }
    }
    
    for (auto & i : pairs_all)
    {
        if (i.first.size() == 0 || i.second.size() == 0)
        {
            continue;
        }

        /**
         * Skip "private" terms.
         */
        if (i.first.compare(0, 1, "_") == 0)
        {
            continue;
        }

        /**
         * Insert the pair.
         */
        pairs_public.insert(std::make_pair(i.first, i.second));
    }
}
