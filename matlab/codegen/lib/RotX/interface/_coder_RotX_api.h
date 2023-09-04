/*
 * File: _coder_RotX_api.h
 *
 * MATLAB Coder version            : 5.1
 * C/C++ source code generated on  : 30-Oct-2022 15:49:34
 */

#ifndef _CODER_ROTX_API_H
#define _CODER_ROTX_API_H

/* Include Files */
#include "emlrt.h"
#include "tmwtypes.h"
#include <string.h>

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

#ifdef __cplusplus

extern "C" {

#endif

  /* Function Declarations */
  void RotX(real32_T a, real32_T f[9]);
  void RotX_api(const mxArray * const prhs[1], const mxArray *plhs[1]);
  void RotX_atexit(void);
  void RotX_initialize(void);
  void RotX_terminate(void);
  void RotX_xil_shutdown(void);
  void RotX_xil_terminate(void);

#ifdef __cplusplus

}
#endif
#endif

/*
 * File trailer for _coder_RotX_api.h
 *
 * [EOF]
 */
