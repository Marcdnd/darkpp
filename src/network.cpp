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

#include <dark/network.hpp>

using namespace dark;

const char g_hex_2_dec[256] =
{
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
    -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};
    
std::string network::uri_decode(const std::string & val)
{
    const auto * ptr_src =
        reinterpret_cast<const std::uint8_t *> (val.c_str())
    ;
    
	const auto len_src = val.length();
    
    const auto * ptr_src_end = ptr_src + len_src;
    
    const auto * ptr_src_last_dec = ptr_src_end - 2;
    

    std::unique_ptr<char> ptr_start(new char[len_src]);
    
    auto * ptr_end = ptr_start.get();

    while (ptr_src < ptr_src_last_dec)
	{
		if (*ptr_src == '%')
        {
            char dec1, dec2;
            
            if (
                -1 != (dec1 = g_hex_2_dec[*(ptr_src + 1)])
                && -1 != (dec2 = g_hex_2_dec[*(ptr_src + 2)])
                )
            {
                *ptr_end++ = (dec1 << 4) + dec2;
                
                ptr_src += 3;
                
                continue;
            }
        }

        *ptr_end++ = *ptr_src++;
	}

    while (ptr_src < ptr_src_end)
    {
        *ptr_end++ = *ptr_src++;
    }

	return std::string(ptr_start.get(), ptr_end);
}

const char g_safe[256] =
{
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
    0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

std::string network::uri_encode(const std::string & val)
{
    const char dec_2_hex[16 + 1] = "0123456789ABCDEF";
    
    const auto * ptr_src =
        reinterpret_cast<const std::uint8_t *> (val.c_str())
    ;
    
    const auto len_src = val.length();
    
    std::unique_ptr<char> ptr_start(new char[len_src * 3]);
    
    auto * ptr_end = ptr_start.get();
    
    const std::uint8_t * const ptr_src_end = ptr_src + len_src;

    for (; ptr_src < ptr_src_end; ++ptr_src)
	{
		if (g_safe[*ptr_src])
        {
            *ptr_end++ = *ptr_src;
        }
        else
        {
            *ptr_end++ = '%';
            *ptr_end++ = dec_2_hex[*ptr_src >> 4];
            *ptr_end++ = dec_2_hex[*ptr_src & 0x0F];
        }
	}

    return std::string((char *)ptr_start.get(), (char *)ptr_end);
}
