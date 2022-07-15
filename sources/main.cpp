#include <iostream>

#include <hnz/app.hpp>
#include <hnz/types.hpp>

#include <hnz/ecs/system.hpp>

#include <iostream>

struct Position : public hnz::Component {
    static constexpr Type TYPE = hnz::hash ("PositionComponent");

    Position (float a, float b)
            :
            x { a },
            y { b } {}

    float x;
    float y;
};

struct Velocity : public hnz::Component {
    static constexpr Type TYPE = hnz::hash ("VelocityComponent");

    Velocity (float a, float b)
            :
            dx { a },
            dy { b } {}

    float dx;
    float dy;
};

struct PlayerMovement : public hnz::System {
    static constexpr Type TYPE     = hnz::hash ("PlayerMovementSystem");
    static constexpr auto REQUIRED = { Position::TYPE, Velocity::TYPE };

    void operator() (hnz::f64 delta,
                     hnz::entity entity,
                     hnz::set<hnz::entity>& subscribers,
                     hnz::map<hnz::Component::Type, hnz::raw<hnz::Component>>& components) override {
        std::cout << delta * hnz::as<hnz::f64> (entity) << std::endl;
    }
};

int main () {
    auto app = hnz::App {};

    auto work = std::thread ([&app] () {
        std::this_thread::sleep_for (std::chrono::seconds (3));
        app.running ().set (false);
    });

    auto player = app.spawn ();
    auto weapon = app.spawn (player);

    app.build ();

    while (app.running ().is ()) {
        app.run ();
    }

    work.join ();
    app.join ();
/*
    std::cout << "Total entities : " << app.entities ().size () << std::endl;

    for (const auto& [parent, entities]: app.parents ()) {
        std::cout << parent << " : ";
        for (const auto& entity: entities) {
            std::cout << entity << " ";
        }
        std::cout << std::endl;
    }
*/
    return 0;
}
