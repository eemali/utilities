/*
 * File: mybino.c
 *
 * MATLAB Coder version            : 2.6
 * C/C++ source code generated on  : 23-Sep-2017 12:01:51
 */

/* Include files */
#include "../include/binoinv.h"
/* Function Definitions */

/*
 * Arguments    : double y
 *                int N
 *                double p
 * Return Type  : double
 */
namespace MatLab 
{
	double binoinv(double y, int N, double p)
	{
		double x;
		int xk;
		double cumdist;
		int32_T exitg1;
		double lc;
		if ((N >= 0) && (p >= 0.0) && (p <= 1.0) && (y >= 0.0) && (y <= 1.0)) {
			if (y == 1.0) {
				x = N;
			}
			else {
				xk = 0;
				cumdist = 0.0;
				do {
					exitg1 = 0;
					if (xk <= N) {
						if (p == 0.0) {
							lc = (xk == 0);
						}
						else if (p == 1.0) {
							lc = (xk == N);
						}
						else if (xk == 0) {
							lc = exp((double)N * log(1.0 - p));
						}
						else if (xk == N) {
							lc = exp((double)N * log(p));
						}
						else if (N < 10) {
							lc = -p;
							if (fabs(-p) < 2.2204460492503131E-16) {
								/* DO NOTHING */
							}
							else {
								lc = log(1.0 + -p) * (-p / ((1.0 + -p) - 1.0));
							}

							lc = (((gammaln((double)N + 1.0) - gammaln((double)xk + 1.0)) -
								gammaln((double)(N - xk) + 1.0)) + (double)xk * log(p)) +
								(double)(N - xk) * lc;
							lc = exp(lc);
						}
						else {
							lc = (((stirlerr(N) - stirlerr(xk)) - stirlerr(N - xk)) -
								binodeviance(xk, (double)N * p)) - binodeviance(N - xk,
								(double)N * (1.0 - p));
							lc = exp(lc) * sqrt((double)N / (6.2831853071795862 * (double)xk *
								(double)(N - xk)));
						}
					}
					else {
						lc = 0.0;
					}

					cumdist += lc;
					if ((cumdist > y) || (xk >= N)) {
						exitg1 = 1;
					}
					else {
						xk++;
					}
				} while (exitg1 == 0);

				x = xk;
			}
		}
		else {
			x = rtNaN;
		}

		return x;
	}
}
/*
 * File trailer for mybino.c
 *
 * [EOF]
 */
