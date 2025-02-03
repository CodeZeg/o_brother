use hashbrown::HashMap as Map;
use alloc::vec::Vec;

pub trait LockstepData {
    /// 每帧的输入数据
    type InputData: Clone + Send + Sync + 'static;

    /// 持久化的切片数据（例如用于回放或断线重连）
    type PersistentData: Clone + Send + Sync + 'static;

    /// 渲染数据
    type RenderData: Clone + Send + Sync + 'static;

    /// 从初始数据创建状态
    fn new(initial_data: Self::PersistentData, inputs: Vec<Self::InputData>) -> Self;

    /// 应用输入数据，更新状态
    fn update(&mut self, input: &Self::InputData);

    /// 生成当前状态的持久化切片数据
    fn take_snapshot(&self) -> (Self::PersistentData, Vec<Self::InputData>);

    /// 获取任意时刻（包括逻辑帧上）的渲染数据
    fn take_render_data(&self) -> Self::RenderData;

    /// 获取当前最新的逻辑帧
    fn get_latest_frame_index(&self) -> i32;
}

pub struct LockstepSimulator<TData: LockstepData> {
    logic_state: TData,
    snapshots: Map<i32, TData::PersistentData>,
    inputs: Vec<TData::InputData>,
    render_state: Map<i32, TData::RenderData>,
}

impl<TData: LockstepData> LockstepSimulator<TData> {
    pub fn new(initial_data: TData::PersistentData, inputs: Vec<TData::InputData>) -> Self {
        let logic_state = TData::new(initial_data.clone(), inputs.clone());
        let mut snapshots = Map::new();
        snapshots.insert(inputs.len() as i32, initial_data);
        let mut render_state = Map::new();
        render_state.insert(inputs.len() as i32, logic_state.take_render_data());
        Self {
            logic_state,
            snapshots,
            inputs,
            render_state,
        }
    }

    pub fn update(&mut self, input: TData::InputData) {
        self.logic_state.update(&input);
        self.inputs.push(input);
        let render_state = self.logic_state.take_render_data();
        self.render_state
            .insert(self.inputs.len() as i32, render_state);
    }

    pub fn take_snapshot(&mut self) {
        todo!()
    }

    pub fn take_render_data(&self, frame_index: f32) -> TData::RenderData {
        let idx_left = frame_index as i32;
        let idx_right = idx_left + 1;
        let _alpha = frame_index - idx_left as f32;
        let state_left = self.render_state.get(&idx_left);
        let state_right = self.render_state.get(&idx_right);

        match (state_left, state_right) {
            (Some(left), _) => left.clone(),
            (_, Some(right)) => right.clone(),
            _ => {
                todo!("lerp impl")
            }
        }
    }
}
