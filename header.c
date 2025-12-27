/**
 *  @file header.c
 *  @brief drumlogue SDK unit header
 *
 *  Copyright (c) 2020-2022 KORG Inc. All rights reserved.
 *
 */

#include "unit.h"  // Note: Include common definitions for all units

// ---- Unit header definition  --------------------------------------------------------------------

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),                     // leave as is, size of this header
    .target = UNIT_TARGET_PLATFORM | k_unit_module_masterfx,  // target platform and module for this unit
    .api = UNIT_API_VERSION,                                  // logue sdk API version against which unit was built
    .dev_id = 0x0U,                                           // developer id
    .unit_id = 0x0U,                                          // Id for this unit, should be unique within the scope of a given dev_id
    .version = 0x00010000U,                                   // This unit's version: major.minor.patch (major<<16 minor<<8 patch).
    .name = "poutoushaper",                                  // Name for this unit, will be displayed on device
    .num_presets = 0,                                         // Number of internal presets this unit has
    .num_params = 3,                                          // Number of parameters for this unit, max 24
    .params = {
        // Format: min, max, center, default, type, fractional, frac. type, <reserved>, name

        // See common/runtime.h for type enum and unit_param_t structure

        // Page 1
        // percent param with .5 precision e.g.: "25.0%", "50.5%"
        {0, 3, 0, 0, k_unit_param_type_strings, 0, 0, 0, {"Shape"}},
        {-15, 30, 0, 0, k_unit_param_type_db, 0, 0, 0, {"Gain"}},
        {0, 1, 0, 0, k_unit_param_type_onoff, 0, 0, 0, {"Clip"}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 2
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 3
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 4
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 5
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 6
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}}}};
