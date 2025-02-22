#![no_std]
#![allow(unused)]
#![allow(improper_ctypes)]  // 忽略外部函数的类型检查
extern crate alloc;
mod allocator;
mod engine;
mod lockstep;
mod data;
mod physics;
pub mod math_generated {
    include!("schema/math_generated.rs");
}
use crate::math_generated::*;
pub mod input_data_generated {
    include!("schema/input_data_generated.rs");
}
use crate::input_data_generated::*;
pub mod render_data_generated {
    include!("schema/render_data_generated.rs");
}
use crate::render_data_generated::*;

use data::*;
use alloc::{format, vec};
use alloc::borrow::ToOwned;
use alloc::boxed::Box;
use alloc::ffi::CString;
use alloc::string::{String, ToString};
use alloc::vec::Vec;
use core::mem;
use core::mem::ManuallyDrop;
use core::ops::Sub;
use core::ptr::slice_from_raw_parts;
use flatbuffers::{FlatBufferBuilder, WIPOffset};
use hashbrown::HashMap;
use serde::{Deserialize, Serialize};
use hecs::*;
use crate::data::logic_data::{LogicCharacterData, LogicData, MoveController};
use crate::data::protocol::{MotionState, Vector2D, Transform2D, fix_yaw};
use crate::physics::damper::Damper;

const DELTA_TIME: f32 = 1.0 / 30.0; // 固定帧率
static mut RENDER_CACHE_DATA: Vec<u8> = Vec::new(); // 常驻一份渲染数据给外部访问
static mut WORLD: Option<LogicData> = None; // 常驻一份逻辑数据，作为当前的状态
fn world() -> &'static mut LogicData {
    unsafe {
        WORLD.as_mut().unwrap()
    }
}

#[no_mangle]
pub extern "C" fn start() {
    engine::log("begin start");
    let mut world = LogicData{
        character0: LogicCharacterData {
            id: 1001,
            transform: Default::default(),
            move_controller: MoveController {
                ..Default::default()
            },
        },
        monsters: vec![],
    };

    unsafe { WORLD = Some(world); }
    engine::log("after start")
}

fn update_player_move_controller(data: &mut MoveController, input: &GPInputPlayerData) {
    const MAX_SPEED: f32 = 450.0;
    if let Some(move_data) = input.state_as_gpinput_move_data() {
        let mut tar_vel = Vector2D {
            x: move_data.x(),
            y: move_data.y(),
        };
        let len = tar_vel.length();
        if len > 1.0 {
            tar_vel.x /= len;
            tar_vel.y /= len;
        }
        tar_vel.x *= MAX_SPEED;
        tar_vel.y *= MAX_SPEED;
        data.tar_vel = tar_vel;
    }
}

fn update_player_locomotion(data: &mut LogicCharacterData, input: &GPInputPlayerData) {
    let damper = Damper::new(0.15);
    update_player_move_controller(&mut data.move_controller, &input);
    (data.transform.pos, data.move_controller.cur_vel) = damper.update_movement_2d(&data.transform.pos, &data.move_controller.cur_vel, &data.move_controller.tar_vel, DELTA_TIME);

    if data.move_controller.tar_vel.length() > 150.0 {
        data.move_controller.tar_yaw = data.move_controller.cur_vel.yaw();
    } else {
        data.move_controller.tar_yaw = data.transform.yaw;
    }
    data.transform.yaw = damper.update_yaw(data.transform.yaw, data.move_controller.tar_yaw, DELTA_TIME);
}

fn update_player(data: &mut LogicCharacterData, input: &GPInputPlayerData) {
    update_player_locomotion(data, input);
}

fn update_world(data: &mut LogicData, input: &GPInputData) {
    for (idx, player_input) in input.player_inputs().unwrap().iter().enumerate() {
        update_player(&mut data.character0, &player_input);
    }
}

#[no_mangle]
pub extern "C" fn update(input_buffer: *const u8, input_len: u32) -> *const u8 {
    let mut world = world();
    let input = unsafe {
        let slice = slice_from_raw_parts(input_buffer, input_len as usize);
        root_as_gpinput_data_unchecked(&*slice)
    };
    update_world(&mut world, &input);
    get_render_world_new(&world)
}

fn get_render_character_new(
    data: &LogicCharacterData
) -> (i32, GPTrans2D, GPMotionState) {
    let pos = GPVec2D::new(data.transform.pos.x, data.transform.pos.y);
    let trans = GPTrans2D::new(&pos, data.transform.yaw);
    let motion_state = GPMotionState::new(
        data.move_controller.cur_vel.length(),
        0,
        0.0
    );
    (data.id, trans, motion_state)
}

fn get_render_world_new(data: &LogicData) -> *const u8 {
    let mut builder = FlatBufferBuilder::new();
    let (ch0_id, ch0_trans, ch0_motion_state) = get_render_character_new(&data.character0);
    let character0 = GPRenderCharacterData::create(&mut builder, &GPRenderCharacterDataArgs {
        id: ch0_id,
        transform: Some(&ch0_trans),
        motion_state: Some(&ch0_motion_state),
    });

    let mut monsters = Vec::new();
    for monster_data in data.monsters.iter() {
        let (monster_id, monster_trans, monster_motion_state) = get_render_character_new(&monster_data);
        let monster = GPRenderCharacterData::create(&mut builder, &GPRenderCharacterDataArgs {
            id: monster_id,
            transform: Some(&monster_trans),
            motion_state: Some(&monster_motion_state),
        });
        monsters.push(monster);
    }
    let monsters = builder.create_vector(&monsters);
    let render_data = GPRenderData::create(
        &mut builder,
        &GPRenderDataArgs {
            generation: 0,
            character0: Some(character0),
            monsters: Some(monsters),
        }
    );

    builder.finish(render_data, None);
    let buf = builder.finished_data();
    unsafe {
        RENDER_CACHE_DATA.clear();
        RENDER_CACHE_DATA.extend_from_slice(buf);
        RENDER_CACHE_DATA.as_ptr()
    }
}