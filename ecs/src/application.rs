use std::{
    collections::{
        HashMap,
        HashSet,
    },
    time,
};

use crate::{
    core::{
        component::{
            AnyComponent,
            Component,
            Group,
        },
        entity::Entity,
        system::System,
    },
    memory::storage::Storage,
};
use crate::core::sub_app::SubApp;

pub mod prelude;

pub struct Application {
    storage: Storage,
    next: u64,

    systems: HashMap<Group, Vec<Box<dyn System>>>,
}

/// Implementation of general functions
impl Application {
    pub fn new(systems: Vec<Box<dyn System>>) -> Self {
        let mut descriptor = Vec::<Vec<Component>>::new();

        for system in &systems {
            descriptor.push(system.components());
        }

        let mut mapped_systems = HashMap::<Group, Vec<Box<dyn System>>>::new();

        for system in systems {
            if !mapped_systems.contains_key(&system.id()) {
                mapped_systems.insert(system.id(), Vec::new());
            }

            mapped_systems.get_mut(&system.id()).unwrap().push(system);
        }

        return Self {
            storage: Storage::new(descriptor),
            next: 0,
            systems: mapped_systems,
        };
    }

    pub fn spawn(&mut self) -> Entity {
        self.storage.push_entity(self.next);
        self.next += 1;

        return self.next - 1;
    }

    pub fn run(&mut self) {
        let starting_time = time::Instant::now();
        let mut previous_time = 0f32;

        let limit = time::Duration::from_secs(1);

        loop {
            if starting_time.elapsed() >= limit { break; }

            let now_time = starting_time.elapsed().as_secs_f32();
            let delta_time = now_time - previous_time;
            self.update_all_systems(delta_time);

            previous_time = now_time;
        }
    }

    pub fn add_get_or_get_component<T: AnyComponent + 'static>(&mut self, entity: &Entity, value: T) -> &mut T {
        let (_, groups) = self.storage.add_get_or_get_component(entity, value);

        self.launch_startup_systems(&groups, &[entity.clone()]);

        return self.storage.try_get_component_mut::<T>(entity).unwrap();
    }

    pub fn try_add_component<T: AnyComponent + 'static>(&mut self, entity: &Entity, value: T) -> bool {
        let (result, groups) = self.storage.try_add_component(entity, value);

        self.launch_startup_systems(&groups, &[entity.clone()]);

        return result;
    }

    pub fn try_remove_get_component_any(&mut self, entity: &Entity, id: Component) -> Option<Box<dyn AnyComponent>> {
        let (component, groups) = self.storage.try_remove_get_component_any(entity, id);

        self.launch_quit_systems(&groups, &[entity.clone()]);

        return component;
    }

    pub fn try_remove_get_component<T: AnyComponent + 'static>(&mut self, entity: &Entity) -> Option<Box<T>> {
        let (component, groups) = self.storage.try_remove_get_component::<T>(entity);

        self.launch_quit_systems(&groups, &[entity.clone()]);

        return component;
    }

    pub fn try_remove_component_any(&mut self, entity: &Entity, id: Component) -> bool {
        let (result, groups) = self.storage.try_remove_component_any(entity, id);

        self.launch_quit_systems(&groups, &[entity.clone()]);

        return result;
    }

    pub fn try_remove_component<T: AnyComponent + 'static>(&mut self, entity: &Entity) -> bool {
        let (result, groups) = self.storage.try_remove_component::<T>(entity);

        self.launch_quit_systems(&groups, &[entity.clone()]);

        return result;
    }

    pub fn try_get_component_mut<T: AnyComponent + 'static>(&mut self, entity: &Entity) -> Option<&mut T> {
        return self.storage.try_get_component_mut::<T>(entity);
    }

    pub fn try_get_component<T: AnyComponent + 'static>(&self, entity: &Entity) -> Option<&T> {
        return self.storage.try_get_component::<T>(entity);
    }
}

/// Implementation of systems functions
impl Application {
    fn launch_startup_systems(&mut self, groups: &HashSet<Group>, entities: &[Entity]) {
        let mut sub_app = SubApp::new(&mut self.storage);

        for group in groups {
            if let Some(systems) = self.systems.get_mut(group) {
                for system in systems {
                    system.on_startup(entities, &mut sub_app);
                }
            }
        }
    }

    fn launch_quit_systems(&mut self, groups: &HashSet<Group>, entities: &[Entity]) {
        for group in groups {
            if let Some(systems) = self.systems.get_mut(group) {
                for system in systems {
                    system.on_quit(entities);
                }
            }
        }
    }

    fn update_all_systems(&mut self, delta_time: f32) {
        let mut sub_app = SubApp::new(&mut self.storage);

        for (group, systems) in &mut self.systems {
            let entities = sub_app.view(group.clone());

            for system in systems {
                system.on_update(delta_time, &entities, &mut sub_app);
            }
        }
    }
}
