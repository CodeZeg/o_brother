#pragma once

// Equivalent to FightingInputStateData
struct FightingInputStateData {
	float x; // x轴方向的输入
	float y; // y轴方向的输入
};

// Tag for InputStateData enum
enum InputStateData_Tag {
	InputStateData_Fighting,
	InputStateData_Other
};

// Equivalent to InputStateData
struct InputStateData {
	InputStateData_Tag tag;
	union {
		FightingInputStateData fighting; // Corresponds to Fighting variant
		// No data for Other variant
	} data;
};

// Tag for InputActionData enum
enum InputActionData_Tag {
	None,
	InputActionData_ChoseCard
	// Add other variants here if necessary
};

// Equivalent to InputActionData
struct InputActionData {
	InputActionData_Tag tag;
	union {
		uint8_t choseCard; // Corresponds to ChoseCard variant
		// Add other variant data here if necessary
	} data;
};

// Equivalent to InputPlayerData
struct InputPlayerData {
	InputStateData state;
	InputActionData action;
};

// Equivalent to InputData
struct InputData {
	InputPlayerData player_0;
};