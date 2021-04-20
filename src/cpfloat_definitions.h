/* SPDX-FileCopyrightText: 2020 Massimiliano Fasi and Mantas Mikaitis */
/* SPDX-License-Identifier: LGPL-2.1-or-later                         */

/**
 * @file cpfloat_definitions.h
 * @brief Definition of CPFloat data types.
 *
 * @details This file includes all the external header files used by CPFloat,
 * and defines the enumerated types
 *
 * + @ref cpfloat_subnormal_t,
 * + @ref cpfloat_explim_t,
 * + @ref cpfloat_rounding_t,
 * + @ref cpfloat_softerr_t,
 *
 * and the structured data type @ref optstruct. It is not necessary to include
 * this file in order to use CPFloat, as it is already included by @ref
 * cpfloat_binary32.h and by @ref cpfloat_binary64.h.
 */

#ifndef _CHOPFAST_DEFINITIONS_
#define _CHOPFAST_DEFINITIONS_

#include <stdlib.h>
#include <stdint.h>

#include <time.h>
#include <math.h>
#include <float.h>
#include <string.h>

/* #include "pcg_variants.h" */

#if defined(_OPENMP)
#include <omp.h>
#endif

/**
 * @brief Data type for specifying number of precision bits in target format.
 */
typedef unsigned int cpfloat_precision_t;

/**
 * @brief Data type for specifying exponents in target format.
 */
typedef int cpfloat_exponent_t;

/**
 * @brief Subnormal support modes available in CPFloat.
 */
typedef enum {
  /** Round subnormal numbers using current rounding mode. */
  CPFLOAT_SUBN_RND = 0,
  /** Support storage of subnormal numbers. */
  CPFLOAT_SUBN_USE = 1
} cpfloat_subnormal_t;

/**
 * @brief Extended exponent range modes available in CPFloat.
 */
typedef enum {
  /** Use exponent range of storage format. */
  CPFLOAT_EXPRANGE_STOR = 0,
  /** Use exponent range of target format. */
  CPFLOAT_EXPRANGE_TARG = 1
} cpfloat_explim_t;

/**
 * @brief Rounding modes available in CPFloat.
 */
typedef enum {
  /** Use round-to-nearest with ties-to-away. */
  CPFLOAT_RND_NA = -1,
  /** Use round-to-nearest with ties-to-zero. */
  CPFLOAT_RND_NZ =  0,
  /** Use round-to-nearest with ties-to-even. */
  CPFLOAT_RND_NE =  1,
  /** Use round-toward-+&infin;. */
  CPFLOAT_RND_TP =  2,
  /** Use round-toward-&minus;&infin;. */
  CPFLOAT_RND_TN =  3,
  /** Use round toward zero */
  CPFLOAT_RND_TZ =  4,
  /** Stochastic rounding with proportional probabilities. */
  CPFLOAT_RND_SP =  5,
  /** Stochastic rounding with equal probabilities. */
  CPFLOAT_RND_SE =  6,
  /** Use round-to-odd. */
  CPFLOAT_RND_OD =  7,
  /** Do not perform rounding. */
  CPFLOAT_NO_RND =  8,
} cpfloat_rounding_t;

/**
 * @brief Soft fault simulation modes available in CPFloat.
 */
typedef enum {
  /** Do not simulate soft errors. */
  CPFLOAT_NO_SOFTERR = 0,
  /** Introduce soft errors after the floating-point conversion. */
  CPFLOAT_SOFTERR = 1
} cpfloat_softerr_t;

/**
 * @brief Specify target format, rounding mode, and occurrence of soft faults.
 *
 * @details The fields of this structure determine the parameters of the
 * floating-point format to be simulated, the rounding mode to be used during
 * the conversion process, and whether soft faults striking the rounded numbers
 * should be simulated.
 */
