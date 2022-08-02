#include <iostream>

#include <hnz/types.hpp>
#include <hnz/app.hpp>

struct A : hnz::Component {
    static constexpr Type TYPE = hnz::hash_32 ("A");
};

struct B : hnz::Component {
    static constexpr Type TYPE = hnz::hash_32 ("B");
};

struct C : hnz::Component {
    static constexpr Type TYPE = hnz::hash_32 ("C");
};

struct D : hnz::Component {
    static constexpr Type TYPE = hnz::hash_32 ("D");
};

int main () {
    auto app = hnz::App {};

    app.register_group ({
                                A::TYPE,
                                B::TYPE,
                        });

    app.register_group ({
                                A::TYPE,
                                C::TYPE,
                        });

    app.register_group ({
                                A::TYPE,
                                B::TYPE,
                                C::TYPE,
                        });

    auto entities = hnz::vector<hnz::entity> {};

    for (auto i = 0; i < 6; i++) {
        entities.emplace_back (app.spawn ());
        app.add<A> (entities.back ());
    }

    for (auto i = 3; i < 6; i++) {
        app.add<B> (entities[i]);
    }

    for (auto i = 2; i < 4; i++) {
        app.add<C> (entities[i]);
    }

    app.add<D> (entities[4]);

    for (auto& [type, pool]: app.pools ()) {
        std::cout << "Pool " << type << ": ";

        for (auto entity: pool.entities) {
            std::cout << entity << " ";
        }

        std::cout << std::endl;
    }

    return 0;
}