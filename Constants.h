#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "JukeboxTypes.h"

#include <stddef.h> // to define size_t

const size_t kBatchSize = 64;
const size_t kMaxVoices = 128;
const TJBox_Float32 kReleaseTimeSeconds = 0.3f;
const TJBox_Float32 kPi = 3.14159265358979f;
const TJBox_Float32 kSemiToneFrequencyMultiplier = 1.05946309435929f; // 12th root of 2

const TJBox_Float32 k1 = 6.9078f; // ln(20000) - ln(20)
const TJBox_Float32 k2 = 2.9957f; // ln(20)


#endif
