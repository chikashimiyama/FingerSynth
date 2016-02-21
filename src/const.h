#pragma once
#include "ofMain.h"

const int kMaxTouch = 5;
const int kCaptionOffset = 60;
const int kCircleResolution = 50;

const float kNormalLineWidth = 8;
const float kStemLineWidth = 6;
const float kWaveformLineWidth = 4;
const float kThinLineWidth = 1;
const float kCircleSize = 100;
const float kSmallCircleSize = 50;
const float kCentroidSize = 150;
const float kNormalFontSize = 18;
const float kInterpolationRatio = 0.1;
const int kSampleRate = 44100;
const int kBlockSize = 512;
const int kTicksPerBuffer = 8;
const int kArraySize = 512;
const float kWaveformScale = 1.0/kArraySize;
const float kCaptionMargin = 18;
const float kNearEnoughThreshold = 2.0;
