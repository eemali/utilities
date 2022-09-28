#include "../include/mvnpdf.h"
#include <cmath>
#include <float.h>
#include <algorithm> // for std::max
namespace Utility
{
	cv::Mat1d mvnpdf(const cv::Mat1d &Y, const cv::Mat1d &Mu, const cv::Mat1d &S)
	{
		cv::Mat1d Nu = Y - cv::repeat(Mu, 1, Y.cols);
		cv::Mat1d exponent = -0.5 * (Nu.t() * S.inv() * Nu).diag();
		cv::Mat1d numerator =	cv::exp(exponent);
		cv::Mat1d likelihood = numerator / std::sqrt(cv::determinant(2 * CV_PI * S));
		// Avoid underflow
		for (int r = 0; r < likelihood.rows; r++) {
			likelihood(r) = std::max(likelihood(r), DBL_MIN);
		}
		return likelihood;
	}
}
