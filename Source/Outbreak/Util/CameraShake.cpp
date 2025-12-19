// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraShake.h"

UCameraShake::UCameraShake() {
	OscillationDuration = 0.5f;
	OscillationBlendInTime = 0.05f;
	OscillationBlendOutTime = 0.2f;

	RotOscillation.Pitch.Amplitude = 5.0f;
	RotOscillation.Pitch.Frequency = 25.0f;
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave; 
    
	RotOscillation.Yaw.Amplitude = 3.0f;
	RotOscillation.Yaw.Frequency = 20.0f;
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Yaw.Waveform = EOscillatorWaveform::SineWave;
    
	LocOscillation.Z.Amplitude = 10.0f;
	LocOscillation.Z.Frequency = 20.0f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Z.Waveform = EOscillatorWaveform::SineWave;
}
