/*
 * File: _coder_RotX_mex.c
 *
 * MATLAB Coder version            : 5.1
 * C/C++ source code generated on  : 30-Oct-2022 15:49:34
 */

/* Include Files */
#include "_coder_RotX_mex.h"
#include "_coder_RotX_api.h"

/* Function Definitions */
/*
 * Arguments    : int32_T nlhs
 *                mxArray *plhs[1]
 *                int32_T nrhs
 *                const mxArray *prhs[1]
 * Return Type  : void
 */
void RotX_mexFunction(int32_T nlhs, mxArray *plhs[1], int32_T nrhs, const
                      mxArray *prhs[1])
{
  emlrtStack st = { NULL,              /* site */
    NULL,                              /* tls */
    NULL                               /* prev */
  };

  const mxArray *outputs[1];
  st.tls = emlrtRootTLSGlobal;

  /* Check for proper number of arguments. */
  if (nrhs != 1) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:WrongNumberOfInputs", 5, 12, 1, 4, 4,
                        "RotX");
  }

  if (nlhs > 1) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:TooManyOutputArguments", 3, 4, 4,
                        "RotX");
  }

  /* Call the function. */
  RotX_api(prhs, outputs);

  /* Copy over outputs to the caller. */
  emlrtReturnArrays(1, plhs, outputs);
}

/*
 * Arguments    : int32_T nlhs
 *                mxArray *plhs[]
 *                int32_T nrhs
 *                const mxArray *prhs[]
 * Return Type  : void
 */
void mexFunction(int32_T nlhs, mxArray *plhs[], int32_T nrhs, const mxArray
                 *prhs[])
{
  mexAtExit(&RotX_atexit);

  /* Module initialization. */
  RotX_initialize();

  /* Dispatch the entry-point. */
  RotX_mexFunction(nlhs, plhs, nrhs, prhs);

  /* Module termination. */
  RotX_terminate();
}

/*
 * Arguments    : void
 * Return Type  : emlrtCTX
 */
emlrtCTX mexFunctionCreateRootTLS(void)
{
  emlrtCreateRootTLS(&emlrtRootTLSGlobal, &emlrtContextGlobal, NULL, 1);
  return emlrtRootTLSGlobal;
}

/*
 * File trailer for _coder_RotX_mex.c
 *
 * [EOF]
 */
