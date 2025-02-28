use alloc::vec::Vec;
use hashbrown::HashMap;
use serde::{Deserialize, Serialize};
use crate::data::protocol::{MotionState, Transform2D, Vector2D};

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct MoveController {
    pub cur_vel: Vector2D,
    pub tar_vel: Vector2D,
    pub tar_yaw: f32,
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct LogicCharacterData {
    pub id: i32,
    pub hp: i32,
    pub transform: Transform2D,
    pub move_controller: MoveController,
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct LogicEffectData {
    pub effect_id: i32,
    pub effect_res_id: i32,
    pub transform: Transform2D,
    pub dead_time: f32,
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct EffectSpawnerData {
    pub generation: i32,
    pub next_effect_id: i32,
}

#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct ActorSpawnerData {
    pub generation: i32,
    pub next_spawn_time: f32,
    pub next_monster_id: i32,
}

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize, Default)]
pub struct CircleShape {
    pub radius: f32,
}

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize, Default)]
pub struct RectShape {
    pub width: f32,
    pub height: f32,
}

#[repr(C)]
#[derive(Debug, Clone, Serialize, Deserialize, Default)]
pub enum LogicDamageShape {
    #[default]
    None,
    Circle(CircleShape),
    Rect(RectShape),
}

/// 统一技能抽象，包括攻击，碰撞掉血等，都是技能
#[repr(C)]
#[derive(Default, Debug, Clone, Serialize, Deserialize)]
pub struct LogicSkillData {
    pub next_cue_time: f32,                     // 下次触发时间, 同时播放特效
    pub delay_damage_time: f32,                 // 触发后延迟多久，开始伤害判定
    pub damage_shape: LogicDamageShape,         // 伤害判定形状
    pub damage: i32,                            // 伤害值
    pub cd: f32,                                // 技能冷却时间
    pub cue_effect_id: i32,                     // 触发特效id
    pub hit_effect_id: i32,                     // 命中特效id
    pub actor_id: i32,                          // 施放者id
}

#[repr(C)]
#[derive(Default, Debug, Clone)]
pub struct LogicData {
    pub current_time: f32,
    pub actor_spawner: ActorSpawnerData,
    pub effect_spawner: EffectSpawnerData,
    pub skills: Vec<LogicSkillData>,
    pub character0: LogicCharacterData,
    pub monsters: HashMap<i32, LogicCharacterData>,
    pub effects: HashMap<i32, LogicEffectData>,
}