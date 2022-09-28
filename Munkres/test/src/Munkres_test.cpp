//#define MUNKRES_VERBOSE
#include "../../include/Munkres.h"
#include "../../../get_time_of_day/include/gettimeofday.h"
#include <limits> // for Infinity
#include <iostream>
#include <string>
void dispMat3(cv::Mat1i &A)
{
	std::cout << A.str() << std::endl;
}
void dispMat(cv::Mat1d &A)
{
	std::cout << A.str() << std::endl;
}
int main(int argc, char** argv)
{
#if 0
	cv::Mat1d costMat(4, 4, temp);
	costMat(0, 0) = 1; costMat(0, 1) = 2; costMat(0, 2) = 3;  costMat(0, 3) = 4;
	costMat(1, 0) = 2; costMat(1, 1) = 4; costMat(1, 2) = 6;  costMat(1, 3) = 8;
	costMat(2, 0) = 3; costMat(2, 1) = 6; costMat(2, 2) = 9;  costMat(2, 3) = 12;
	costMat(3, 0) = 4; costMat(3, 1) = 8; costMat(3, 2) = 12; costMat(3, 3) = 16;
#endif // 0
#if 1
	cv::Mat1d costMat(3, 3);
	costMat(0, 0) = 1; costMat(0, 1) = 2; costMat(0, 2) = 3;
	costMat(1, 0) = 4; costMat(1, 1) = 5; costMat(1, 2) = 6;
	costMat(2, 0) = 7; costMat(2, 1) = 8; costMat(2, 2) = 9;
#endif // 0
#if 0
	cv::Mat1d costMat(2, 1);
	costMat(0) = 48.28193007408159;
	costMat(1) = 40.42833549100408;
#endif // 0
	std::cout << "costMat : " << std::endl << costMat.str() << std::endl;
	DataAssociation::AssignmentAlgorithms::Munkres assnAlgo(costMat, DataAssociation::AssignmentAlgorithms::MINIMIZE);
	double cost;
	struct timeval tic, toc;
	gettimeofday(&tic, NULL);
	cv::Mat1i assignment = assnAlgo.solve(&cost);
	gettimeofday(&toc, NULL);
	double time_taken = (double)(toc.tv_sec - tic.tv_sec) + (double)(toc.tv_usec - tic.tv_usec) * 1e-6;
	std::cout << "Time taken : " << time_taken << std::endl;
	std::cout << "cost : " << cost << std::endl;
	std::cout << "assignment : " << std::endl << assignment.str() << std::endl;
	return 0;
}