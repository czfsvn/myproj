#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Log.h"

#define MAX_BUFFER_SIZE 100000


struct MsgHeader
{
    uint16_t msg_code = 0;
    uint16_t msg_len  = 0;
};

const uint32_t HEADER_LEN = sizeof(MsgHeader);

namespace cncpp
{
    class BaseBuffer;
    using BaseBufferPtr = std::shared_ptr<BaseBuffer>;

    class BaseBuffer
    {
    public:
        BaseBuffer(uint32_t size = MAX_BUFFER_SIZE)
        {
            data_.clear();
            len_ = size;
            data_.reserve(len_);
        }

        BaseBuffer(const BaseBuffer& buf)
        {
            data_.clear();
            len_       = buf.len_;
            rd_offset_ = buf.rd_offset_;
            wr_offset_ = buf.wr_offset_;
            data_.reserve(wr_offset_);
            memcpy(begin(), buf.begin(), wr_offset_);
        }

        BaseBuffer(BaseBuffer&& buf)
        {
            std::swap(buf.data_, data_);
            len_       = buf.len_;
            rd_offset_ = buf.rd_offset_;
            wr_offset_ = buf.wr_offset_;
        }

        BaseBuffer& operator=(BaseBuffer&& buf)
        {
            std::swap(buf.data_, data_);
            len_       = buf.len_;
            rd_offset_ = buf.rd_offset_;
            wr_offset_ = buf.wr_offset_;
            return *this;
        }

        BaseBuffer& operator=(const BaseBuffer& buf)
        {
            data_.clear();
            len_       = buf.len_;
            rd_offset_ = buf.rd_offset_;
            wr_offset_ = buf.wr_offset_;
            data_.reserve(wr_offset_);
            memcpy(begin(), buf.begin(), wr_offset_);
            return *this;
        }

        void reset()
        {
            rd_offset_ = 0;
            wr_offset_ = 0;
        }

        bool available()
        {
            return rd_offset_ < wr_offset_;
        }

        const uint32_t getCapacity() const
        {
            return len_;
        }

        uint32_t size()
        {
            return wr_offset_;
        }

        uint32_t size() const
        {
            return wr_offset_;
        }

        uint32_t canRdSize()
        {
            if (rd_offset_ >= wr_offset_)
                return 0;

            return wr_offset_ - rd_offset_;
        }

        uint32_t canRdSize() const
        {
            if (rd_offset_ >= wr_offset_)
                return 0;

            return wr_offset_ - rd_offset_;
        }

        void consume_rd(uint32_t size)
        {
            rd_offset_ += size;
        }

        uint32_t canWrSize() const
        {
            if (wr_offset_ >= len_)
                return 0;

            return len_ - wr_offset_;
        }

        void consume_wr(uint32_t size)
        {
            wr_offset_ += size;
        }

        uint32_t append_data(const char* data, uint32_t size)
        {
            if (!data || !size)
                return 0;

            uint32_t can_wr = std::min<uint32_t>(size, canWrSize());
            if (can_wr)
            {
                std::copy(data, data + can_wr, beginWr());
                wr_offset_ += can_wr;
            }
            return can_wr;
        }

        uint32_t append_data(const std::string& data)
        {
            return append_data(data.c_str(), data.size());
        }

        void set_datas(const char* data, const uint32_t& len, const uint32_t& pos)
        {
            if (!data || !len)
                return;

            std::copy(data, data + len, begin() + pos);
        }

        void set_data(const void* data, const uint32_t& len, const uint32_t& pos)
        {
            set_datas(( char* )data, len, pos);
            /*
            if (!data || !len)
                return 0;

            memcpy(&data_[pos], data, len);
            return len;
            */
        }

        uint32_t get_data(void* toBuf, const uint32_t& len, const int32_t& pos) const
        {
            if (len_ < pos || len_ < pos + len)
                return 0;

            memcpy(( char* )toBuf, &data_[pos], len);
            return len;
        }

        // private:
        char* begin()
        {
            return &*data_.begin();
        }

        const char* begin() const
        {
            return &*data_.begin();
        }

        char* beginWr()
        {
            return begin() + wr_offset_;
        }

        const char* beginWr() const
        {
            return begin() + wr_offset_;
        }

        char* beginRd()
        {
            return begin() + rd_offset_;
        }

        const char* beginRd() const
        {
            return begin() + rd_offset_;
        }

        // protected:
        std::vector<char> data_      = {};
        uint32_t          rd_offset_ = 0;
        uint32_t          wr_offset_ = 0;
        uint32_t          len_       = 0;
    };

    
class Message : public BaseBuffer
    {
    public:
        Message(uint32_t size = MAX_BUFFER_SIZE) : BaseBuffer(size + HEADER_LEN)
        {
            Reset();
        }

