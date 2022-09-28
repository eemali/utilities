#include "../include/immekf.h"
#include "../../MatLab/include/gamma.h"
#include "../../MatLab/include/chi2inv.h"
#include "../../mvnpdf/include/mvnpdf.h"
#include <cmath>
namespace Filters
{
	IMMEKF::IMMEKF() : filterBank(NULL)
	{
		assert(nModels > 0);
		assert(nModels == TPM.rows && nModels == TPM.cols);
		/* Ensure that each row of tpm matrix sums to one */
		for (int j = 0; j < nModels; ++j) {
			assert(1 == sum(TPM.row(j)));
		}
		assert(Mu0.rows == nModels && Mu0.cols == 1);
		/* Ensure that Mu0 sums to one */
		assert(1 == sum(Mu0));
		filterBank = new EKF[nModels];
	}
	IMMEKF::~IMMEKF()
	{
		if (NULL != filterBank) {
			delete[] filterBank;
			filterBank = NULL;
		}
	}
	IMMEKF::IMMEKF(const IMMEKF &l) : filterBank(NULL)
	{
		Mu = l.Mu.clone();
		filterBank = new EKF[nModels];
		for (int j = 0; j < nModels; ++j) {
			filterBank[j] = l.filterBank[j];
		}
		measPre = l.measPre.clone();
		S_t = l.S_t.clone();
		statePost = l.statePost.clone();
		errorCovPost = l.errorCovPost.clone();
		term2 = l.term2.clone();
	}
	IMMEKF& IMMEKF::operator=(const IMMEKF &l)
	{
		Mu = l.Mu.clone();
		for (int j = 0; j < nModels; ++j) {
			filterBank[j] = l.filterBank[j];
		}
		statePre = l.statePre.clone();
		errorCovPre = l.errorCovPre.clone();
		measPre = l.measPre.clone();
		S_t = l.S_t.clone();
		statePost = l.statePost.clone();
		errorCovPost = l.errorCovPost.clone();
		term2 = l.term2.clone();
		return *this;
	}
	void IMMEKF::commence(const cv::Mat1d &x0, const cv::Mat1d &P0)
	{
		Mu = Mu0.clone();
		for (int j = 0; j < nModels; ++j) {
			filterBank[j].commence(x0, P0);
		}
		this->overallStatePost();
	}
	void IMMEKF::mixing(void)
	{
		cv::Mat1d MPM = cv::Mat1d::diag(Mu) * TPM; //!< Mixing Probability Matrix			
		// L1 Normalize each column of MPM
		double col_norm;
		for (int j = 0; j < MPM.cols; ++j) {
			col_norm = cv::norm(MPM.col(j), cv::NORM_L1);
			for (int i = 0; i < MPM.rows; ++i) {
				MPM(i, j) = MPM(i, j) / col_norm;
			}
		}
		// Handle case when probability for a model becomes 0
		for (int j = 0; j < nModels; ++j) {
			if (0 == Mu(j)) {
				for (int i = 0; i < MPM.rows; ++i) {
					MPM(i, j) = 0;
				}
				MPM(j, j) = 1;
			}
		}
		for (int j = 0; j < nModels; ++j) {
			cv::Mat1d statePost0 = MPM(0, j) * filterBank[0].statePost;
			for (int i = 1; i < nModels; ++i) {
				statePost0 = statePost0 + MPM(i, j) * filterBank[i].statePost;
			}
			cv::Mat1d errorCovPost0 = MPM(0, j) * (filterBank[0].errorCovPost + (filterBank[0].statePost - statePost0) * (filterBank[0].statePost - statePost0).t());
			for (int i = 1; i < nModels; ++i) {
				errorCovPost0 = errorCovPost0 + MPM(i, j) * (filterBank[i].errorCovPost + (filterBank[i].statePost - statePost0) * (filterBank[i].statePost - statePost0).t());
			}
			errorCovPost0 = (errorCovPost0 + errorCovPost0.t()) / 2;
			// use this mixed state estimate and covariance as statePost and errorCovPost
			filterBank[j].statePost0 = statePost0.clone();   // clone() was solution to irritating problem
			filterBank[j].errorCovPost0 = errorCovPost0.clone();// clone() was solution to irritating problem
		}
	}
	const cv::Mat1d& IMMEKF::predictState(const cv::Mat1d *F_t, const cv::Mat1d *Q_t)
	{
		for (int j = 0; j < nModels; ++j) {
			filterBank[j].predictState(F_t[j], Q_t[j]);
		}
		// Calculate coefficient to find overall predicted measurement 
		// and innovation covariance matrix
		cv::Mat1d Nu = TPM.t() * Mu;
		statePre = Nu(0) * filterBank[0].statePre;
		for (int j = 1; j < nModels; ++j) {
			statePre = statePre + Nu(j) * filterBank[j].statePre;
		}
#ifdef MAX_DET_IN_IMM
		double curr_det = cv::determinant(filterBank[0].errorCovPre);
		double max_det = curr_det;
		errorCovPre = filterBank[0].errorCovPre;
		for (int j = 1; j < nModels; ++j) {
			curr_det = cv::determinant(filterBank[j].errorCovPre);
			if (max_det < curr_det) {
				max_det = curr_det;
				errorCovPre = filterBank[j].errorCovPre;
			}
		}
#else
		errorCovPre = Nu(0) * (filterBank[0].errorCovPre + (filterBank[0].statePre - statePre) * (filterBank[0].statePre - statePre).t());
		for (int j = 1; j < nModels; ++j) {
			errorCovPre = errorCovPre + Nu(j) * (filterBank[j].errorCovPre + (filterBank[j].statePre - statePre) * (filterBank[j].statePre - statePre).t());
		}
#endif // MAX_DET_IN_IMM
		errorCovPre = (errorCovPre + errorCovPre.t()) / 2;
		return statePre;
	}
	const cv::Mat1d& IMMEKF::predictMeas(const cv::Mat1d &H_t, const cv::Mat1d &R_t)
	{
		for (int j = 0; j < nModels; ++j) {
			filterBank[j].predictMeas(H_t, R_t);
		}
		// Calculate coefficient to find overall predicted measurement 
		// and innovation covariance matrix
		cv::Mat1d Nu = TPM.t() * Mu;
		measPre = Nu(0) * filterBank[0].measPre;
		for (int j = 1; j < nModels; ++j) {
			measPre = measPre + Nu(j) * filterBank[j].measPre;
		}
#ifdef MAX_DET_IN_IMM
		double curr_det = cv::determinant(filterBank[0].S_t);
		double max_det = curr_det;
		S_t = filterBank[0].S_t;
		for (int j = 1; j < nModels; ++j) {
			curr_det = cv::determinant(filterBank[j].S_t);
			if (max_det < curr_det) {
				max_det = curr_det;
				S_t = filterBank[j].S_t;
			}
		}
#else
		S_t = Nu(0) * (filterBank[0].S_t + (filterBank[0].measPre - measPre) * (filterBank[0].measPre - measPre).t());
		for (int j = 1; j < nModels; ++j) {
			S_t = S_t + Nu(j) * (filterBank[j].S_t + (filterBank[j].measPre - measPre) * (filterBank[j].measPre - measPre).t());
		}
#endif // MAX_DET_IN_IMM
		S_t = (S_t + S_t.t()) / 2;
		return measPre;
	}
	const cv::Mat1d& IMMEKF::update(const cv::Mat1d &Y_t, cv::Point *maxLoc_ptr)
	{
		cv::Mat1d meas;
		if (0 != Y_t.cols) {
			cv::Point maxLoc;
			cv::minMaxLoc(Utility::mvnpdf(Y_t, measPre, S_t), NULL, NULL, NULL, &maxLoc);
			if (NULL != maxLoc_ptr) {
				*maxLoc_ptr = maxLoc;
			}
			meas = Y_t.col(maxLoc.y);
		}
		term2 = cv::Mat1d(1, 1, 1);
		// update each Kalman Filter
		cv::Mat1d Likelihood = cv::Mat1d::zeros(nModels, 1); //< Likelihood for filters
		for (int j = 0; j < nModels; ++j) {
			filterBank[j].update(meas);						// update jth Kalman Filter for given measurement
			Likelihood(j) = cv::sum(filterBank[j].term1.mul(term2));
		}
		// update Mode Probabilities
		Mu = cv::Mat1d::diag(Likelihood) * TPM.t() * Mu;
		// L1 Normalize Mu
		Mu = Mu / cv::norm(Mu, cv::NORM_L1); // Note : DO NOT USE cv::normalize function

		this->overallStatePost();
		return statePost;
	}
	const cv::Mat1d& IMMEKF::update(const cv::Mat1d &Y_t, double P_D, double P_G, double lambda)
	{
		assert(0 <= P_D && P_D <= 1);
		assert(0 <= P_G && P_G <= 1);
		int n_y = Y_t.rows;
		int m_t = Y_t.cols;
		if (0 == m_t) {
			term2 = cv::Mat1d(1, 1, 1); // cv::Mat1d(1, 1, 1 - P_D * P_G); // This is causing numerical unstability 
		}
		else {
			double V_t = std::pow(CV_PI, (double)n_y / 2) / MatLab::gamma((double)n_y / 2 + 1) * std::pow(MatLab::chi2inv(1 == P_G ? 1 - 1e-16 : P_G, (double)n_y), (double)n_y / 2) * std::sqrt(cv::determinant(S_t)); //< volume of the validation region
			double Lambda = lambda <= 0 ? m_t / V_t : lambda;
			term2 = cv::Mat1d(m_t + 1, 1, P_D);
			term2(0) = Lambda * (1 - P_D * P_G);
			// term2 *= std::pow(V_t, -m_t + 1) * std::pow(P_D * P_G * m_t + (1 - P_D * P_G) * Lambda * V_t, -1); // This is causing numerical unstability 
		}
		// update each Kalman Filter
		cv::Mat1d Likelihood = cv::Mat1d::zeros(nModels, 1); //< Likelihood for filters
		for (int j = 0; j < nModels; ++j) {
			filterBank[j].update(Y_t, P_D, P_G, lambda); // update jth  Filter for given set measurements
			Likelihood(j) = cv::sum(filterBank[j].term1.mul(term2));
		}
		// update Mode Probabilities
		Mu = cv::Mat1d::diag(Likelihood) * TPM.t() * Mu;
		// L1 Normalize Mu
		Mu = Mu / cv::norm(Mu, cv::NORM_L1); // Note : DO NOT USE cv::normalize function
		this->overallStatePost();
		return statePost;
	}
	void IMMEKF::overallStatePost(void)
	{
		statePost = Mu(0) * filterBank[0].statePost;
		for (int j = 1; j < nModels; ++j) {
			statePost = statePost + Mu(j) * filterBank[j].statePost;
		}
		errorCovPost = Mu(0) * (filterBank[0].errorCovPost + (filterBank[0].statePost - statePost) * (filterBank[0].statePost - statePost).t());
		for (int j = 1; j < nModels; ++j) {
			errorCovPost = errorCovPost + Mu(j) * (filterBank[j].errorCovPost + (filterBank[j].statePost - statePost) * (filterBank[j].statePost - statePost).t());
		}
		errorCovPost = (errorCovPost + errorCovPost.t()) / 2;
	}
}
