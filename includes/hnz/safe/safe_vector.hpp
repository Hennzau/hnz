#pragma once

#include <hnz/types.hpp>
#include <mutex>

namespace hnz::safe {
    template<typename T>
    class vector {
        public:
            vector () = default;

            ~vector () = default;

            void push_back (T const& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_vector.push_back (value);
            }

            void push_back (T&& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_vector.push_back (std::move (value));
            }

            void emplace_back (T&& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_vector.emplace_back (std::move (value));
            }

            void emplace_back (T const& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_vector.emplace_back (value);
            }

            void pop_back () {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_vector.pop_back ();
            }

            T& operator[] (size_t index) {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_vector[index];
            }

            T const& operator[] (size_t index) const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_vector[index];
            }

            void erase (size_t index) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_vector.erase (m_vector.begin () + index);
            }

            bool contains (T const& value) const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_vector.find (value) != m_vector.end ();
            }

            bool empty () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_vector.empty ();
            }

            void clear () {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_vector.clear ();
            }

            auto begin () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_vector.begin ();
            }

            auto end () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_vector.end ();
            }

        private:
            std::vector<T>     m_vector;
            mutable std::mutex m_mutex;
    };
}