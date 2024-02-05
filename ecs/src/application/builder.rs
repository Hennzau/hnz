use ahash::{
    AHashMap,
    AHashSet,
};

use crate::{
    application::{
        Application,
    },
    memory::{
        mapping::MemoryMappingDescriptor,
    },
    core::{
        event::EventID,
        system::CustomSystem,
        component::Group,
    },
};

/// Builder for constructing an application with specific configurations.
pub struct ApplicationBuilder {
    event_systems: AHashMap<EventID, Vec<CustomSystem>>,

    join_systems: AHashMap<Group, Vec<CustomSystem>>,
    quit_systems: AHashMap<Group, Vec<CustomSystem>>,
    tick_systems: Vec<CustomSystem>,

    descriptor: MemoryMappingDescriptor,
    seen: AHashSet<Group>,
}

/// Enum representing different types of systems.
#[derive(Eq, Hash, PartialEq)]
pub enum SystemType {
    /// System type for handling entity join events.
    JOIN,
    /// System type for handling entity quit events.
    QUIT,
    /// System type for handling tick events.
    TICK,
    /// System type for handling custom events with specified `EventID`.
    EVENT(EventID),
}

impl ApplicationBuilder {
    /// Creates a new instance of the ApplicationBuilder.
    ///
    /// # Returns
    ///
    /// Returns a new ApplicationBuilder with default configurations.
    ///
    /// # Example
    ///
    /// ```
    /// // Create a new instance of the ApplicationBuilder.
    /// let app_builder = ApplicationBuilder::new();
    ///
    /// // Use the created ApplicationBuilder for building an application with specific configurations.
    /// ```
    pub fn new() -> Self {
        return Self {
            event_systems: AHashMap::new(),

            join_systems: AHashMap::new(),
            quit_systems: AHashMap::new(),
            tick_systems: Vec::new(),

            descriptor: MemoryMappingDescriptor::new(),
            seen: AHashSet::new(),
        };
    }

    /// Builds the application using the specified configurations.
    ///
    /// # Returns
    ///
    /// Returns a new instance of the Application based on the provided configurations.
    ///
    /// # Example
    ///
    /// ```
    /// // Build the application using the specified configurations.
    /// let application = app_builder.build();
    ///
    /// // Add entities and components to your application
    ///
    /// let entity = application.spawn ();
    ///
    /// // Start and run the created application.
    /// application.run();
    /// ```
    pub fn build(self) -> Application {
        return Application::new(
            self.descriptor,
            self.event_systems,
            self.join_systems,
            self.quit_systems,
            self.tick_systems,
        );
    }

    /// Adds a custom system to the application with specified system types.
    ///
    /// # Arguments
    ///
    /// * `system` - The custom system to be added to the application.
    /// * `types` - A hash set (`AHashSet`) of system types indicating when the system should be executed.
    ///
    /// # Example
    ///
    /// ```
    /// let mut app_builder = // ... (create or obtain an ApplicationBuilder instance)
    /// let custom_system = // ... (create or obtain a CustomSystem instance)
    /// let system_types = // ... (provide a hash set of SystemType instances)
    ///
    /// // Add a custom system to the application with specified system types.
    /// app_builder.add_system(custom_system, system_types);
    ///
    /// // Continue configuring the application or build it using other methods.
    /// ```
    ///
    /// The method adds a custom system to the application with specified system types, allowing fine-grained control over system execution.
    pub fn add_system(&mut self, system: CustomSystem, types: AHashSet<SystemType>) {
        for system_type in types {
            match system_type {
                SystemType::JOIN => self.add_join_system(system.clone()),
                SystemType::QUIT => self.add_quit_system(system.clone()),
                SystemType::TICK => self.add_tick_system(system.clone()),
                SystemType::EVENT(id) => self.add_event_system(id, system.clone())
            }
        }
    }

    /// Adds multiple custom systems to the application with specified system types.
    ///
    /// # Arguments
    ///
    /// * `systems` - A vector (`Vec`) of custom systems to be added to the application.
    /// * `types` - A hash set (`AHashSet`) of system types indicating when the systems should be executed.
    ///
    /// # Example
    ///
    /// ```
    /// let mut app_builder = // ... (create or obtain an ApplicationBuilder instance)
    /// let custom_systems = // ... (create or obtain a vector of CustomSystem instances)
    /// let system_types = // ... (provide a hash set of SystemType instances)
    ///
    /// // Add multiple custom systems to the application with specified system types.
    /// app_builder.add_systems(custom_systems, system_types);
    ///
    /// // Continue configuring the application or build it using other methods.
    /// ```
    ///
    /// The method adds multiple custom systems to the application with specified system types, allowing fine-grained control over system execution.
    pub fn add_systems(&mut self, systems: Vec<CustomSystem>, types: AHashSet<SystemType>) {
        for system_type in types {
            match system_type {
                SystemType::JOIN => {
                    for system in &systems {
                        self.add_join_system(system.clone())
                    }
                }
                SystemType::QUIT => {
                    for system in &systems {
                        self.add_quit_system(system.clone())
                    }
                }
                SystemType::TICK => {
                    for system in &systems {
                        self.add_tick_system(system.clone())
                    }
                }
                SystemType::EVENT(id) => {
                    for system in &systems {
                        self.add_event_system(id, system.clone())
                    }
                }
            }
        }
    }

