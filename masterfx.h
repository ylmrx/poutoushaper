#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <math.h>

#include <arm_neon.h>

#include "unit.h"  // Note: Include common definitions for all units

enum Params {
  Shape = 0,
  Gain,
  Clip
};

class Saturator {
  public:
    float32x4_t squared(float32x4_t x) {
      return vmulq_f32(x, x);
    }

    float32x4_t cubed(float32x4_t x) {
      return vmulq_f32(squared(x), x);
    }

    float32x4_t vdiv(float32x4_t x, float32x4_t y) {
      float32x4_t rec = vrecpeq_f32(y);
      return vmulq_f32(x, rec);
    }

    float32x4_t vectorHardClip(float32x4_t input) {
      const float32x4_t min_val = vdupq_n_f32(-1.0f);
      const float32x4_t max_val = vdupq_n_f32(1.0f);
      return vmaxq_f32(vminq_f32(input, max_val), min_val);
    }

    float32x4_t vectorSoftClip(float32x4_t input) {
      // reimplementation of maximilian formula, but unclipped
      return vmulq_f32(
          vsubq_f32(input, vmulq_f32(cubed(input), vdupq_n_f32(0.33333f))),
          vdupq_n_f32(0.66667f)
        );
    }

    float32x4_t vectorFastAtan(float32x4_t input) {
      return vdiv(input, vaddq_f32(
        vdupq_n_f32(1.0f),
        vmulq_f32(vdupq_n_f32(0.28f), squared(input))));
    }

    float32x4_t vectorPSoftClip(float32x4_t input) {
      float32x4_t abs_in = vabsq_f32(input);
      return vmulq_f32(input,
        vsubq_f32(
          vdupq_n_f32(1.0f),
          vmulq_f32(abs_in, vdupq_n_f32(0.5f))
        )
      );
    }
};

class MasterFX {
 public:
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

  fast_inline void Process(const float * in, float * out, size_t frames) {
    const float * __restrict in_p = in;
    float * __restrict out_p = out;
    const float * out_e = out_p + (frames << 1);  // assuming stereo output

    float32x4_t mul_vec = vdupq_n_f32(powf(10.0f, gain_ * 0.05f));

    for (; out_p != out_e; in_p += 4, out_p += 2) {
      float32x4_t input_vec = vld1q_f32(in_p);
      float32x4_t driver_vec = vmulq_f32(input_vec, mul_vec);
      float32x4_t output_vec;
      switch (shape_)
      {
      case 3:
        if (clip_ == 1) {
          output_vec = sat.vectorHardClip(sat.vectorPSoftClip(driver_vec));
        } else {
          output_vec = sat.vectorPSoftClip(driver_vec);
        }
        break;
      case 2:
        if (clip_ == 1) {
          output_vec = sat.vectorHardClip(sat.vectorFastAtan(driver_vec));
        } else {
          output_vec = sat.vectorFastAtan(driver_vec);
        }
        break;
      case 1:
        output_vec = sat.vectorHardClip(driver_vec);
        break;

      default:
        if (clip_ == 1) {
          output_vec = sat.vectorHardClip(sat.vectorSoftClip(driver_vec));
        } else {
          output_vec = sat.vectorSoftClip(driver_vec);
        }
      }

      vst1_f32(out_p, vget_low_f32(output_vec));
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
      case Clip:
        clip_ = value;
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
        if (value < 4) {
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

  static inline const char * getPresetName(uint8_t idx) {
    (void)idx;
    // Note: String memory must be accessible even after function returned.
    //       It can be assumed that caller will have copied or used the string
    //       before the next call to getPresetName
    return nullptr;
  }

 private:
  std::atomic_uint_fast32_t flags_;
  int32_t p_[24];
  Saturator sat;
  float gain_;
  int shape_;
  int clip_;
  const char *ShapeStr[4] = {
    "Soft",
    "Hard",
    "Atan",
    "PSoft"
  };
};
