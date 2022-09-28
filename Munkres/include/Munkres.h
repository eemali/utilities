#ifndef MUNKRES_H
#define MUNKRES_H
#include "../../Mat/include/Mat.h"
#include <list>
#include <limits> // for Infinity
namespace DataAssociation 
{
	namespace AssignmentAlgorithms 
	{
		enum MUNKRES_MODE
		{
			MINIMIZE,
			MAXIMIZE
		};
		class Munkres
		{
		public:
			Munkres(const cv::Mat1d costMat, MUNKRES_MODE Mode);
			cv::Mat1i solve(double* cost = NULL);
		private:
			/*!
				Rotate the dMat so that there are at least as many columns as rows => rows <= cols
			*/
			void step_zero(int &step);
			/*!
				For each row of the cost matrix, find the smallest element and subtract
				it from every element in its row.  When finished, Go to Step 2.
			*/
			void step_one(int &step);
			/*!
				Find a zero (Z) in the resulting matrix.  If there is no starred 
				zero in its row or column, star Z. Repeat for each element in the 
				matrix. Go to Step 3.
			*/
			void step_two(int &step);
			/*!
				Cover each column containing a starred zero.  If K columns are covered, 
				the starred zeros describe a complete set of unique assignments.  In this
				case, Go to DONE, otherwise, Go to Step 4.
			*/
			void step_three(int &step);
			/* methods to support step 4 */
			void find_a_zero(int &row, int &col);
			int find_star_in_row(int r);
			/*!
				Find a noncovered zero and prime it.  If there is no starred zero 
				in the row containing this primed zero, Go to Step 5.  Otherwise, 
				cover this row and uncover the column containing the starred zero.
				Continue in this manner until there are no uncovered zeros left. 
				Save the smallest uncovered value and Go to Step 6.
			*/
			void step_four(int &step);
			/* methods to support step 5 */
			int find_star_in_col(int c);
			int find_prime_in_row(int r);
			void augment_path(void);
			void clear_covers(void);
			void erase_primes(void);
			/*!
				Construct a series of alternating primed and starred zeros as follows.  
				Let Z0 represent the uncovered primed zero found in Step 4.  Let Z1 denote 
				the starred zero in the column of Z0 (if any). Let Z2 denote the primed zero
				in the row of Z1 (there will always be one).  Continue until the series 
				terminates at a primed zero that has no starred zero in its column.  
				Unstar each starred zero of the series, star each primed zero of the series,
				erase all primes and uncover every line in the matrix.  Return to Step 3.
			*/
			void step_five(int &step);
			/* methods to support step 6 */
			double find_smallest(void);
			/*!
				Add the value found in Step 4 to every element of each covered row, and subtract
				it from every element of each uncovered column.  Return to Step 4 without 
				altering any stars, primes, or covered lines.
			*/
			void step_six(int &step);

			void step_seven(int &step);

			cv::Mat1d cMat;
			cv::Mat1d dMat;
			cv::Mat1i starMat;
			cv::Mat1i rowCover;
			cv::Mat1i colCover;
			std::list<cv::Point> path;
			cv::Point last_point;
			cv::Mat1i assignment;
			bool done;
			int step;
			bool istransposed;
		};
	}
}
#endif // !MUNKRES_H

