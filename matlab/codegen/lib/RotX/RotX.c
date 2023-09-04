/*
 * File: RotX.c
 *
 * MATLAB Coder version            : 5.1
 * C/C++ source code generated on  : 30-Oct-2022 15:49:34
 */

/* Include Files */
#include "RotX.h"
#include <math.h>

/* Function Definitions */
/*
 * Arguments    : float a
 *                float f[9]
 * Return Type  : void
 */
void RotX(float a, float f[9])
{
  float b_f_tmp;
  float f_tmp;
  a *= 0.0174532924F;
  f_tmp = sinf(a);
  b_f_tmp = cosf(a);
  f[0] = 1.0F;
  f[3] = 0.0F;
  f[6] = 0.0F;
  f[1] = 0.0F;
  f[4] = b_f_tmp;
  f[7] = -f_tmp;
  f[2] = 0.0F;
  f[5] = f_tmp;
  f[8] = b_f_tmp;
}

/*
 * File trailer for RotX.c
 *
 * [EOF]
 */
