/*
 * Copyright 2005,2007,2009 Colin Percival.
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

#include <memory>

#include <dark/sha256.hpp>

using namespace dark;

sha256::sha256()
{
    std::memset(m_digest, 0, sizeof(m_digest));
    
    init();
}

sha256::sha256(std::uint64_t b)
{
    std::memset(m_digest, 0, sizeof(m_digest));
    
    *reinterpret_cast<std::uint32_t *>(&m_digest[0]) =
        static_cast<std::uint32_t> (b)
    ;
    
    *reinterpret_cast<std::uint32_t *>(&m_digest[0] + sizeof(std::uint32_t)) =
        static_cast<std::uint32_t> (b >> 32)
    ;
}

sha256::sha256(const std::string & hex)
{
    std::memset(m_digest, 0, sizeof(m_digest));
    
    auto psz = hex.c_str();

    while (isspace(*psz))
    {
        psz++;
    }

    if (psz[0] == '0' && tolower(psz[1]) == 'x')
    {
        psz += 2;
    }

    static const std::uint8_t phexdigit[256] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0, 0,0xa,0xb,0xc,0xd,0xe,0xf,0,0,0,0,0,
        0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0, 0, 0, 0, 0, 0,0xa, 0xb, 0xc, 0xd,
        0xe, 0xf, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    const char* pbegin = psz;
    
    while (phexdigit[(std::uint8_t)*psz] || *psz == '0')
    {
        psz++;
    }
    
    psz--;
    
    std::uint8_t * p1 = (std::uint8_t *)&m_digest;
    std::uint8_t * pend = p1 + digest_length * 4;
    
    while (psz >= pbegin && p1 < pend)
    {
        *p1 = phexdigit[(std::uint8_t)*psz--];
    
        if (psz >= pbegin)
        {
            *p1 |= (phexdigit[(std::uint8_t)*psz--] << 4);
            p1++;
        }
    }
}

sha256::sha256(const std::uint8_t * buf, const std::size_t & len)
{
    init();
    update(buf, len);
    final();
}

sha256 sha256::from_digest(const std::uint8_t * digest)
{
    sha256 ret;
    
    std::memcpy(ret.digest(), digest, digest_length);
    
    return ret;
}

void sha256::init()
{
    SHA256_Init(&context_);
}

void sha256::final()
{
    SHA256_Final(m_digest, &context_);
}

void sha256::update(const std::uint8_t * buf, std::size_t len)
{
    SHA256_Update(&context_, buf, len);
}

std::array<std::uint8_t, sha256::digest_length> sha256::hash(
    const std::uint8_t * buf, const std::size_t & len
    )
{
    std::array<std::uint8_t, digest_length> ret;

    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, buf, len);
    SHA256_Final(&ret[0], &context);

    return ret;
}

std::string sha256::to_string() const
{
    char ret[sha256::digest_length * 2 + 1];
    
    for (auto i = 0; i < sizeof(m_digest); i++)
    {
        sprintf(
            ret + i * 2, "%02x", ((std::uint8_t *)&m_digest)
            [sizeof(m_digest) - i - 1]
        );
    }
    
    return std::string(ret, ret + sha256::digest_length * 2);
}

std::uint64_t sha256::to_uint64(const std::uint32_t & index) const
{
    auto ptr = reinterpret_cast<const std::uint32_t *>(m_digest);
    
    return ptr[2 * index] | (std::uint64_t)ptr[2 * index + 1] << 32;
}

bool sha256::is_empty() const
{
    for (auto & i : m_digest)
    {
        if (i != 0)
        {
            return false;
        }
    }
    
    return true;
}

void sha256::clear()
{
    std::memset(m_digest, 0, sizeof(m_digest));
}

std::uint8_t * sha256::digest()
{
    return m_digest;
}

const std::uint8_t * sha256::digest() const
{
    return m_digest;
}
