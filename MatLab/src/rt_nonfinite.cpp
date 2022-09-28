/* 
 * File: rt_nonfinite.c 
 *  
 * MATLAB Coder version            : 2.6 
 * C/C++ source code generated on  : 12-Jun-2017 11:44:17 
 */

/*
 * Abstract:
 *      MATLAB for code generation function to initialize non-finites,
 *      (Inf, NaN and -Inf).
 */
#include "../include/rt_nonfinite.h"
#include "../include/rtGetNaN.h"
#include "../include/rtGetInf.h"
namespace MatLab 
{
	real_T rtInf = rtGetInf();
	real_T rtMinusInf = rtGetMinusInfF();
	real_T rtNaN = rtGetNaN();
	real32_T rtInfF = rtGetInfF();
	real32_T rtMinusInfF = rtGetMinusInfF();
	real32_T rtNaNF = rtGetNaNF();
	/* Function: rt_InitInfAndNaN ==================================================
	 * Abstract:
	 * Initialize the rtInf, rtMinusInf, and rtNaN needed by the
	 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
	 */
	void rt_InitInfAndNaN(size_t realSize)
	{
		/*(void)(realSize);*/
		static bool initDone = false;
		if (!initDone){
			rtNaN = rtGetNaN();
			rtNaNF = rtGetNaNF();
			rtInf = rtGetInf();
			rtInfF = rtGetInfF();
			rtMinusInf = rtGetMinusInf();
			rtMinusInfF = rtGetMinusInfF();
			initDone = true;
		}
	}

	/* Function: rtIsInf ==================================================
	 * Abstract:
	 * Test if value is infinite
	 */
	boolean_T rtIsInf(real_T value)
	{
		return (((value == rtInf) || (value == rtMinusInf)) ? 1U : 0U);
	}

	/* Function: rtIsInfF =================================================
	 * Abstract:
	 * Test if single-precision value is infinite
	 */
	boolean_T rtIsInfF(real32_T value)
	{
		return (((value == rtInfF) || (value == rtMinusInfF)) ? 1U : 0U);
	}

	/* Function: rtIsNaN ==================================================
	 * Abstract:
	 * Test if value is not a number
	 */
	boolean_T rtIsNaN(real_T value)
	{
#if defined(_MSC_VER) && (_MSC_VER <= 1200)
		return _isnan(value)? TRUE:FALSE;
#else
		return (value != value) ? 1U : 0U;
#endif
	}

	/* Function: rtIsNaNF =================================================
	 * Abstract:
	 * Test if single-precision value is not a number
	 */
	boolean_T rtIsNaNF(real32_T value)
	{
#if defined(_MSC_VER) && (_MSC_VER <= 1200)
		return _isnan((real_T)value)? true:false;
#else
		return (value != value) ? 1U : 0U;
#endif
	}

}
/* 
 * File trailer for rt_nonfinite.c 
 *  
 * [EOF] 
 */
