#include "../include/ukf.h"
#include "../../MatLab/include/gamma.h"
#include "../../MatLab/include/chi2inv.h"
#include "../../mvnpdf/include/mvnpdf.h"
#include <cassert> // for assert
#include <cmath>
namespace Filters
{
	UKF::UKF() {}
	UKF::UKF(const UKF &l)
	{
		statePost0 = l.statePost0.clone();
		errorCovPost0 = l.errorCovPost0.clone();
		statePre = l.statePre.clone();
		errorCovPre = l.errorCovPre.clone();
		measPre = l.measPre.clone();
		S_t = l.S_t.clone();
		K_t = l.K_t.clone();
		statePost = l.statePost.clone();
		errorCovPost = l.errorCovPost.clone();
		term1 = l.term1.clone();
		term2 = l.term2.clone();
	}
	UKF& UKF::operator=(const UKF &l)
	{
		statePost0 = l.statePost0.clone();
		errorCovPost0 = l.errorCovPost0.clone();
		statePre = l.statePre.clone();
		errorCovPre = l.errorCovPre.clone();
		measPre = l.measPre.clone();
		S_t = l.S_t.clone();
		K_t = l.K_t.clone();
		statePost = l.statePost.clone();
		errorCovPost = l.errorCovPost.clone();
		term1 = l.term1.clone();
		term2 = l.term2.clone();
		return *this;
	}
	void UKF::commence(const cv::Mat1d &x0, const cv::Mat1d &P0)
	{
		assert(1 == x0.cols);
		assert((P0.rows == P0.cols) && (x0.rows == P0.rows));
		statePost = x0.clone();
		errorCovPost = P0.clone();
		errorCovPost = (errorCovPost + errorCovPost.t()) / 2;
		equatePost0ToPost();
	}
	const cv::Mat1d& UKF::predictState(const cv::Mat1d &F_t, const cv::Mat1d &Q_t)
	{
		cv::Mat1d F_t_ = F_t.clone();
		F_t_(cv::Range(F_t_.rows - 1, F_t_.rows), cv::Range(0, F_t_.cols - 1)) = 0;
		F_t_(cv::Range(0, F_t_.rows - 1), cv::Range(F_t_.cols - 1, F_t_.cols)) = 0;

		int Ns = statePost0.rows;

		double c, OOM;
		cv::Mat1d Wmean, Wcov;
		calcUTParameters(Ns, c, Wmean, Wcov, OOM);
		
		cv::Mat1d X1;
		cv::Mat1d X2state;
		calcSigmaPoints(errorCovPost0, statePost0, c, X1, X2state);
		
		cv::Mat1d Y1 = F_t_ * X1;
		cv::Mat1d Y2 = cv::Mat1d::zeros(Ns, 2 * Ns);
		for (int kk = 0; kk < Y2.cols; ++kk) {
			Y2.setTo(0, kk, F_t_ * X2state.col(kk));
		}
		
		cv::Mat1d Ymean, Pyy, Pxy;
		UTMeanCov(Wmean, Wcov, OOM, Y1, Y2, X1, X2state, Ymean, Pyy, Pxy);
		
		statePre = Ymean;
		errorCovPre = Pyy + Q_t;
		errorCovPre = (errorCovPre + errorCovPre.t()) / 2;

		return statePre;
	}
	const cv::Mat1d& UKF::predictMeas(const cv::Mat1d &H_t, const cv::Mat1d &R_t)
	{
		int Ns = statePre.rows;
		int Ny = H_t.rows;

		double c, OOM;
		cv::Mat1d Wmean, Wcov;
		calcUTParameters(Ns, c, Wmean, Wcov, OOM);
		
		cv::Mat1d X1;
		cv::Mat1d X2state;
		calcSigmaPoints(errorCovPre, statePre, c, X1, X2state);

		cv::Mat1d Y1 = H_t * X1;
		cv::Mat1d Y2 = cv::Mat1d::zeros(Ny, 2 * Ns);
		for (int kk = 0; kk < Y2.cols; ++kk) {
			Y2.setTo(0, kk, H_t * X2state.col(kk));
		}

		cv::Mat1d Ymean, Pyy, Pxy;
		UTMeanCov(Wmean, Wcov, OOM, Y1, Y2, X1, X2state, Ymean, Pyy, Pxy);

		measPre = Ymean;
		S_t = Pyy + R_t;
		S_t = (S_t + S_t.t()) / 2;
		K_t = Pxy * S_t.inv();

		return measPre;
	}
	const cv::Mat1d& UKF::update(const cv::Mat1d &Y_t, cv::Point *maxLoc_ptr)
	{
		if (0 == Y_t.cols) { // update in case of no measurement
			term1 = cv::Mat1d(1, 1, 1);
			statePost = statePre.clone();
			errorCovPost = errorCovPre.clone();
		}
		else {
			double maxVal;
			cv::Point maxLoc;
			cv::minMaxLoc(Utility::mvnpdf(Y_t, measPre, S_t), NULL, &maxVal, NULL, &maxLoc);
			term1 = cv::Mat1d(1, 1, maxVal);
			cv::Mat1d meas = Y_t.col(maxLoc.y);
			statePost = statePre + K_t * (meas - measPre);
			errorCovPost = errorCovPre - K_t * S_t * K_t.t();
			errorCovPost = (errorCovPost + errorCovPost.t()) / 2;
			if (NULL != maxLoc_ptr) {
				*maxLoc_ptr = maxLoc;
			}
		}
		term2 = cv::Mat1d(1, 1, 1);
		equatePost0ToPost();
		return statePost;
	}
	const cv::Mat1d& UKF::update(const cv::Mat1d &Y_t, double P_D, double P_G, double lambda)
	{
		assert(0 <= P_D && P_D <= 1);
		assert(0 <= P_G && P_G <= 1);
		cv::Mat1d Nu_t = innoMat(Y_t);
		cv::Mat1d beta = assoProb(Nu_t, P_D, P_G, lambda);
		cv::Mat1d cNu = Nu_t * beta;
		statePost = statePre + K_t * cNu;
		cv::Mat1d errorCovPost_c = errorCovPre - K_t * S_t * K_t.t();
		errorCovPost_c = (errorCovPost_c + errorCovPost_c.t()) / 2;
		cv::Mat1d errorCovPost_spread = K_t * (Nu_t * cv::Mat1d::diag(beta) * Nu_t.t() - cNu * cNu.t()) * K_t.t();
		errorCovPost_spread = (errorCovPost_spread + errorCovPost_spread.t()) / 2;
		errorCovPost = beta(0) * errorCovPre + (1 - beta(0)) * errorCovPost_c + errorCovPost_spread;
		errorCovPost = (errorCovPost + errorCovPost.t()) / 2;
		equatePost0ToPost();
		return statePost;
	}
	void UKF::calcUTParameters(int n, double &c, cv::Mat1d &Wmean, cv::Mat1d &Wcov, double &OOM)
	{
		c = std::pow(alpha, 2) * (n + kappa);

		Wmean = cv::Mat1d::zeros(2, 1);
		Wmean(0) = 1 - n / c;
		Wmean(1) = 1 / (2 * c);
		
		Wcov = cv::Mat1d::zeros(2, 1);
		Wcov(0) = Wmean(0) + (1 - std::pow(alpha, 2) + beta);
		Wcov(1) = Wmean(1);

		if (0 != Wmean(0)) {
			OOM = Wmean(0);
			Wmean = Wmean / OOM;
			Wcov = Wcov / OOM;
		}
		else {
			OOM = 1;
		}
	}
	void UKF::calcSigmaPoints(const cv::Mat1d &P, const cv::Mat1d &X, double c, cv::Mat1d &X1, cv::Mat1d &X2)
	{
		cv::Mat1d sqrtP = cv::chol(P);
		sqrtP = std::sqrt(c) * sqrtP;

		X1 = X.clone();
		X2 = cv::Mat1d::zeros(sqrtP.rows, 2 * sqrtP.cols);
		for (int kkC = 0; kkC < sqrtP.cols; ++kkC) {
			X2.setTo(0, kkC				, X1 + sqrtP.col(kkC));
			X2.setTo(0, sqrtP.cols + kkC, X1 - sqrtP.col(kkC));
		}
	}
	void UKF::UTMeanCov(const cv::Mat1d &Wmean, const cv::Mat1d &Wcov, double OOM,
						cv::Mat1d Y1, cv::Mat1d Y2,
						cv::Mat1d X1, cv::Mat1d X2,
						cv::Mat1d &Ymean, cv::Mat1d &Pyy, cv::Mat1d &Pxy)
	{
		Ymean = Wmean(0) * Y1;
		for (int kk = 0; kk < Y2.cols; ++kk) {
			Ymean = Ymean + Wmean(1) * Y2.col(kk);
		}

		// Rescale to the correct order of magnitude
		Ymean = OOM * Ymean;

		Y1 = Y1 - Ymean;
		for (int kk = 0; kk < Y2.cols; ++kk) {
			Y2.setTo(0, kk, Y2.col(kk) - Ymean);
		}

		Pyy = Wcov(0) * Y1 * Y1.t() + Wcov(1) * Y2 * Y2.t();
		Pyy = OOM * Pyy;

		for (int kk = 0; kk < X2.cols; ++kk) {
			X2.setTo(0, kk, X2.col(kk) - X1);
		}

		cv::Mat1d Y22 = cv::Mat1d::zeros(X2.rows, X2.cols);
		for (int kk = 0; kk < Y22.cols; ++kk) {
			Y22.setTo(0, kk, Y2.col(kk));
		}

		Pxy = X2 * Y22.t();
		Pxy = Wcov(1) * OOM * Pxy;
	}
	cv::Mat1d UKF::innoMat(const cv::Mat1d &Y_t)
	{
		int n_y = Y_t.rows;
		int m_t = Y_t.cols;
		//! Innnovation matrix : The matrix containing each innovation nu(j) = Y_t(j) - measPre; as jth column
		//! where j = 0 correponds to none of the measurements is correct 
		//! OR there is no validated measurements
		//! for  which nu(0) = 0;
		cv::Mat1d Nu_t = cv::Mat1d::zeros(n_y, m_t + 1); // + 1 to account for j = 0 case.
		if (m_t > 0) {
			Nu_t.setTo(0, 1, Y_t - cv::repeat(measPre, 1, m_t));
		}
		return Nu_t;
	}
	cv::Mat1d UKF::assoProb(const cv::Mat1d &Nu_t, double P_D, double P_G, double lambda)
	{
		int n_y = Nu_t.rows;
		int m_t = Nu_t.cols - 1;
		if (0 == m_t) {
			term1 = cv::Mat1d(1, 1, 1);
			term2 = cv::Mat1d(1, 1, 1 - P_D * P_G);
			cv::Mat1d beta = cv::Mat1d(1, 1, 1);
			return beta;
		}
		term1 = Utility::mvnpdf(Nu_t, cv::Mat1d::zeros(n_y, 1), S_t);
		term1(0) = 1;
		MatLab::rt_InitInfAndNaN(8U);
		double V_t = std::pow(CV_PI, (double)n_y / 2) / MatLab::gamma((double)n_y / 2 + 1) * std::pow(MatLab::chi2inv(1 == P_G ? 1 - 1e-16 : P_G, (double)n_y), (double)n_y / 2) * std::sqrt(determinant(S_t)); //< volume of the validation region
		if (lambda <= 0) { /* Non Parametric PDAF => diffuse prior model for # false measurements (clutter) */
			lambda = m_t / V_t;
		}
		term2 = cv::Mat1d(m_t + 1, 1, P_D);
		term2(0) = lambda * (1 - P_D * P_G);
		// term2 *= std::pow(V_t, -m_t + 1) * std::pow(P_D * P_G * m_t + (1 - P_D * P_G) * lambda * V_t, -1); // This is causing numerical unstability
		cv::Mat1d beta = term1.mul(term2);
		// L1 Normalize
		beta = beta / cv::norm(beta, cv::NORM_L1); // Note : DO NOT USE cv::normalize function
		return beta;
	}
	void UKF::equatePost0ToPost(void)
	{
		statePost0 = statePost.clone();
		errorCovPost0 = errorCovPost.clone();
	}
}