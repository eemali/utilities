#include "../../include/Mat.h"
#include <iostream>
#include <string>
int main(int argc, char** argv) 
{
	cv::Mat1d A;
	std::cout << "A:" << std::endl << A.str4() << std::endl << std::endl;

	cv::Mat1d B(2, 3);
	std::cout << "B:" << std::endl << B.str4() << std::endl << std::endl;

	cv::Mat1d C(2, 3, -0.5);
	std::cout << "C:" << std::endl << C.str4() << std::endl << std::endl;

	cv::Mat1d O = cv::Mat1d::ones(2, 3);
	std::cout << "O:" << std::endl << O.str4() << std::endl << std::endl;

	cv::Mat1d Z = cv::Mat1d::zeros(3, 2);
	std::cout << "Z:" << std::endl << Z.str4() << std::endl << std::endl;

	cv::Mat1d I1 = cv::Mat1d::eye(3, 3);
	std::cout << "I1:" << std::endl << I1.str4() << std::endl << std::endl;

	cv::Mat1d I2 = cv::Mat1d::eye(2, 3);
	std::cout << "I2:" << std::endl << I2.str4() << std::endl << std::endl;

	cv::Mat1d D(C);
	D(0, 0) = -1;
	std::cout << "D:" << std::endl << D.str4() << std::endl << std::endl;
	std::cout << "C:" << std::endl << C.str4() << std::endl << std::endl;

	cv::Mat1d E = C.clone();
	E(0, 1) = 1;
	std::cout << "E:" << std::endl << E.str4() << std::endl << std::endl;
	std::cout << "C:" << std::endl << C.str4() << std::endl << std::endl;

	cv::Mat1d exp_I1 = cv::exp(I1);
	std::cout << "exp_I1:" << std::endl << exp_I1.str4() << std::endl << std::endl;


	cv::Mat1d H(3, 3);
	H(0, 0) = 1; H(1, 0) = 0; H(2, 0) = 1;
	H(0, 1) = 0; H(1, 1) = 2; H(2, 1) = 0;
	H(0, 2) = 1; H(1, 2) = 0; H(2, 2) = 3;

	cv::Mat1d H_chol = cv::chol(H);
	cv::Mat1d H_hat = H_chol * H_chol.t();
	std::cout << "H:" << std::endl << H.str4() << std::endl << std::endl;
	std::cout << "H_col:" << std::endl << H_chol.str4() << std::endl << std::endl;
	std::cout << "H_hat:" << std::endl << H_hat.str4() << std::endl << std::endl;
	return 0;
}