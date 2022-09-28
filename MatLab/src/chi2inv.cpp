/*
 * File: mychi2inv.c
 *
 * MATLAB Coder version            : 2.6
 * C/C++ source code generated on  : 17-Jun-2017 16:28:45
 */

/* Include files */
#include "../include/chi2inv.h"
namespace MatLab 
{
	/* Function Definitions */

	/*
	 * Arguments    : double p
	 *                double v
	 * Return Type  : double
	 */
	double chi2inv(double p, double v)
	{
		double x;
		double vd2;
		double lga;
		double lgap1;
		vd2 = v / 2.0;
		if ((0.0 <= vd2) && (!rtIsInf(vd2)) && (p >= 0.0) && (p <= 1.0)) {
			if ((p > 0.0) && (p < 1.0) && (vd2 > 0.0)) {
				lga = gammaln(vd2);
				lgap1 = gammaln(vd2 + 1.0);
				vd2 = eml_gammaincinv(p, vd2, log(vd2), lga, lgap1);
				x = vd2 * 2.0;
			}
			else if ((vd2 == 0.0) || (p == 0.0)) {
				x = 0.0;
			}
			else if (p == 1.0) {
				x = rtInf;
			}
			else {
				x = rtNaN;
			}
		}
		else {
			x = rtNaN;
		}

		return x;
	}
}
/*
 * File trailer for mychi2inv.c
 *
 * [EOF]
 */
