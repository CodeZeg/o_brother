#![no_std]
#![allow(unused)]
#![allow(improper_ctypes)]  // 忽略外部函数的类型检查
extern crate alloc;
mod allocator;
mod engine;
mod lockstep;
mod data;

use data::*;

use alloc::{format, vec};
use alloc::string::ToString;
use hashbrown::HashMap;
use serde::{Deserialize, Serialize};
use hecs::*;
use crate::data::input_data::InputStateData;

#[no_mangle]
pub extern "C" fn start() {
    engine::log("begin start");

    engine::log("after start")
}

#[no_mangle]
pub extern "C" fn update(input: input_data::InputData) {
    // let state = input.player_0.state;
    // match state {
    //     InputStateData::Fighting(fight_state) => {
    //         let direction = fight_state.direction;
    //         let magnitude = fight_state.magnitude;
    //         engine::log(&format!("input, direction: {}, magnitude: {}", direction, magnitude));
    //     }
    //     InputStateData::Other => {
    //         engine::log("input, other state");
    //     }
    // }

    let json =serde_json::to_string(&input).unwrap_or("failed to serialize input data".to_string());
    engine::log(&format!("input: {}", json));
    // engine::log(&format!("inputs: {:#?}", inputs));
}

#[no_mangle]
pub extern "C" fn q_add(a: i32, b: i32) -> i32 {
    a + b
}
