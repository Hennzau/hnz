#pragma once

#include <hnz/types.hpp>

#include <mutex>

namespace hnz::safe {
    template<typename T>
    class queue {
        public:
            queue () = default;

            ~queue () = default;

            void push (T const& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_queue.push (value);
            }

            void push (T&& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_queue.push (std::move (value));
            }

            void pop () {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_queue.pop ();
            }

            [[nodiscard]] T& front () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_queue.front ();
            }

            [[nodiscard]] T& front () {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_queue.front ();
            }

            bool empty () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_queue.empty ();
            }

            auto begin () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_queue.begin ();
            }

            auto end () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_queue.end ();
            }

            auto size () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_queue.size ();
            }

        private:
            hnz::queue<T>      m_queue;
            mutable std::mutex m_mutex;
    };
}