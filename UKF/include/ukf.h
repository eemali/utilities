#ifndef UKF_FILTER_H
#define UKF_FILTER_H
#include "../../Mat/include/Mat.h"
namespace Filters
{
	class UKF
	{
	public:		
		//! Default constructor
		UKF();
		//! Copy constructor
		UKF(const UKF &l);
		//! Assignment operator
		UKF& operator=(const UKF &l);
		/*! Initialize the kalman filter for given initial state vector and initial state error covariance matrix.
			@param x0 initial state vector
			@param P0 initial state error covariance matrix
			@return void
		*/		
		void commence(const cv::Mat1d &x0, const cv::Mat1d &P0);
		/*! computes predicated state and covariance as
			statePre = F_t * statePost
			@param F_t state transition matrix
			@param Q_t process noise covariance matrix			
			@return statePre predicated state vector
		*/
		const cv::Mat1d& predictState(const cv::Mat1d &F_t, const cv::Mat1d &Q_t);
		/*! compute predicated measurement & covariance and kalman gain for given
			@param H_t measurement matrix
			@param R_t measurement noise covariance matrix
			@return measPre predicated measurment vector
		*/
		const cv::Mat1d& predictMeas(const cv::Mat1d &H_t, const cv::Mat1d &R_t);
		/*! updates the predicted state using a measurement from given list of validated measurements
			@param measChain list of associated measurements
			@return statePost estimated state vector
		*/
		const cv::Mat1d& update(const cv::Mat1d &Y_t, cv::Point *maxLoc_ptr = NULL);
		
		/*! updates the predicted state, given
		 @param measChain list of associated measurements
		 @param P_D probability of detection \f$ 0 \le P_D \le 1 \f$
		 @param P_G gate probability \f$ 0 \le P_G \le 1 \f$
		 @param lambda \f$ \le 0 \f$ non paramteric PDA filtering. The number of clutter (false measurements) in validation region is uniformly distributed (diffuse prior model).\n
					   \f$ > 0 \f$  parameteric PDA filtering. The number of clutter (false measurements) in validation region is possion distributed with spatial density equal to lambda.
		*/
		const cv::Mat1d& update(const cv::Mat1d &Y_t, double P_D, double P_G, double lambda = 0);
	private:
		/*!
		*/
		void calcUTParameters(int n, double &c, cv::Mat1d &Wmean, cv::Mat1d &Wcov, double &OOM);
		void calcSigmaPoints(const cv::Mat1d &P, const cv::Mat1d &X, double c, cv::Mat1d &X1, cv::Mat1d &X2);
		void UTMeanCov(const cv::Mat1d &Wmean, const cv::Mat1d &Wcov, double OOM,
					   cv::Mat1d Y1, cv::Mat1d Y2,
					   cv::Mat1d X1, cv::Mat1d X2,
					   cv::Mat1d &Ymean, cv::Mat1d &Pyy, cv::Mat1d &Pxy);
		/*! Calculate Innovation Matrix Nu_t \in n_y x (m_t + 1), m_t = Y_t.cols
			# of validated measurements.
			where,				
			Nu_t.col(j) =  0				j = 0
						= Y_t(j) - measPre  1 <= j <= m_t
			@param Y_t : matrix having associated measurements as columns
		*/
		cv::Mat1d innoMat(const cv::Mat1d &Y_t);
		/*! Calculate Association Probabilities, given
			@param Nu_t Innovation Matrix, output of innoMat function
			@param P_D probability of detection \f$ 0 \le P_D \le 1 \f$
			@param P_G gate probability \f$ 0 \le P_G \le 1 \f$
			@param lambda \f$ \le 0 \f$ Non paramteric PDA filtering. The number of clutter (false measurements) in validation region is uniformly distributed (diffuse prior model).\n
					      \f$ > 0 \f$  Parameteric PDA filtering. The number of clutter (false measurements) in validation region is possion distributed with spatial density equal to lambda.			
		 */
		cv::Mat1d assoProb(const cv::Mat1d &Nu_t, double P_D, double P_G, double lambda = 0);
		/*
		*/
		void equatePost0ToPost(void);
	public:
		static double alpha;	// 0 <  Alpha <= 1		(1e-3)
		static double beta;		// 0 <= Beta			(2)
		static double kappa;	// 0 <= Kappa <= 3		(0)
	public:
		cv::Mat1d statePost0;	 //!< intermediate estimated state vector for IMM 
		cv::Mat1d errorCovPost0; //!< intermediate estimation error covariance matrix for IMM
		cv::Mat1d statePre; //!< predicated state vector
		cv::Mat1d errorCovPre; //!< predication error covariance matrix
		cv::Mat1d measPre; //!< predicated measurement vector
		cv::Mat1d S_t; //!< innovation covariance matrix
		cv::Mat1d K_t; //!< kalman gain
		cv::Mat1d statePost; //!< estimated state vector
		cv::Mat1d errorCovPost; //!< estimation error covariance matrix
		cv::Mat1d term1;
		cv::Mat1d term2;
	};
}
#endif // !UKF_FILTER_H

