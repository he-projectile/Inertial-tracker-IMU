/*
 * File: _coder_RotX_api.c
 *
 * MATLAB Coder version            : 5.1
 * C/C++ source code generated on  : 30-Oct-2022 15:49:34
 */

/* Include Files */
#include "_coder_RotX_api.h"
#include "_coder_RotX_mex.h"

/* Variable Definitions */
emlrtCTX emlrtRootTLSGlobal = NULL;
emlrtContext emlrtContextGlobal = { true,/* bFirstTime */
  false,                               /* bInitialized */
  131595U,                             /* fVersionInfo */
  NULL,                                /* fErrorFunction */
  "RotX",                              /* fFunctionName */
  NULL,                                /* fRTCallStack */
  false,                               /* bDebugMode */
  { 2045744189U, 2170104910U, 2743257031U, 4284093946U },/* fSigWrd */
  NULL                                 /* fSigMem */
};

/* Function Declarations */
static real32_T b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId);
static real32_T c_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId);
static real32_T emlrt_marshallIn(const emlrtStack *sp, const mxArray *a, const
  char_T *identifier);
static const mxArray *emlrt_marshallOut(const real32_T u[9]);

/* Function Definitions */
/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : real32_T
 */
static real32_T b_emlrt_marshallIn(const emlrtStack *sp, const mxArray *u, const
  emlrtMsgIdentifier *parentId)
{
  real32_T y;
  y = c_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 * Return Type  : real32_T
 */
static real32_T c_emlrt_marshallIn(const emlrtStack *sp, const mxArray *src,
  const emlrtMsgIdentifier *msgId)
{
  static const int32_T dims = 0;
  real32_T ret;
  emlrtCheckBuiltInR2012b(sp, msgId, src, "single|double", false, 0U, &dims);
  emlrtImportArrayR2015b(sp, src, &ret, 4, false);
  emlrtDestroyArray(&src);
  return ret;
}

/*
 * Arguments    : const emlrtStack *sp
 *                const mxArray *a
 *                const char_T *identifier
 * Return Type  : real32_T
 */
static real32_T emlrt_marshallIn(const emlrtStack *sp, const mxArray *a, const
  char_T *identifier)
{
  emlrtMsgIdentifier thisId;
  real32_T y;
  thisId.fIdentifier = (const char_T *)identifier;
  thisId.fParent = NULL;
  thisId.bParentIsCell = false;
  y = b_emlrt_marshallIn(sp, emlrtAlias(a), &thisId);
  emlrtDestroyArray(&a);
  return y;
}

/*
 * Arguments    : const real32_T u[9]
 * Return Type  : const mxArray *
 */
static const mxArray *emlrt_marshallOut(const real32_T u[9])
{
  static const int32_T iv[2] = { 3, 3 };

  const mxArray *m;
  const mxArray *y;
  int32_T b_i;
  int32_T i;
  real32_T *pData;
  y = NULL;
  m = emlrtCreateNumericArray(2, &iv[0], 7, 0);
  pData = (real32_T *)emlrtMxGetData(m);
  i = 0;
  for (b_i = 0; b_i < 3; b_i++) {
    pData[i] = u[3 * b_i];
    i++;
    pData[i] = u[3 * b_i + 1];
    i++;
    pData[i] = u[3 * b_i + 2];
    i++;
  }

  emlrtAssign(&y, m);
  return y;
}

/*
 * Arguments    : const mxArray * const prhs[1]
 *                const mxArray *plhs[1]
 * Return Type  : void
 */
void RotX_api(const mxArray * const prhs[1], const mxArray *plhs[1])
{
  emlrtStack st = { NULL,              /* site */
    NULL,                              /* tls */
    NULL                               /* prev */
  };

  real32_T f[9];
  real32_T a;
  st.tls = emlrtRootTLSGlobal;

  /* Marshall function inputs */
  a = emlrt_marshallIn(&st, emlrtAliasP(prhs[0]), "a");

  /* Invoke the target function */
  RotX(a, f);

  /* Marshall function outputs */
  plhs[0] = emlrt_marshallOut(f);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void RotX_atexit(void)
{
  emlrtStack st = { NULL,              /* site */
    NULL,                              /* tls */
    NULL                               /* prev */
  };

  mexFunctionCreateRootTLS();
  st.tls = emlrtRootTLSGlobal;
  emlrtEnterRtStackR2012b(&st);
  emlrtLeaveRtStackR2012b(&st);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
  RotX_xil_terminate();
  RotX_xil_shutdown();
  emlrtExitTimeCleanup(&emlrtContextGlobal);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void RotX_initialize(void)
{
  emlrtStack st = { NULL,              /* site */
    NULL,                              /* tls */
    NULL                               /* prev */
  };

  mexFunctionCreateRootTLS();
  st.tls = emlrtRootTLSGlobal;
  emlrtClearAllocCountR2012b(&st, false, 0U, 0);
  emlrtEnterRtStackR2012b(&st);
  emlrtFirstTimeR2012b(emlrtRootTLSGlobal);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void RotX_terminate(void)
{
  emlrtStack st = { NULL,              /* site */
    NULL,                              /* tls */
    NULL                               /* prev */
  };

  st.tls = emlrtRootTLSGlobal;
  emlrtLeaveRtStackR2012b(&st);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
}

/*
 * File trailer for _coder_RotX_api.c
 *
 * [EOF]
 */
