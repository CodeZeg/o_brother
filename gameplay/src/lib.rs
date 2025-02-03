#![no_std]
#![allow(unused)]
#![allow(improper_ctypes)]  // 忽略外部函数的类型检查
extern crate alloc;
mod allocator;
mod engine;
mod lockstep;

use alloc::vec;
use hashbrown::HashMap;
use serde::{Deserialize, Serialize};
use hecs::*;

// mod lockstep;
//
// #[derive(Component, Debug, Clone)]
// pub struct InputData {}
//
// #[derive(Component, Debug, Clone)]
// pub struct LogicData {}
//
// #[derive(Component, Debug, Clone)]
// pub struct PersistentState {}
//
// #[derive(Component, Debug, Clone)]
// pub struct RenderData {}
//
// #[derive(Component, Debug, Clone)]
// pub struct World {}
//
//
// impl lockstep::LockstepData for World {
//     type InputData = InputData;
//     type PersistentData = PersistentState;
//     type RenderData = RenderData;
//
//     fn new(_initial_data: Self::PersistentData, _inputs: Vec<Self::InputData>) -> Self {
//         todo!()
//     }
//
//     fn update(&mut self, _input: &Self::InputData) {
//         todo!()
//     }
//
//     fn take_snapshot(&self) -> (Self::PersistentData, Vec<Self::InputData>) {
//         todo!()
//     }
//
//     fn take_render_data(&self) -> Self::RenderData {
//         todo!()
//     }
//
//     fn get_latest_frame_index(&self) -> i32 {
//         todo!()
//     }
// }

#[no_mangle]
pub extern "C" fn start() {
    engine::log("begin start");
    let mut map = vec![];
    map.push(1);

    let mut world = World::new();
    // Nearly any type can be used as a component with zero boilerplate
    let a = world.spawn((123, true, "abc"));
    let b = world.spawn((42, false));
    // Systems can be simple for loops
    for (id, (number, &flag)) in world.query_mut::<(&mut i32, &bool)>() {
        if flag { *number *= 2; }
    }
    engine::log("after start")
}

#[no_mangle]
pub extern "C" fn update() {}

#[no_mangle]
pub extern "C" fn q_add(a: i32, b: i32) -> i32 {
    a + b
}
