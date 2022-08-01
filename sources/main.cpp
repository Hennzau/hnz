#include <iostream>

#include <hnz/app.hpp>
#include <hnz/types.hpp>

#include <hnz/ecs/system.hpp>
#include <utility>

#include <iostream>

struct Position : public hnz::Component {
    static constexpr Type TYPE = 2;

    Position (float a, float b)
            :
            x { a },
            y { b } {}

    float x;
    float y;
};

struct Velocity : public hnz::Component {
    static constexpr Type TYPE = 3;

    Velocity (float a, float b)
            :
            dx { a },
            dy { b } {}

    float dx;
    float dy;
};

struct Name : public hnz::Component {
    static constexpr Type TYPE = 5;

    explicit Name (std::string a)
            :
            name { std::move (a) } {}

    std::string name;
};

struct PlayerMovement : public hnz::System {
    static constexpr Type TYPE         = hnz::hash ("PlayerMovementSystem");
    static constexpr auto REQUIREMENTS = { Position::TYPE, Velocity::TYPE };

    static constexpr auto USAGE = hnz::System::Usage::ON_TICK;

    void operator() (hnz::f32 delta,
                     hnz::entity entity,
                     const hnz::map<hnz::Component::Type, hnz::owner<hnz::Component>>& components,
                     const hnz::vector<hnz::entity>& subscribers) override {
        auto& position = *hnz::reinterpret<Position*> (components.at (Position::TYPE).get ());
        auto& velocity = *hnz::reinterpret<Velocity*> (components.at (Velocity::TYPE).get ());

        position.x += velocity.dx * delta;
        position.y += velocity.dy * delta;
    }
};

#include <bitset>

int main () {/*
    auto app = hnz::App {};

    auto player = app.spawn ();             // 1
    auto weapon = app.spawn (player);   // 2
    auto armor  = app.spawn (player);   // 3
    auto ammo   = app.spawn (weapon);   // 4
    auto wings  = app.spawn (player);   // 5
    auto fire   = app.spawn (wings);    // 6

    app.add<Position> ({ player, armor, ammo },
                       0.0f,
                       0.0f);

    app.add<Velocity> ({ player, fire },
                       1.0f,
                       0.0f);

    app.kill (wings, true);

    auto work = std::thread ([&app] () {
        std::this_thread::sleep_for (std::chrono::seconds (2));
        app.running ().set (false);
    });

    while (app.running ().is ()) {
        app.run ();
    }

    work.join ();
    app.join ();*/

    std::cout << hnz::bitset_256 ("Position") << std::endl;

    return 0;
}