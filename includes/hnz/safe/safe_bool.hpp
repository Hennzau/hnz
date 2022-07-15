#pragma once

#include <mutex>

#include <hnz/types.hpp>

namespace hnz::safe {
    class safe_bool {
        public:
            safe_bool () = default;

            ~safe_bool () = default;

            void set (bool value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_value = value;
            }

            bool is () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_value;
            }

        private:
            bool               m_value = false;
            mutable std::mutex m_mutex;
    };
}