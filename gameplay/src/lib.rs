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
use core::ops::{Mul, Sub};
use core::ptr::slice_from_raw_parts;
use flatbuffers::{FlatBufferBuilder, WIPOffset};
use hashbrown::HashMap;
use serde::{Deserialize, Serialize};
use hecs::*;
use crate::data::logic_data::{CircleShape, LogicCharacterData, LogicDamageShape, LogicData, LogicEffectData, LogicSkillData, ActorSpawnerData, MoveController, EffectSpawnerData};
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

struct LcgRng {
    state: u64,
}

impl LcgRng {
    fn new(seed: u64) -> Self {
        Self { state: seed }
    }

    fn next_u32(&mut self) -> u32 {
        self.state = self.state.wrapping_mul(6364136223846793005).wrapping_add(1);
        (self.state >> 32) as u32
    }

    fn random_pos(&mut self) -> Vector2D{
        let x = ((self.next_u32() % 2000) as i32 - 1000) as f32;
        let y = ((self.next_u32() % 2000) as i32 - 1000) as f32;
        Vector2D {
            x,
            y,
        }
    }
}

#[no_mangle]
pub extern "C" fn start() {
    engine::log("begin start");
    let mut world = LogicData{
        current_time: 0.0,
        actor_spawner: ActorSpawnerData {
            generation: 0,
            next_spawn_time: 0.0,
            next_monster_id: 200_0001,
        },
        effect_spawner: EffectSpawnerData {
            generation: 0,
            next_effect_id: 300_0001,
        },
        skills: vec![LogicSkillData{
            next_cue_time: 0.0,
            delay_damage_time: 0.2,
            damage_shape: LogicDamageShape::Circle(CircleShape{
                radius: 200.0,
            }),
            cd: 1.2,
            cue_effect_id: 1_0001,
            actor_id: 1001,
            damage: 1,
            hit_effect_id: 2_0001,
        }],
        character0: LogicCharacterData {
            id: 1001,
            hp: 100,
            transform: Default::default(),
            move_controller: MoveController {
                ..Default::default()
            },
        },
        monsters: HashMap::new(),
        effects: Default::default(),
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

fn update_spawner(data: &mut LogicData) {
    if data.current_time < data.actor_spawner.next_spawn_time {
        return;
    }

    data.actor_spawner.generation += 1;
    data.actor_spawner.next_spawn_time = data.current_time + 5.0;
    let mut rng = LcgRng::new(data.current_time as u64);
    let spawn_num = rng.next_u32() % 5 + 2;
    engine::log(&format!("spawn_num: {}", spawn_num));
    for i in 0..spawn_num {
         data.actor_spawner.next_monster_id += 1;
        let mut monster = LogicCharacterData {
            id: data.actor_spawner.next_monster_id ,
            hp: 1,
            transform: Transform2D {
                pos: rng.random_pos(),
                yaw: 0.0,
            },
            move_controller: MoveController {
                ..Default::default()
            }
        };
        data.monsters.insert(monster.id, monster);
    };
}

impl Mul<f64> for Vector2D {
    type Output = Vector2D;

    fn mul(self, rhs: f64) -> Self::Output {
        Self::Output {
            x : self.x * rhs as f32,
            y : self.y * rhs as f32,
        }
    }
}

fn update_monster_ai(target: &LogicCharacterData, monster: &mut LogicCharacterData) {
    let damper = Damper::new(0.5);
    monster.move_controller.tar_vel = (&target.transform.pos - &monster.transform.pos).normalize() * 200.0;
    (monster.transform.pos, monster.move_controller.cur_vel) = damper.update_movement_2d(&monster.transform.pos, &monster.move_controller.cur_vel, &monster.move_controller.tar_vel, DELTA_TIME);

    if monster.move_controller.tar_vel.length() > 150.0 {
        monster.move_controller.tar_yaw = monster.move_controller.cur_vel.yaw();
    } else {
        monster.move_controller.tar_yaw = monster.transform.yaw;
    }
    monster.transform.yaw = damper.update_yaw(monster.transform.yaw, monster.move_controller.tar_yaw, DELTA_TIME);
}

fn update_ai(data: &mut LogicData) {
    let target = &data.character0;
    for (monster_id, monster_data) in data.monsters.iter_mut() {
        update_monster_ai(target, monster_data);
    }
}

fn update_damage(actor: &mut LogicCharacterData, skill: &mut LogicSkillData, character: &mut LogicCharacterData) {
    match skill.damage_shape {
        LogicDamageShape::None => {}
        LogicDamageShape::Circle(CircleShape { radius }) => {
            let dis = (&actor.transform.pos - &character.transform.pos).length();
            // engine::log(&format!("id:{}, dis:{}, radius:{}, hp:{}", character.id, dis, radius, character.hp));
            if dis < radius {
                character.hp -= skill.damage;
                play_effect(skill.hit_effect_id, character.transform.clone(), 5.0);
            }
        }
        LogicDamageShape::Rect(_) => {}
    }
}

fn play_effect(effect_res_id: i32, transform: Transform2D, dead_time: f32) {
    let world = world();
    world.effects.insert(world.effect_spawner.next_effect_id, LogicEffectData {
        effect_id: world.effect_spawner.next_effect_id,
        effect_res_id,
        transform,
        dead_time: world.current_time + dead_time,
    });
    world.effect_spawner.generation += 1;
    world.effect_spawner.next_effect_id += 1;
}

fn update_skills(skills: &mut Vec<LogicSkillData>, monsters: &mut HashMap<i32, LogicCharacterData>) {
    let current_time = world().current_time;
    for skill in skills.iter_mut() {
        let actor = &mut world().character0;;
        // 伤害判定
        if current_time >= skill.next_cue_time + skill.delay_damage_time && current_time < skill.next_cue_time + skill.delay_damage_time + DELTA_TIME{
            for (monster_id, monster_data) in monsters.iter_mut() {
                update_damage(actor, skill, monster_data);
            }
        }

        // 播放特效
        if current_time >= skill.next_cue_time + skill.cd {
            skill.next_cue_time += skill.cd;
            play_effect(skill.cue_effect_id, actor.transform.clone(), 5.0);
        }
    }
    update_death(monsters);
}

fn update_death(monsters: &mut HashMap<i32, LogicCharacterData>) {
    let keys_to_remove: Vec<i32> = monsters.iter()
        .filter(|(_, monster)| monster.hp <= 0)
        .map(|(key, _)| *key)
        .collect();

    if keys_to_remove.len() > 0 {
        world().actor_spawner.generation += 1;
    }

    for key in keys_to_remove {
        monsters.remove(&key);
        engine::log(&format!("monster {} died", key));
    }
}

fn update_effects(effects: &mut HashMap<i32, LogicEffectData>) {
    let current_time = world().current_time;
    let keys_to_remove: Vec<i32> = effects.iter()
        .filter(|(_, effect)| current_time >= effect.dead_time)
        .map(|(key, _)| *key)
        .collect();

    if keys_to_remove.len() > 0 {
        world().effect_spawner.generation += 1;
    }

    for key in keys_to_remove {
        effects.remove(&key);
        engine::log(&format!("effect {} died", key));
    }
}

fn update_world(data: &mut LogicData, input: &GPInputData) {
    for (idx, player_input) in input.player_inputs().unwrap().iter().enumerate() {
        update_player(&mut data.character0, &player_input);
    }
    update_spawner(data);
    update_ai(data);
    update_skills(data.skills.as_mut(), &mut data.monsters);
    update_effects(&mut data.effects);
    data.current_time += DELTA_TIME;
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
    let mut actors = Vec::new();
    let (ch0_id, ch0_trans, ch0_motion_state) = get_render_character_new(&data.character0);
    let character0 = GPRenderCharacterData::create(&mut builder, &GPRenderCharacterDataArgs {
        actor_id: ch0_id,
        actor_res_id: 1001,
        transform: Some(&ch0_trans),
        motion_state: Some(&ch0_motion_state),
    });
    actors.push(character0);

    for (monster_id, monster_data) in data.monsters.iter() {
        let (monster_id, monster_trans, monster_motion_state) = get_render_character_new(&monster_data);
        let monster = GPRenderCharacterData::create(&mut builder, &GPRenderCharacterDataArgs {
            actor_id: monster_id,
            actor_res_id: 2001,
            transform: Some(&monster_trans),
            motion_state: Some(&monster_motion_state),
        });
        actors.push(monster);
    }
    let actors = builder.create_vector(&actors);

    let mut effects = Vec::new();
    for (effect_id, effect_data) in data.effects.iter() {
        let pos = GPVec2D::new(effect_data.transform.pos.x, effect_data.transform.pos.y);
        let trans = GPTrans2D::new(&pos, effect_data.transform.yaw);
        let effect = GPRenderEffectData::create(&mut builder, &GPRenderEffectDataArgs {
            effect_id: effect_id.clone(),
            effect_res_id: effect_data.effect_res_id,
            transform: Some(&trans),
        });
        effects.push(effect);
    }
    let effects = builder.create_vector(&effects);

    let render_data = GPRenderData::create(
        &mut builder,
        &GPRenderDataArgs {
            actors_generation: data.actor_spawner.generation,
            actors: Some(actors),
            effects_generation: data.effect_spawner.generation,
            effects: Some(effects),
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