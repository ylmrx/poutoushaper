#pragma once
/*
 *  File: master.h
 *
 *  Dummy Master Effect Class
 *
 *  Author: Etienne Noreau-Hebert <etienne@korg.co.jp>
 *
 *  2021 (c) Korg
 *
 */

#include <atomic>
#include <cstddef>
#include <cstdint>

#include <arm_neon.h>

#include "unit.h"  // Note: Include common definitions for all units

#include "maximilian.h"
// #include "libs/maxiPolyBLEP.h"

enum Params {
  Shape = 0,
  Gain,
  Taste
};

class MasterFX {
 public:
  /*===========================================================================*/
  /* Public Data Structures/Types. */
  /*===========================================================================*/

  /*===========================================================================*/
  /* Lifecycle Methods. */
  /*===========================================================================*/

  MasterFX(void) {}
  virtual ~MasterFX(void) {}

  inline int8_t Init(const unit_runtime_desc_t * desc) {
    // Check compatibility of samplerate with unit, for drumlogue should be 48000
    if (desc->samplerate != 48000)  // Note: samplerate format may change to add fractional bits
      return k_unit_err_samplerate;

    // Check compatibility of frame geometry
    // Note: input format: [ main_left, main_right, sidechain_left, sidechain_right ]
    // Note: Sidechain feature may unfortunately get removed before official release, in which case
    //       there will be only 2 input channels
    if (desc->input_channels != 4 || desc->output_channels != 2)
      return k_unit_err_geometry;

    // Note: if need to allocate some memory can do it here and return k_unit_err_memory if getting allocation errors

    return k_unit_err_none;
  }

  inline void Teardown() {
    // Note: cleanup and release resources if any
  }

  inline void Reset() {
    // Note: Reset effect state.
  }

  inline void Resume() {
    // Note: Effect will resume and exit suspend state. Usually means the synth
    // was selected and the render callback will be called again
  }

  inline void Suspend() {
    // Note: Effect will enter suspend state. Usually means another effect was
    // selected and thus the render callback will not be called
  }

  /*===========================================================================*/
  /* Other Public Methods. */
  /*===========================================================================*/
  
  fast_inline void Process(const float * in, float * out, size_t frames) {
    const float * __restrict in_p = in;
    float * __restrict out_p = out;
    const float * out_e = out_p + (frames << 1);  // assuming stereo output

    // comp_.setAttackHigh(10.f);
    // comp_.setReleaseHigh(40.f);
    float mul = maxiConvert::dbsToAmp(gain_);
    for (; out_p != out_e; in_p += 4, out_p += 2) {
      // Note: should take advantage of NEON ArmV7 instructions
      // float32x4_t sig = vld1q_f32(in_p);
      // vst1_f32(out_p, vget_low_f32(sig));
      // float in = (in_p[0] + in_p[1]) / 2.f;
      for (int c = 0; c < 2; c++) {
        float driven = in_p[c] *  mul;
        if (shape_ == 2) {
          out_p[c] = sat.fastatan(driven);
        } else if (shape_ == 1) {
          out_p[c] = sat.softclip(driven);
        } else {
          out_p[c] = sat.hardclip(driven);
        }
      }
    }
  }

  inline void setParameter(uint8_t index, int32_t value) {
    p_[index] = value;
    switch (index) {
      case Gain:
        gain_ = value;
        break;
      case Shape:
        shape_ = value;
        break;
      default:
        break;
    }
  }

  inline int32_t getParameterValue(uint8_t index) const {
    return p_[index];
  }

  inline const char * getParameterStrValue(uint8_t index, int32_t value) const {
    (void)value;

    switch (index) {
      case Shape:
        if (value < 3) {
          return ShapeStr[value];
        } else {
          return nullptr;
        }
        break;
      // Note: String memory must be accessible even after function returned.
      //       It can be assumed that caller will have copied or used the string
      //       before the next call to getParameterStrValue
      default:
        break;
    }
    return nullptr;
  }

  inline const uint8_t * getParameterBmpValue(uint8_t index,
                                              int32_t value) const {
    (void)value;
    switch (index) {
      // Note: Bitmap memory must be accessible even after function returned.
      //       It can be assumed that caller will have copied or used the bitmap
      //       before the next call to getParameterBmpValue
      // Note: Not yet implemented upstream
      default:
        break;
    }
    return nullptr;
  }

  inline void LoadPreset(uint8_t idx) { (void)idx; }

  inline uint8_t getPresetIndex() const { return 0; }

  /*===========================================================================*/
  /* Static Members. */
  /*===========================================================================*/

  static inline const char * getPresetName(uint8_t idx) {
    (void)idx;
    // Note: String memory must be accessible even after function returned.
    //       It can be assumed that caller will have copied or used the string
    //       before the next call to getPresetName
    return nullptr;
  }

 private:
  /*===========================================================================*/
  /* Private Member Variables. */
  /*===========================================================================*/

  std::atomic_uint_fast32_t flags_;
  int32_t p_[24];
  maxiNonlinearity sat;
  float gain_;
  int shape_;

  // maxiConvert util;

  /*===========================================================================*/
  /* Private Methods. */
  /*===========================================================================*/

  /*===========================================================================*/
  /* Constants. */
  /*===========================================================================*/
  const char *ShapeStr[3] = {
    "Soft",
    "Hard",
    "Atan",
  };
};