typedef struct {
  /**
   * @brief String specifying target format.
   *
   * @details This field is defined only for compatibility with the MATLAB
   * function `chop`, and its value is used by the MEX interface but ignored by
   * the pure C implementation.
   *
   * Possible values are:
   * + `b`, `bf16`, and `bfloat16` for bfloat16;
   * + `h`, `fp16`, `binary16`, and `half` for binary16;
   * + `t`, `tf32`, and `TensorFloat-32`, for TensorFloat-32;
   * + `s`, `fp32`, `binary32`, and `single` for binary32;
   * + `d`, `fp64`, `binary64`, and `double` for binary64; and
   * + `custom`, and `c` for a format specified using `precision` and `emax`.
   *
   * The validation functions cpfloatf_validate_optstruct() and
   * cpfloat_validate_optstruct() return a warning code if this field is not set
   * to either the empty string or one of the strings above.
   */
  char format [10];
  /**
   * @brief Bits of precision of target format.
   *
   * @details The maximum values allowed are 24 and 53 if the storage format is
   * `float` or `double`, respectively.
   *
   * For compatibility with the MATLAB function `chop`, in the MEX interface the
   * number of digits of precision for `float` and `double` cannot exceed 11 and
   * 25, respectively, when using stochastic rounding, and cannot exceed 23 and
   * 52, respectively, for other rounding modes. The C implementation does not
   * have any such restrictions, but note that using larger values can cause
   * double rounding.
   *
   * The validation functions cpfloatf_validate_optstruct() and
   * cpfloat_validate_optstruct() return an error code if the required number of
   * digits is larger than the maximum allowed by the storage format, and a
   * warning code if the required number of digits is above the maximum allowed
   * by the MEX interface.
   */
  cpfloat_precision_t precision;
  /**
   * @brief Maximum exponent of target format.
   *
   * @details The maximum values allowed are 127 and 1023 if the storage format
   * is `float` or `double`, respectively. Larger values are reduced to the
   * maximum allowed value without warning. This field is ignored unless
   * `explim` is set to `CPFLOAT_EXPRANGE_TARG`.
   *
   * The validation functions cpfloatf_validate_optstruct() and
   * cpfloat_validate_optstruct() return an error code if the required maximum
   * exponent is larger than the maximum allowed by the storage format.
   */
  cpfloat_exponent_t emax;
  /**
   * @brief Support for subnormal numbers in target format.
   *
   * @details Subnormal numbers are supported if this field is set to
   * `CPFLOAT_SUBN_USE` and rounded to a normal number using the current
   * rounding mode if it is set to `CPFLOAT_SUBN_RND`.
   */
  cpfloat_subnormal_t subnormal;
  /**
   * @brief Support for extended exponents in target format.
   *
   * @details The upper limit of the exponent range is set to `emax` if this
   * field is set to `CPFLOAT_EXPRANGE_TARG`, and to the upper limit of the
   * exponent range of the storage format if it is set to
   * `CPFLOAT_EXPRANGE_STOR`.
   */
  cpfloat_explim_t explim;
  /**
   * @brief Rounding mode to be used for the conversion.
   *
   * @details The values of this field are consistent with those of the MATLAB
   *function `chop`.
   *
   * Possible values are:
   * + CPFLOAT_RND_NA for round-to-nearest with ties-to-away;
   * + CPFLOAT_RND_NZ for round-to-nearest with ties-to-zero;
   * + CPFLOAT_RND_NE for round-to-nearest with ties-to-even;
   * + CPFLOAT_RND_TP for round-to-+&infin;
   * + CPFLOAT_RND_TN for round-to-&minus;&infin;
   * + CPFLOAT_RND_TZ for round-to-zero;
   * + CPFLOAT_RND_SP for stochastic rounding with proportional probabilities;
   * + CPFLOAT_RND_SE for stochastic rounding with equal probabilities;
   * + CPFLOAT_RND_OD for round-to-odd; and
   * + CPFLOAT_NO_RND for no rounding.
   *
   * No rounding is performed if this field is set to any other value.
   *
   * The validation functions cpfloatf_validate_optstruct() and
   * cpfloat_validate_optstruct() return a warning code if a value other than
   * those in the list above is specified.
   */
  cpfloat_rounding_t round;
  /**
   * @brief Support for soft errors.
   *
   * @details If this field is not set to `CPFLOAT_SOFTERR`, a single bit flip
   * in the significand of the rounded result is introduced with probability
   * `p`.
   */
  cpfloat_softerr_t flip;
  /**
   * @brief Probability of bit flips.
   *
   * @details The probability of flipping a single bit in the significand of a
   * floating-point number after rounding. This field is ignored if `flip` is
   * set to `CPFLOAT_NO_SOFTERR`.
   *
   * The validation functions cpfloatf_validate_optstruct() and
   * cpfloat_validate_optstruct() return an error code if `flip` is set to
   * `CPFLOAT_SOFTERR` and this field does not contain a number in the interval
   * [0,1].
   */
  double p;
} optstruct;

#endif // _CHOPFAST_DEFINITIONS_

/*
 * CPFloat - Custom Precision Floating-point numbers.
 *
 * Copyright 2020 Massimiliano Fasi and Mantas Mikaitis
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