        Message(const Message& buf)
        {
            data_.clear();
            len_       = buf.len_;
            rd_offset_ = buf.rd_offset_;
            wr_offset_ = buf.wr_offset_;
            data_.reserve(wr_offset_);
            memcpy(begin(), buf.begin(), wr_offset_);
        }

        Message(Message&& buf)
        {
            std::swap(buf.data_, data_);
            len_       = buf.len_;
            rd_offset_ = buf.rd_offset_;
            wr_offset_ = buf.wr_offset_;
        }

        Message& operator=(Message&& buf)
        {
            std::swap(buf.data_, data_);
            len_       = buf.len_;
            rd_offset_ = buf.rd_offset_;
            wr_offset_ = buf.wr_offset_;
            return *this;
        }

        Message& operator=(const Message& buf)
        {
            data_.clear();
            len_       = buf.len_;
            rd_offset_ = buf.rd_offset_;
            wr_offset_ = buf.wr_offset_;
            data_.reserve(buf.size());
            memcpy(begin(), buf.begin(), wr_offset_);
            return *this;
        }

        void Reset()
        {
            reset();
            rd_offset_ = HEADER_LEN;
            wr_offset_ = HEADER_LEN;
        }

        void setHeader(const MsgHeader& head)
        {
            set_data(&head, sizeof(head), 0);
        }

        void setMsgCode(const uint16_t code)
        {
            set_data(&code, sizeof(code), 0);
        }

        void setMsgLen(const uint16_t len)
        {
            set_data(&len, sizeof(len), 2);
        }

        uint16_t getMsgCode() const
        {
            uint16_t code = 0;
            get_data(&code, sizeof(code), 0);
            return code;
        }

        uint16_t getMsgCode()
        {
            uint16_t code = 0;
            get_data(&code, sizeof(code), 0);
            return code;
        }

        uint16_t getMsgLen() const
        {
            uint16_t length = 0;
            get_data(&length, sizeof(length), 2);
            return length;
        }

        uint16_t getMsgLen()
        {
            uint16_t length = 0;
            get_data(&length, sizeof(length), 2);
            return length;
        }

        uint32_t writeBody(const std::string& msg)
        {
            const uint32_t cur_size = canWrSize();
            if (msg.size() > cur_size)
            {
                len_ = msg.size() + size();
                std::vector<char> data(len_);
                // std::copy(data_.begin(), data_.end(), std::back_inserter(data));
                memcpy(&data[0], &data_[0], wr_offset_);
                std::swap(data, data_);

                // data_.resize(msg.size() - cur_size);
            }
            return append_data(msg);
        }

        std::string readBody()
        {
            const uint32_t can_rd_size = canRdSize();
            const uint32_t body_len    = getMsgLen();
            if (can_rd_size < body_len)
                return "";

            /* 
            char arr[body_len];
            // = {};
            memcpy(arr, beginRd(), body_len);
            consume_rd(body_len);
            return std::string(arr, body_len);
            */
            // todo: check work ok!!
            return std::string(beginRd(), body_len);
        }
    };

    
template <uint32_t N>
    class BufferPool
    {
    public:
        BufferPool() {}

        BaseBufferPtr getBuffer()
        {
            if (buffer_pool.empty())
            {
                //using namespace std;
                //std::cout << "allocate memtory for BaseBuffer\n";
                INFO("[BufferPool][getBuffer] allocate memtory for BaseBuffer");
                return std::make_shared<BaseBuffer>();
            }
            BaseBufferPtr ptr = buffer_pool.front();
            buffer_pool.pop_front();
            ptr->reset();
            return ptr;
        }

        void putBuffer(const BaseBufferPtr& ptr)
        {
            if (!ptr)
                return;

            if (buffer_pool.size() >= N)
                return;

            buffer_pool.emplace_back(ptr);
        }

    private:
        std::list<BaseBufferPtr> buffer_pool = {};
    };

    class RingBuffer
    {
    public:
        RingBuffer(uint32_t init_size = 102400) : size(init_size)
        {
            // buffers_.reserve(size);
        }

        void emplace_back(const BaseBufferPtr& buf)
        {
            if (!buf || !buf->available())
                return;

            buffers_.emplace_back(buf);
        }

        const uint32_t canReadSize()
        {
            uint64_t size = 0;
            for (const auto& buf : buffers_)
            {
                if (!buf)
                    continue;
                size += buf->canRdSize();
            }
            return size;
        }

