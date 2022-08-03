#include <hnz/app.hpp>


auto hnz::App::build () -> void {
    sort (m_groups_required);

    auto map = hnz::map<hnz::Component::Sum, EntitiesInSystem> {};

    auto group_it = m_groups_required.begin ();
    while (group_it != m_groups_required.end ()) {
        auto& group = m_groups.emplace_back (Group {});
        map[hnz::sum (*group_it)] = EntitiesInSystem {
                .entities = &group.entities,
                .count    = &group.first_not,
        };

        while (group_it + 1 != m_groups_required.end () && is_included (*(group_it + 1),
                                                                        *group_it)) {
            group.nested[hnz::sum (*(group_it + 1))] = 0;
            map[hnz::sum (*(group_it + 1))].entities = &group.entities;
            map[hnz::sum (*(group_it + 1))].count    = &group.nested[hnz::sum (*(group_it + 1))];

            group_it++;
        }

        group_it++;
    }

    /* link all to the systems */

    for (auto& [type, system]: m_systems) {
        auto sum = hnz::sum (system.requirements);
        m_entities_in_system[type] = map[sum];
    }
}

constexpr auto hnz::App::sort (hnz::vector<hnz::Component::Types>& wanted) -> void {
    std::sort (wanted.begin (),
               wanted.end (),
               [&wanted] (const auto& a, const auto& b) {
                   if (a.size () > b.size ()) {
                       return true;
                   }

                   if (a.size () == b.size ()) {
                       return hnz::count (a,
                                          wanted) < hnz::count (b,
                                                                wanted);
                   }

                   return false;
               });

    auto result = hnz::vector<hnz::Component::Types> {};

    for (auto& wanted_type: wanted) {
        auto found = false;

        auto index = hnz::i32 { 0 };
        for (auto& result_type: result) {
            if (hnz::is_included (result_type,
                                  wanted_type)) {
                if (index == 0) {
                    result.insert (result.begin (),
                                   wanted_type);
                    found = true;
                    break;
                }

                if (hnz::is_included (wanted_type,
                                      result[index - 1])) {
                    result.insert (result.begin () + index,
                                   wanted_type);
                    found = true;
                    break;
                }

                if (not hnz::is_included (result[index],
                                          result[index - 1])) {
                    result.insert (result.begin () + index,
                                   wanted_type);
                    found = true;
                    break;
                }
            }

            index++;
        }

        if (not found) {
            result.emplace_back (wanted_type);
        }
    }

    wanted = result;
}