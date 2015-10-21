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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <openssl/bio.h>
#ifdef __cplusplus
}
#endif // __cplusplus

#include <dark/ecdhe.hpp>

using namespace dark;

ecdhe::ecdhe()
    : m_ecdhe(EC_DHE_new(NID_secp256k1))
{
    // ...
}

ecdhe::~ecdhe()
{
    if (m_ecdhe)
    {
        EC_DHE_free(m_ecdhe), m_ecdhe = 0;
    }
}

const std::string & ecdhe::public_key()
{
    if (m_public_key.size() == 0)
    {
        auto len = 0;
        
        auto buf = EC_DHE_getPublicKey(m_ecdhe, &len);
        
        m_public_key = std::string(buf, len);
    }
    
    return m_public_key;
}

std::vector<std::uint8_t> ecdhe::derive_secret_key(
    const std::string & peer_public_key
    )
{
    std::vector<std::uint8_t> ret;
    
    auto len = 0;
    
    auto buf = EC_DHE_deriveSecretKey(
        m_ecdhe, peer_public_key.c_str(),
        static_cast<int> (peer_public_key.size()), &len
    );
    
    ret.insert(ret.begin(), buf, buf + len);
    
    return ret;
}

EC_DHE * ecdhe::get_EC_DHE()
{
    return m_ecdhe;
}

bool ecdhe::sign(
    const std::uint8_t * buf, const std::size_t & len,
    std::vector<std::uint8_t> & signature
    )
{
    auto * ec_key = EVP_PKEY_get1_EC_KEY(m_ecdhe->privkey);

    assert(ec_key);
    
    unsigned int size = ECDSA_size(ec_key);
    
    signature.resize(size);

    if (
        !ECDSA_sign(0, buf, static_cast<std::uint32_t> (len), &signature[0],
        &size, ec_key)
        )
    {
        signature.clear();
        
        return false;
    }
    
    signature.resize(size);
    
    return true;
}

bool ecdhe::verify(
    const std::uint8_t * buf, const std::size_t & len,
    const std::vector<std::uint8_t> & public_key,
    const std::vector<std::uint8_t> & signature
    )
{
    bool ret = false;
    
    auto mem_buf = BIO_new_mem_buf(
        (void *)public_key.data(), static_cast<int> (public_key.size())
    );

    auto ec_key = PEM_read_bio_EC_PUBKEY(mem_buf, 0, 0, 0);
    
    BIO_free(mem_buf);
#if 1
    EC_KEY_print_fp(stdout, ec_key, 2);
#endif
    assert(ec_key);
    
    if (signature.size() > 0)
    {
        auto ptr_signature = &signature[0];
        
        ECDSA_SIG * ecdsa_sig = 0;
        
        if (
            (ecdsa_sig = d2i_ECDSA_SIG(
            0, &ptr_signature, signature.size())) != 0
            )
        {
            std::uint8_t * pp = 0;
            
            auto len = i2d_ECDSA_SIG(ecdsa_sig, &pp);
            
            ECDSA_SIG_free(ecdsa_sig), ecdsa_sig = 0;
            
            if (pp && len > 0)
            {
                ret = ECDSA_verify(
                    0, buf, len, pp, len, ec_key
                ) == 1;
                
                OPENSSL_free(pp), pp = 0;
            }
        }
    }
    
    return ret;
}

int ecdhe::run_test()
{
    ecdhe ecdhe_a, ecdhe_b, ecdhe_c;
    
    printf("A: %s\n", ecdhe_a.public_key().c_str());
    printf("B: %s\n", ecdhe_b.public_key().c_str());
    printf("C: %s\n", ecdhe_c.public_key().c_str());
    
    printf("A Size: %zu\n", ecdhe_a.public_key().size());
    printf("B Size: %zu\n", ecdhe_b.public_key().size());
    printf("C Size: %zu\n", ecdhe_c.public_key().size());
    
    auto shared_secret1 = ecdhe_a.derive_secret_key(ecdhe_b.public_key());
    auto shared_secret2 = ecdhe_b.derive_secret_key(ecdhe_a.public_key());
    auto shared_secret3 = ecdhe_a.derive_secret_key(ecdhe_c.public_key());
    
    assert(shared_secret1 != shared_secret3);
    
    printf("SS1: %zu\n", shared_secret1.size());
    printf("SS2: %zu\n", shared_secret2.size());
    printf("SS3: %zu\n", shared_secret3.size());
    
    assert(shared_secret1.size() == 32);
    assert(shared_secret2.size() == 32);
    assert(shared_secret3.size() == 32);
    
    /**
     * Signatures
     */
    std::uint8_t buf[32];
    
    for (auto i = 0; i < sizeof(buf); i++)
    {
        buf[i] = i;
    }
    
    std::vector<std::uint8_t> signature;
    
    auto success = ecdhe_a.sign(buf, sizeof(buf), signature);
    
    if (success)
    {
        std::vector<std::uint8_t> public_key(
            ecdhe_a.public_key().begin(), ecdhe_a.public_key().end()
        );

        success = verify(buf, sizeof(buf), public_key, signature);
        
        if (success)
        {
            printf("verify success\n");
        }
        else
        {
            printf("verify failed\n");
        }
    }
    else
    {
        printf("sign failed\n");
    }
    
    return 0;
}
