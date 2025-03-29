#pragma once

#include <mutex>

namespace cncpp
{
    template<typename T>
    class Singleton
    {
    private:
        Singleton(const Singleton&) = delete;
        const Singleton& operator=(const Singleton&) = delete;

    public:
        class Destroy
        {
        public:
            ~Destroy()
            {
                if (Singleton<T>::instance)
                {
                    delete Singleton<T>::instance;
                    Singleton<T>::instance = nullptr;
                }
            }
        };

    protected:
        static std::once_flag once_flag_;

        static T* instance; // = 0;  // nullptr;
        Singleton() {}

        ~Singleton() {}

        static void createInst()
        {
            instance = new T();
            static Singleton<T>::Destroy destroy;
        }

    public:
        static T& getMe()
        {
            std::call_once(once_flag_, createInst);
            return *instance;
        }
    };

    template<typename T>
    T* Singleton<T>::instance = 0;
    template<typename T>
    std::once_flag Singleton<T>::once_flag_;
} // namespace cncpp