    /// Adds an event system to the application with the specified event and system.
    ///
    /// # Arguments
    ///
    /// * `event` - The event ID associated with the event system.
    /// * `system` - The custom system to be added to the application for handling the specified event.
    ///
    /// # Example
    ///
    /// ```
    /// let mut app_builder = // ... (create or obtain an ApplicationBuilder instance)
    /// let event_id = // ... (provide the EventID associated with the event system)
    /// let custom_system = // ... (create or obtain a CustomSystem instance)
    ///
    /// // Add an event system to the application with the specified event and system.
    /// app_builder.add_event_system(event_id, custom_system);
    ///
    /// // Continue configuring the application or build it using other methods.
    /// ```
    ///
    /// The method adds an event system to the application with the specified event and system, ensuring proper configuration and group registration.
    fn add_event_system(&mut self, event: EventID, system: CustomSystem) {
        if !self.event_systems.contains_key(&event) {
            self.event_systems.insert(event, Vec::new());
        }

        if !self.seen.contains(&system.borrow().group()) {
            self.descriptor.push(system.borrow().components());
            self.seen.insert(system.borrow().group());
        }

        self.event_systems.get_mut(&event).unwrap().push(system);
    }

    /// Adds a join system to the application with the specified custom system.
    ///
    /// # Arguments
    ///
    /// * `system` - The custom system to be added to the application for handling entity join events.
    ///
    /// # Example
    ///
    /// ```
    /// let mut app_builder = // ... (create or obtain an ApplicationBuilder instance)
    /// let custom_system = // ... (create or obtain a CustomSystem instance for handling entity join events)
    ///
    /// // Add a join system to the application with the specified custom system.
    /// app_builder.add_join_system(custom_system);
    ///
    /// // Continue configuring the application or build it using other methods.
    /// ```
    ///
    /// The method adds a join system to the application with the specified custom system, ensuring proper configuration and group registration.
    fn add_join_system(&mut self, system: CustomSystem) {
        if !self.seen.contains(&system.borrow().group()) {
            self.descriptor.push(system.borrow().components());
            self.seen.insert(system.borrow().group());
        }

        let group = system.borrow().group();

        if !self.join_systems.contains_key(&group) {
            self.join_systems.insert(group, Vec::new());
        }

        self.join_systems.get_mut(&group).unwrap().push(system);
    }

    /// Adds a quit system to the application with the specified custom system.
    ///
    /// # Arguments
    ///
    /// * `system` - The custom system to be added to the application for handling entity quit events.
    ///
    /// # Example
    ///
    /// ```
    /// let mut app_builder = // ... (create or obtain an ApplicationBuilder instance)
    /// let custom_system = // ... (create or obtain a CustomSystem instance for handling entity quit events)
    ///
    /// // Add a quit system to the application with the specified custom system.
    /// app_builder.add_quit_system(custom_system);
    ///
    /// // Continue configuring the application or build it using other methods.
    /// ```
    ///
    /// The method adds a quit system to the application with the specified custom system, ensuring proper configuration and group registration.
    fn add_quit_system(&mut self, system: CustomSystem) {
        if !self.seen.contains(&system.borrow().group()) {
            self.descriptor.push(system.borrow().components());
            self.seen.insert(system.borrow().group());
        }

        let group = system.borrow().group();

        if !self.quit_systems.contains_key(&group) {
            self.quit_systems.insert(group, Vec::new());
        }

        self.quit_systems.get_mut(&group).unwrap().push(system);
    }

    /// Adds a tick system to the application with the specified custom system.
    ///
    /// # Arguments
    ///
    /// * `system` - The custom system to be added to the application for handling tick events.
    ///
    /// # Example
    ///
    /// ```
    /// let mut app_builder = // ... (create or obtain an ApplicationBuilder instance)
    /// let custom_system = // ... (create or obtain a CustomSystem instance for handling tick events)
    ///
    /// // Add a tick system to the application with the specified custom system.
    /// app_builder.add_tick_system(custom_system);
    ///
    /// // Continue configuring the application or build it using other methods.
    /// ```
    ///
    /// The method adds a tick system to the application with the specified custom system, ensuring proper configuration and group registration.
    fn add_tick_system(&mut self, system: CustomSystem) {
        if !self.seen.contains(&system.borrow().group()) {
            self.descriptor.push(system.borrow().components());
            self.seen.insert(system.borrow().group());
        }

        self.tick_systems.push(system);
    }
}