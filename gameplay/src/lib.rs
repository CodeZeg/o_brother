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
use alloc::ffi::CString;
use alloc::string::ToString;
use hashbrown::HashMap;
use serde::{Deserialize, Serialize};
use hecs::*;
use crate::data::input_data::{InputData, InputStateData};
use crate::data::protocol::{MotionState, Position, Rotation, Transform2D};
use crate::data::render_data::{RenderCharacterData, RenderData};

#[no_mangle]
pub extern "C" fn start() {
    engine::log("begin start");
    engine::log("after start")
}

#[no_mangle]
pub extern "C" fn update(input: InputData) -> RenderData {
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

    let json = serde_json::to_string(&input).unwrap_or("failed to serialize input data".to_string());
    engine::log(&format!("input: {}", json));

    let character = RenderCharacterData {
        id: 1001,
        transform: Transform2D {
            position: Position {
                x: 1231.0,
                y: 789.0,
            },
            rotation: Rotation {
                yaw: 16.0,
            },
        },
        motion_state: MotionState {
            locomotion_speed: 0.456,
            montage_id: 001,
            montage_progress: 0.123,
        }
    };
    
    let render_data = RenderData {
        generation: 0,
        character0: character,
    };

    let json = serde_json::to_string(&render_data).unwrap_or("failed to serialize render data".to_string());
    engine::log(&format!("render: {}", json));
    render_data
}

#[no_mangle]
pub extern "C" fn q_add(a: i32, b: i32) -> i32 {
    a + b
}
