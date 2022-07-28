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

    static constexpr auto USING = hnz::System::Use::EVERY_TICK;

    void operator() (hnz::f64 delta,
                     hnz::entity entity,
                     hnz::set<hnz::entity>& subscribers,
                     hnz::map<hnz::Component::Type, hnz::raw<hnz::Component>>& components) override {
        std::cout << delta * hnz::as<hnz::f64> (entity) << std::endl;
    }
};

int main () {
    auto app = hnz::App {};

    auto player = app.spawn ();             // 1
    auto weapon = app.spawn (player);   // 2
    auto armor  = app.spawn (player);   // 3
    auto ammo   = app.spawn (weapon);   // 4
    auto wings  = app.spawn (player);   // 5
    auto fire   = app.spawn (wings);    // 6

    app.kill (wings,
              true);

    app.add<Position> ({ player, armor, ammo },
                       0.0f,
                       0.0f);

    app.add<Velocity> ({ player, fire },
                       1.0f,
                       0.0f);

    auto work = std::thread ([&app] () {
        std::this_thread::sleep_for (std::chrono::seconds (2));
        app.running ().set (false);
    });

    while (app.running ().is ()) {
        app.run ();
    }

    work.join ();
    app.join ();

    auto& position = app.component<Position> (player);
    auto& velocity = app.component<Velocity> (player);

    auto view = app.view ({ Position::TYPE });

    std::cout << "Final Position of the player : " << position.x << " / " << position.y << std::endl;

    std::cout << "Total entities : " << app.entities ().size () << std::endl;

    for (const auto& [parent, entities]: app.parents ()) {
        std::cout << parent << " : ";
        for (const auto& entity: entities) {
            std::cout << entity << " ";
        }
        std::cout << std::endl;
    }


    return 0;
}