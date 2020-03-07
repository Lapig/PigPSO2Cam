#pragma once
#ifndef CAMERA_H
#define CAMERA_H

namespace Camera {
	struct Offset {
		float UpDownStep = 0.3f;
		int DistMax = 30;
		int UpDown = 0;
		int Dist = 6;
		float UpDownMin = -0.3f;
		float Velocity = 0.5f;
		float UpDownMax = 0.6f;
		int DistMin = 4;
	};

	static struct Camera_ActorTrackTestNormal {
		int AngleMin = -65;
		double Velocity;
		int IdleOffset_y = 0;
		float TrackFactorAngleDefault = 0.1f;
		float TrackFactorPosition_x = 0.3f;
		int TrackAngleMax = 65;
		float TrackFactorAngle_y = 0.3f;
		int TrackAngleDefault = 8;
		int AngleMax = 40;
		char OnDistanceChanged;
		int IdleOffset_x = 1;
		int IdleDegree_y = 12;
		int IdleDegree_xz = 17;
		double ShakeEffect;
		int TrackAngleMin = -40;
		int PrintDebugFlag = 0;
		Offset Offset;
		float TrackFactorPosition_y = 0.3f;
		float TrackFactorPosition_z = 0.8f;
		float IdleOffset_z = 0.2f;
		int Fovy = 44;
		float TrackFactorAngle_xz = 0.3f;
	} cameraBase;

}

#endif