#ifndef MVN_PDF_H
#define MVN_PDF_H
#include "../../Mat/include/Mat.h"
namespace Utility
{
	/*! Multivariate normal probability density function		
		@param Y Columns of Y correspond to observations and rows correspond to variables or coordinates
		@param mu mean of multivariate normal distribution. n x 1 column vector
		@param S covariance matrix of multivariate normal distribution. n x n symmetric positive definite matrix.
		@return returns the density of the multivariate normal distribution with mean mu 
		and covariance matrix S, evaluated at each column of Y
	*/
	cv::Mat1d mvnpdf(const cv::Mat1d &Y, const cv::Mat1d &Mu, const cv::Mat1d &S);
}
#endif // !MVN_PDF_H