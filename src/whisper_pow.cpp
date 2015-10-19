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

#include <boost/asio.hpp>

#include <dark/logger.hpp>
#include <dark/sha256.hpp>
#include <dark/whisper_pow.hpp>

using namespace dark;

std::uint64_t whisper_pow::generate_nonce(
    const std::vector<std::uint8_t> & val,
    const std::uint64_t & target
    )
{
    std::uint64_t ret = 0;
    
    auto result = std::numeric_limits<std::uint64_t>::max();
    
    auto hash = sha256(&val[0], val.size());
    
    std::vector<std::uint8_t> digest(
        hash.digest(), hash.digest() + sha256::digest_length
    );

    std::vector<std::uint8_t> buf(sizeof(std::uint64_t) + digest.size());

    std::memcpy(
        buf.data() + sizeof(std::uint64_t), digest.data(), digest.size()
    );
    
    auto * nonce = (std::uint64_t *)buf.data();

    while (result > target)
    {
        ++ret;
        
        *nonce = (
            (std::uint64_t)(((std::uint64_t)htonl(
            (std::uint32_t)ret)) << 32) + htonl(ret >> 32)
        );
        
        auto hash = sha256(&buf[0], buf.size());
        
        digest = std::vector<std::uint8_t> (
            hash.digest(), hash.digest() + sha256::digest_length
        );
        
        std::memcpy(&result, digest.data(), sizeof(std::uint64_t));
        
        result =
            ((((std::uint64_t)ntohl((std::uint32_t)result)) << 32) +
            ntohl(result >> 32))
        ;
    }    

    return ret;
}

bool whisper_pow::validate_nonce(
    const std::uint64_t & nonce, const std::vector<std::uint8_t> & val
    )
{
    if (val.size() > 0)
    {
        std::vector<std::uint8_t> with_nonce(
            sizeof(std::uint64_t) + val.size()
        );

        std::memcpy(
            with_nonce.data(), &nonce, sizeof(std::uint64_t)
        );
        std::memcpy(
            with_nonce.data() + sizeof(std::uint64_t),
            val.data(), val.size()
        );
    
        std::uint64_t result = std::numeric_limits<std::uint64_t>::max();

        std::vector<std::uint8_t> target_buf(
            with_nonce.begin() + sizeof(std::uint64_t), with_nonce.end()
        );
        
        auto target_hash = sha256(&target_buf[0], target_buf.size());

        auto target =
            std::numeric_limits<std::uint64_t>::max() /
            ((target_buf.size() + extra_work +
            sizeof(std::uint64_t)) * nonce_fudge)
        ;

        std::vector<std::uint8_t> buf(
            sizeof(std::uint64_t) + sha256::digest_length
        );

        std::memcpy(buf.data(), &nonce, sizeof(std::uint64_t));
        std::memcpy(
            buf.data() + sizeof(std::uint64_t), target_hash.digest(),
            sha256::digest_length
        );

        auto * digest = sha256(&buf[0], buf.size()).digest();
        
        result =
            ((((std::uint64_t)ntohl((std::uint32_t)*digest)) << 32) +
            ntohl((std::uint64_t)*digest >> 32))
        ;
        
        return result <= target;
    }
    
    return false;
}

int whisper_pow::run_test()
{
    std::time_t time_start = std::time(0);
    
    std::vector<std::uint8_t> buf(160);
    
    for (auto i = 0; i < buf.size(); i++)
    {
        buf[i] = std::rand() % 128;
    }

    auto target =
        std::numeric_limits<std::uint64_t>::max() / (
        (buf.size() + extra_work + sizeof(std::uint64_t)) * nonce_fudge)
    ;
    
    log_debug(__FUNCTION__ << ": " << "gen nonce target = " << target);
    
    auto nonce = generate_nonce(buf, target);
    
    log_debug(__FUNCTION__ << ": " << "nonce = " << nonce);

    nonce = (std::uint64_t)(((std::uint64_t)htonl(
        (std::uint32_t)nonce)) << 32) + htonl(nonce >> 32)
    ;

    log_info(__FUNCTION__ << ": " << "time = " << std::time(0) - time_start);

    if (validate_nonce(nonce, buf))
    {
        log_debug(__FUNCTION__ << ": " << "valid nonce");
    }
    else
    {
        log_error(__FUNCTION__ << ": " << "invalid nonce");
    }
    
    return 0;
}
