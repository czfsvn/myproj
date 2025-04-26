#include "compress.h"

#include <iostream>
#include <zlib.h>
#include <bzlib.h>
#include <vector>
#include <zstd.h>
#include <stdexcept>

#include <sstream>
#include <string>

class ZlibHelper
{
public:
    // 压缩数据（默认使用 Z_BEST_SPEED 压缩级别）
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& data, int level = Z_BEST_SPEED)
    {
        z_stream zs = { 0 };
        if (deflateInit(&zs, level) != Z_OK)
        {
            throw std::runtime_error("deflateInit failed");
        }

        zs.next_in  = const_cast<Bytef*>(data.data());
        zs.avail_in = data.size();

        std::vector<uint8_t> compressed;
        int                  ret;
        char                 buffer[4096];

        do
        {
            zs.next_out  = reinterpret_cast<Bytef*>(buffer);
            zs.avail_out = sizeof(buffer);

            ret = deflate(&zs, Z_FINISH);
            if (compressed.size() < zs.total_out)
            {
                compressed.insert(
                    compressed.end(), buffer, buffer + zs.total_out - compressed.size());
            }
        } while (ret == Z_OK);

        deflateEnd(&zs);

        if (ret != Z_STREAM_END)
        {
            throw std::runtime_error("Compression failed: " + std::to_string(ret));
        }

        return compressed;
    }

    // 解压数据
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed)
    {
        z_stream zs = { 0 };
        if (inflateInit(&zs) != Z_OK)
        {
            throw std::runtime_error("inflateInit failed");
        }

        zs.next_in  = const_cast<Bytef*>(compressed.data());
        zs.avail_in = compressed.size();

        std::vector<uint8_t> decompressed;
        int                  ret;
        char                 buffer[4096];

        do
        {
            zs.next_out  = reinterpret_cast<Bytef*>(buffer);
            zs.avail_out = sizeof(buffer);

            ret = inflate(&zs, Z_NO_FLUSH);
            if (decompressed.size() < zs.total_out)
            {
                decompressed.insert(
                    decompressed.end(), buffer, buffer + zs.total_out - decompressed.size());
            }
        } while (ret == Z_OK);

        inflateEnd(&zs);

        if (ret != Z_STREAM_END)
        {
            throw std::runtime_error("Decompression failed: " + std::to_string(ret));
        }

        return decompressed;
    }
};

class ZstdHelper
{
public:
    // 压缩数据
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& data, int level = 3)
    {
        size_t               bound = ZSTD_compressBound(data.size());
        std::vector<uint8_t> compressed(bound);

        size_t result_size =
            ZSTD_compress(compressed.data(), bound, data.data(), data.size(), level);

        if (ZSTD_isError(result_size))
        {
            throw std::runtime_error("压缩失败: " + std::string(ZSTD_getErrorName(result_size)));
        }

        compressed.resize(result_size);
        return compressed;
    }

    // 解压数据
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed)
    {
        
        size_t decompressed_size = ZSTD_getFrameContentSize(compressed.data(), compressed.size());
        if (decompressed_size == ZSTD_CONTENTSIZE_ERROR || decompressed_size == ZSTD_CONTENTSIZE_UNKNOWN)
        {
            throw std::runtime_error("无效的压缩数据 ");
            //throw std::runtime_error("无效的压缩数据");
        }

        std::vector<uint8_t> decompressed(decompressed_size);
        size_t               result_size = ZSTD_decompress(
            decompressed.data(), decompressed_size, compressed.data(), compressed.size());

        if (ZSTD_isError(result_size))
        {
            throw std::runtime_error("解压失败: " + std::string(ZSTD_getErrorName(result_size)));
        }

        decompressed.resize(result_size);
        return decompressed;
    }
};

#if 0

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>

namespace boost_compress
{
    // 压缩数据
    std::string zlib_compress(const std::string& data)
    {
        namespace io = boost::iostreams;
        std::stringstream compressed;
        std::stringstream origin(data);

        io::filtering_streambuf<io::input> out;
        out.push(io::zlib_compressor());  // 添加 zlib 压缩过滤器
        out.push(origin);                 // 输入原始数据

        //io::copy(out, compressed);  // 执行压缩
        return compressed.str();
    }

    // 解压数据
    std::string zlib_decompress(const std::string& compressed_data)
    {
        namespace io = boost::iostreams;
        std::stringstream compressed(compressed_data);
        std::stringstream decompressed;

        io::filtering_streambuf<io::input> in;
        in.push(io::zlib_decompressor());  // 添加 zlib 解压过滤器
        in.push(compressed);               // 输入压缩数据

        //io::copy(in, decompressed);  // 执行解压
        return decompressed.str();
    }
}

#endif

namespace ns_zlib
{
    int main()
    {
        try
        {
            // 原始数据（可以是任意二进制数据）
            std::string          original = "Hello, zlib! This is a test string for compression.";
            std::vector<uint8_t> data(original.begin(), original.end());

            // 压缩数据
            auto compressed = ZlibHelper::compress(data, Z_BEST_COMPRESSION);
            std::cout << "Original size: " << data.size()
                      << ", Compressed size: " << compressed.size()
                      << ", Ratio: " << ( float )compressed.size() / data.size() << std::endl;

            // 解压数据
            auto        decompressed = ZlibHelper::decompress(compressed);
            std::string result(decompressed.begin(), decompressed.end());

            // 验证结果
            if (result == original)
            {
                std::cout << "Decompression succeeded!\n";
            }
            else
            {
                std::cerr << "Decompression failed: data mismatch.\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }

        return 0;
    }
}

namespace ns_zstd
{
    int main()
    {
        try
        {
            // 原始数据
            //std::string original = "Zstandard (Zstd) 是由 Facebook 开发的高效压缩算法！";
            std::string original = "Zstandard (Zstd) 是由 Facebook 开发的高效压缩算法!";
            std::vector<uint8_t> data(original.begin(), original.end());

            // 压缩
            auto compressed = ZstdHelper::compress(data, 5);
            std::cout << "原始大小: " << data.size() << ", 压缩后大小: " << compressed.size()
                      << ", 压缩率: " << ( float )compressed.size() / data.size() << "\n";

            // 解压
            auto        decompressed = ZstdHelper::decompress(compressed);
            std::string result(decompressed.begin(), decompressed.end());

            // 验证
            if (result == original)
            {
                std::cout << "解压成功！\n";
            }
            else
            {
                std::cerr << "解压数据不一致！\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "错误: " << e.what() << std::endl;
            return 1;
        }

        return 0;
    }
}


namespace ns_compress
{
    void test()
    {
        // 测试 zlib 版本
        std::cout << "zlib version: " << zlibVersion() << std::endl;

        // 测试 bzip2 版本
        std::cout << "bzip2 vesion: " << BZ2_bzlibVersion() << std::endl;
    }
    
    void main()
    {
        test();
        ns_zlib::main();
        ns_zstd::main();
        //std::cout << "ns_compress::main\n";
        return;
    }
}