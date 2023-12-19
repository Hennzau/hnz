/*
This is the main module for stocking entities in different memory, in order to iterate over them efficiently
 */

mod graph;
mod mapping;
mod pool;
mod factory;

pub use mapping::MemoryMapping;
pub use mapping::MemoryMappingDescriptor;