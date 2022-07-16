#pragma once

#include <hnz/types.hpp>

#include <mutex>

namespace hnz::safe {
    template<typename T>
    class set {
        public:
            set () = default;

            ~set () = default;

            void insert (T const& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_set.insert (value);
            }

            void erase (T const& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_set.erase (value);
            }

            void erase (T&& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_set.erase (std::move (value));
            }

            bool contains (T const& value) const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_set.find (value) != m_set.end ();
            }

            bool empty () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_set.empty ();
            }

            void clear () {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_set.clear ();
            }

            auto begin () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_set.begin ();
            }

            auto end () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_set.end ();
            }

            auto size () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_set.size ();
            }

        private:
            hnz::set<T>        m_set;
            mutable std::mutex m_mutex;
    };
}