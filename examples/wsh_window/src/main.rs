use simple_logger::SimpleLogger;

use hnz::ecs::prelude::*;

use hnz::physics::maths::{
    Position2Di32,
    Scale2Du32
};

use hnz::wsh::window::{
    Window,
    WindowController,
};

fn main() {
    SimpleLogger::new().init().unwrap();

    let mut builder = ApplicationBuilder::new();

    builder.add_system(WindowController::new(), vec![
        SystemType::JOIN,
        SystemType::TICK,
    ].into_iter().collect());

    let mut app = builder.build();

    let window = app.spawn();
    let _ = app.bundle(window).add_component(Window::new()).add_component(basic::components::Label {
        text: "Hello, window!".to_string(),
    }).add_component(Position2Di32 {
        x: 110,
        y: 110,
    }).add_component(Scale2Du32 {
        width: 800,
        height: 600,
    }).try_build();

    app.run(60f32);
}