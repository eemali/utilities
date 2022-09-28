#ifndef IMMEKF_FILTER_H
#define IMMEKF_FILTER_H
#include "ekf.h"
namespace Filters
{
	class IMMEKF
	{
	public:
		//! Default constructor
		IMMEKF();
		//! Destructor
		~IMMEKF();
		//! Copy constructor
		IMMEKF(const IMMEKF &l);
		//! Assignment operator
		IMMEKF& operator=(const IMMEKF &l);
		//!
		void commence(const cv::Mat1d &x0, const cv::Mat1d &P0);
		//!
		void mixing(void);
		//! For each model computes predicated state and covariance for given
		//! F_t : state transition matrix
		//! Q_t : process noise covariance matrix
		const cv::Mat1d& predictState(const cv::Mat1d *F_t, const cv::Mat1d *Q_t);
		//! For each model compute predicated measurement & covariance and Kalman gain also for given
		//! H_t : measurement matrix
		//! R_t : measurement noise covariance matrix
		const cv::Mat1d& predictMeas(const cv::Mat1d &H_t, const cv::Mat1d &R_t);
		//! For each model updates the predicted state and also update mode probabilities 
		//! using a measurement from given list of validated measurements
		const cv::Mat1d& update(const cv::Mat1d &Y_t, cv::Point *maxLoc_ptr = NULL);
		//! updates the predicted state, given
		//! measChain : List of validated measurements
		//! P_D : detection probability (0 <= P_D && P_D <= 1)
		//! P_G : gate probability (0 <= P_G && P_G <= 1)
		//! lambda : spatial density parameter for possion pmf model
		//! lambda <= 0 ==> Non Parameteric PDAF
		//! lambda > 0 ==>     Parameteric PDAF
		//! By default lambda = 0
		const cv::Mat1d& update(const cv::Mat1d &Y_t, double P_D, double P_G, double lambda = 0);
		
		static const int nModels;
		static const cv::Mat1d Mu0; //!< Initial Mode Probability vector
		static const cv::Mat1d TPM; //!< Transition Probability Matrix		
		cv::Mat1d Mu; //!< Mode Probability vector
		EKF *filterBank; //!< Filter Bank
		cv::Mat1d statePre; //!< predicated state vector
		cv::Mat1d errorCovPre; //!< predication error covariance matrix
		cv::Mat1d measPre; //!< predicated measurement vector
		cv::Mat1d S_t; //!< innovation covariance matrix
		cv::Mat1d statePost; //!< estimated state vector
		cv::Mat1d errorCovPost; //!< estimation error covariance matrix
		cv::Mat1d term2;
	private:
		//! Calculate the overall estimate and covariance
		void overallStatePost(void);
	};
}
#endif // !IMMEKF_FILTER_H
