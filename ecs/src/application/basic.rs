pub mod events {
    use crate::core::{
        event::{
            Event,
            EventID,
            AnyEvent,
        },
        component::{
            ComponentID,
            AnyComponent,
        },
    };

    use ahash::RandomState;

    #[derive(Event)]
    pub struct CloseApplication {}

    #[derive(Event)]
    pub struct TryRemoveComponent {
        pub entity: u64,
        pub component_id: ComponentID,
    }

    #[derive(Event)]
    pub struct TryAddComponent {
        pub entity: u64,
        pub component: Box<dyn AnyComponent>,
    }
}

pub mod components {
    use crate::core::component::{
        Component,
        ComponentID,
        AnyComponent,
    };

    use ahash::RandomState;

    #[derive(Component)]
    pub struct SendCloseEventAfterTime {
        pub time: f32,
    }

    #[derive(Component)]
    pub struct Label {
        pub text: String,
    }

    impl Label {
        pub fn new(text: String) -> Self {
            return Self {
                text
            };
        }
    }
}

pub mod systems {
    use std::{
        rc::Rc,
        cell::RefCell,
    };

    use ahash::AHashSet;

    use crate::{
        application::basic::{
            events,
            components::{
                SendCloseEventAfterTime
            },
        },
        core::{
            component::{
                ComponentID,
                AnyComponent,
            },
            entity::Entity,
            world::World,
            system::{
                System,
                SystemBuilder,
            },
        },
    };

    pub struct CloseApplication {
        pub time: f32,
    }

    impl CloseApplication {
        pub fn new() -> Rc::<RefCell<Self>> {
            return SystemBuilder::new(Self {
                time: 0.0
            });
        }
    }

    impl System for CloseApplication {
        fn components(&self) -> AHashSet<ComponentID> {
            return vec![
                SendCloseEventAfterTime::component_id()
            ].into_iter().collect();
        }

        fn on_tick(&mut self, delta_time: f32, entities: &[Entity], world: &mut World) {
            self.time += delta_time;

            for entity in entities {
                if let Some(component) = world.try_get_component::<SendCloseEventAfterTime>(entity) {
                    if self.time >= component.time {
                        world.send_event(Box::new(events::CloseApplication {}));
                    }
                }
            }
        }
    }
}