        uint32_t read(void* toBuf, const uint32_t& size)
        {
            const uint64_t& remain = canReadSize();
            if (remain < size)
                return 0;

            uint32_t rd_idx = 0;
            while (rd_idx < size)
            {
                if (buffers_.empty())
                    break;

                BaseBufferPtr& buf = buffers_.front();
                if (!buf || !buf->available())
                {
                    pool.putBuffer(buf);
                    buffers_.pop_front();
                    continue;
                }

                const uint32_t will_read = std::min(buf->canRdSize(), size - rd_idx);
                memcpy(( char* )toBuf + rd_idx, buf->beginRd(), will_read);
                rd_idx += will_read;
                buf->consume_rd(will_read);
            }

            return rd_idx;
        }

        uint32_t read(BaseBuffer& toBuf, const uint32_t& size)
        {
            const uint64_t& remain = canReadSize();
            if (remain < size)
                return 0;

            uint32_t rd_idx = 0;
            while (rd_idx < size)
            {
                if (buffers_.empty())
                    break;

                BaseBufferPtr& buf = buffers_.front();
                if (!buf || !buf->available())
                {
                    pool.putBuffer(buf);
                    buffers_.pop_front();
                    continue;
                }

                const uint32_t will_read = std::min(buf->canRdSize(), size - rd_idx);
                // memcpy(( char* )toBuf + rd_idx, buf->beginRd(), will_read);
                memcpy(toBuf.beginWr(), buf->beginRd(), will_read);
                toBuf.consume_wr(will_read);
                // std::vector<char> data(msg.size() + cur_size);
                // std::copy(data_.begin(), data_.end(), std::back_inserter(toBuf.data_));

                rd_idx += will_read;
                buf->consume_rd(will_read);
            }

            return rd_idx;
        }

        std::string readAsString(const uint32_t& size)
        {
            const uint64_t& remain = canReadSize();
            if (remain < size)
                return "";

            //char arr[size] = {};
            std::vector<char> arr       = {};
            uint32_t rd_idx = 0;
            while (rd_idx < size)
            {
                if (buffers_.empty())
                    break;

                BaseBufferPtr& buf = buffers_.front();
                if (!buf || !buf->available())
                {
                    pool.putBuffer(buf);
                    buffers_.pop_front();
                    continue;
                }

                const uint32_t will_read = std::min(buf->canRdSize(), size - rd_idx);
                //memcpy(arr + rd_idx, buf->beginRd(), will_read);
                memcpy(&arr[0] + rd_idx, buf->beginRd(), will_read);
                rd_idx += will_read;
                buf->consume_rd(will_read);
            }

            return std::string(&arr[0], rd_idx);
        }

        bool writeString(const std::string& str)
        {
            if (str.empty())
                return false;

            uint32_t       writed = 0;
            const uint32_t size   = str.size();
            while (writed < size)
            {
                if (buffers_.empty())
                {
                    BaseBufferPtr ptr = allocBuff();
                    writed += ptr->append_data(str.c_str() + writed, size - writed);
                    buffers_.emplace_back(ptr);
                    continue;
                }
                else
                {
                    BaseBufferPtr ptr = buffers_.back();
                    if (!ptr || !ptr->canWrSize())
                    {
                        BaseBufferPtr ptr = allocBuff();
                        writed += ptr->append_data(str.c_str() + writed, size - writed);
                        buffers_.emplace_back(ptr);
                        continue;
                    }
                    writed += ptr->append_data(str.c_str() + writed, size - writed);
                }
            }
            return writed;
        }

        bool writeData(const char* data, const uint32_t& size)
        {
            if (!data || !size)
                return false;

            uint32_t writed = 0;
            while (writed < size)
            {
                if (buffers_.empty())
                {
                    BaseBufferPtr ptr = allocBuff();
                    writed += ptr->append_data(data + writed, size - writed);
                    buffers_.emplace_back(ptr);
                }
                else
                {
                    BaseBufferPtr ptr = buffers_.back();
                    if (!ptr || !ptr->canWrSize())
                    {
                        BaseBufferPtr ptr = allocBuff();
                        writed += ptr->append_data(data + writed, size - writed);
                        buffers_.emplace_back(ptr);
                        continue;
                    }
                    writed += ptr->append_data(data + writed, size - writed);
                }
            }
            return true;
        }

        bool writeMessage(const Message& msg)
        {
            return writeString(std::string(msg.begin(), msg.size()));
        }

        BaseBufferPtr allocBuff()
        {
            return pool.getBuffer();
        }

    private:
        std::list<BaseBufferPtr> buffers_ = {};
        uint32_t                 rd_idx   = 0;
        uint32_t                 wr_idx   = 0;
        uint32_t                 size     = 0;
        BufferPool<1000>         pool;
    };
}  // namespace cncpp