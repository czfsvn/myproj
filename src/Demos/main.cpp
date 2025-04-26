#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include "siphash.h"

#include <vector>

#include "compress.h"

#if 0
namespace test
{
    void calculateMD5(const char* input, char* output)
    {
        EVP_MD_CTX*   mdctx;
        EVP_MD*       md;
        unsigned int  digest_len;
        unsigned char digest[EVP_MAX_MD_SIZE];

        // 获取MD5算法
        md = EVP_MD_fetch(nullptr, "MD5", nullptr);
        if (!md)
        {
            std::cerr << "Failed to fetch MD5 algorithm." << std::endl;
            return;
        }

        // 创建上下文
        mdctx = EVP_MD_CTX_new();
        if (!mdctx)
        {
            std::cerr << "Failed to create MD5 context." << std::endl;
            EVP_MD_free(md);
            return;
        }

        // 初始化上下文
        if (EVP_DigestInit_ex2(mdctx, md, nullptr) != 1)
        {
            std::cerr << "Failed to initialize MD5 context." << std::endl;
            EVP_MD_CTX_free(mdctx);
            EVP_MD_free(md);
            return;
        }

        // 更新数据
        if (EVP_DigestUpdate(mdctx, input, strlen(input)) != 1)
        {
            std::cerr << "Failed to update MD5 context." << std::endl;
            EVP_MD_CTX_free(mdctx);
            EVP_MD_free(md);
            return;
        }

        // 完成计算
        if (EVP_DigestFinal_ex(mdctx, digest, &digest_len) != 1)
        {
            std::cerr << "Failed to finalize MD5 context." << std::endl;
            EVP_MD_CTX_free(mdctx);
            EVP_MD_free(md);
            return;
        }

        // 将结果转换为十六进制字符串
        for (unsigned int i = 0; i < digest_len; ++i)
        {
            // sprintf(&output[i * 2], "%02x", ( unsigned int )digest[i]);
        }

        // 清理资源
        EVP_MD_CTX_free(mdctx);
        EVP_MD_free(md);
    }

    #if 0
    void test_siphash24()
    {
        const char* input = "Hello, OpenSSL in VS2022!";
        char        output[33];  // MD5 hash is 32 characters long + null terminator
        calculateMD5(input, output);

        std::cout << "MD5 Hash: " << output << std::endl;

        // 128-bit 密钥（需安全存储）
        const uint8_t key[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA,
            0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

        // 输入数据
        std::vector<uint8_t> data = { 'H', 'e', 'l', 'l', 'o', ' ', 'V', 'S', '2', '0', '2', '2' };

        // 计算 SipHash-2-4
        uint64_t hash = siphash24(data.data(), data.size(), key);

        // 输出结果
        printf("SipHash: 0x%016llX\n", hash);  // 输出示例：0x7A3C55D8B2E1F0A9
    }
    #endif
}

#endif

int main()
{
    ns_compress::main();

    return 0;
}