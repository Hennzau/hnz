#include <iostream>

#include <hnz/types.hpp>
#include <hnz/app.hpp>

struct Position : public hnz::Component {
    static constexpr Type TYPE = hnz::hash_32 ("PositionComponent");

    Position (float a, float b)
            :
            x { a },
            y { b } {}

    float x;
    float y;
};

struct Velocity : public hnz::Component {
    static constexpr Type TYPE = hnz::hash_32 ("VelocityComponent");

    Velocity (float a, float b)
            :
            x { a },
            y { b } {}

    float x;
    float y;
};

struct Name : public hnz::Component {
    static constexpr Type TYPE = hnz::hash_32 ("NameComponent");

    explicit Name (std::string&& a)
            :
            name { std::move (a) } {}

    std::string name;
};

struct PlayerMovement : public hnz::System {
    static constexpr Type TYPE         = hnz::hash_32 ("PlayerMovementSystem");
    static constexpr auto REQUIREMENTS = { Position::TYPE, Velocity::TYPE };

    static constexpr auto USAGE = hnz::System::Usage::ON_TICK;

    void operator() (hnz::f32 delta,
                     hnz::entity entity,
                     const hnz::map<hnz::Component::Type, hnz::owner<hnz::Component>>& components,
                     const hnz::vector<hnz::entity>& subscribers) override {

    }
};

struct PrintPosition : public hnz::System {
    static constexpr Type TYPE         = hnz::hash_32 ("PrintPositionSystem");
    static constexpr auto REQUIREMENTS = { Position::TYPE };
    static constexpr auto USAGE        = hnz::System::Usage::ON_NOTIFICATION;

    void operator() (hnz::f32 delta,
                     hnz::entity entity,
                     const hnz::map<hnz::Component::Type, hnz::owner<hnz::Component>>& components,
                     const hnz::vector<hnz::entity>& subscribers) override {
    }
};

struct PrintPositionVelocityWithName : public hnz::System {
    static constexpr Type TYPE         = hnz::hash_32 ("PrintPositionVelocityWithNameSystem");
    static constexpr auto REQUIREMENTS = { Position::TYPE, Velocity::TYPE, Name::TYPE };
    static constexpr auto USAGE        = hnz::System::Usage::ON_NOTIFICATION;

    void operator() (hnz::f32 delta,
                     hnz::entity entity,
                     const hnz::map<hnz::Component::Type, hnz::owner<hnz::Component>>& components,
                     const hnz::vector<hnz::entity>& subscribers) override {
    }
};

struct PrintVelocityWithName : public hnz::System {
    static constexpr Type TYPE         = hnz::hash_32 ("PrintVelocityWithNameSystem");
    static constexpr auto REQUIREMENTS = { Velocity::TYPE, Name::TYPE };
    static constexpr auto USAGE        = hnz::System::Usage::ON_NOTIFICATION;

    void operator() (hnz::f32 delta,
                     hnz::entity entity,
                     const hnz::map<hnz::Component::Type, hnz::owner<hnz::Component>>& components,
                     const hnz::vector<hnz::entity>& subscribers) override {
    }
};

int main () {
    auto app = hnz::App {};

    app.record<PlayerMovement> ();
    app.record<PrintPosition> ();
    app.record<PrintVelocityWithName> ();
    app.record<PrintPositionVelocityWithName> ();

    app.build ();

    return 0;
}