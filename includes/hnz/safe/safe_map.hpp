#pragma once

#include <hnz/types.hpp>

#include <mutex>

namespace hnz::safe {
    template<typename Key, typename Val>
    class map {
        public:
            map () = default;

            ~map () = default;

            void insert (Key const& key, Val const& value) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_map.insert (std::make_pair (key,
                                              value));
            }

            Val& operator[] (Key const& key) {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_map[key];
            }

            Val const& operator[] (Key const& key) const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_map.at (key);
            }

            void erase (Key const& key) {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_map.erase (key);
            }

            bool contains (Key const& key) const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_map.find (key) != m_map.end ();
            }

            bool empty () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_map.empty ();
            }

            void clear () {
                std::lock_guard<std::mutex> lock (m_mutex);
                m_map.clear ();
            }

            auto begin () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_map.begin ();
            }

            auto end () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_map.end ();
            }

            auto size () const {
                std::lock_guard<std::mutex> lock (m_mutex);
                return m_map.size ();
            }

        private:
            hnz::map<Key, Val> m_map;
            mutable std::mutex m_mutex;
    };
}