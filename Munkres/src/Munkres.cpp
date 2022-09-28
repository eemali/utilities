#include "../include/Munkres.h"
namespace DataAssociation {
	namespace AssignmentAlgorithms {
		Munkres::Munkres(const cv::Mat1d costMat, MUNKRES_MODE Mode) : cMat(costMat), assignment(cv::Mat1i(costMat.rows, 1, -1)), done(false), step(0), istransposed(false)
		{
			if ((0 == costMat.rows) || (0 == costMat.cols)) {
				done = true;
				return;
			}
			if (0 != cv::countNonZero(costMat < 0)) {
				std::cout << "\x1B[33m" << "Warning : costMat must be non-negative matrix" << "\033[0m" << std::endl;
				done = true;
				return;
			}
			dMat = costMat.clone();
			if (MUNKRES_MODE::MAXIMIZE == Mode) {
				double maxVal = 0.0;
				cv::Point maxLoc;
				cv::minMaxLoc(dMat, nullptr, &maxVal, nullptr, &maxLoc);
				dMat = maxVal - dMat;
			}
			else {
				/* NOTHING TO DO */
			}			
		}
		Munkres::~Munkres() {}
		cv::Mat1i Munkres::solve(double* cost)
		{
#ifdef MUNKRES_VERBOSE
			std::cout << "At begining" << std::endl;
			std::cout << "dMat : " << std::endl << dMat << std::endl;
			std::cout << "starMat : " << std::endl << starMat << std::endl;
#endif // MUNKRES_VERBOSE
			while (!done)
			{
#ifdef MUNKRES_VERBOSE
				std::cout << "After step : " << step << std::endl;
#endif // MUNKRES_VERBOSE
				switch (step)
				{
				case 0:
					step_zero(step);
					break;
				case 1:
					step_one(step);
					break;
				case 2:
					step_two(step);
					break;
				case 3:
					step_three(step);
					break;
				case 4:
					step_four(step);
					break;
				case 5:
					step_five(step);
					break;
				case 6:
					step_six(step);
					break;
				case 7:
					step_seven(step);
					break;
				default:
					done = true;
					break;
				}
#ifdef MUNKRES_VERBOSE
				std::cout << "dMat : " << std::endl << dMat << std::endl;
				std::cout << "starMat : " << std::endl << starMat << std::endl;
#endif // MUNKRES_VERBOSE
			}
			if (nullptr != cost) {
				int c;
				*cost = 0;
				for (int r = 0; r < assignment.rows; r++) {
					c = assignment(r);
					*cost += (-1 != c) ? cMat(r, c) : 0;
				}
			}
			return assignment;
		}
		void Munkres::step_zero(int &step)
		{
			if (dMat.cols < dMat.rows) {
				dMat = dMat.t();
				istransposed = true;
			}
			else {
				/* DO NOTHING */
			}
			starMat = cv::Mat1i::zeros(dMat.rows, dMat.cols);
			rowCover = cv::Mat1i::zeros(dMat.rows, 1);
			colCover = cv::Mat1i::zeros(1, dMat.cols);
			step = 1;
		}
		void Munkres::step_one(int &step)
		{
			double min_in_row;
			double dMat_rc;
			for (int r = 0; r < dMat.rows; ++r) {
				min_in_row = dMat(r, 0);
				for (int c = 0; c < dMat.cols; ++c) {
					dMat_rc = dMat(r, c);
					min_in_row = dMat_rc < min_in_row ? dMat_rc : min_in_row;
				}
				for (int c = 0; c < dMat.cols; ++c) {
					dMat(r, c) = dMat(r, c) - min_in_row;
				}
			}
			step = 2;
		}
		void Munkres::step_two(int &step)
		{
			double eps = std::numeric_limits<double>::epsilon();
			for (int r = 0; r < dMat.rows; ++r) {
				for (int c = 0; c < dMat.cols; ++c) {
					if (dMat(r, c) < eps) { // e.g (0 == dMat(r, c))
						if (0 == rowCover(r) && 0 == colCover(c)) {
							starMat(r, c) = 1;
							rowCover(r) = 1;
							colCover(c) = 1;
						}
					}
				}
			}
			rowCover = 0;
			colCover = 0;
			step = 3;
		}
		void Munkres::step_three(int &step)
		{
			int nColCovered = 0;
			for (int c = 0; c < starMat.cols; ++c) {
				for (int r = 0; r < starMat.rows; ++r) {
					if (1 == starMat(r, c)) {
						colCover(c) = 1;
						++nColCovered;
						break;
					}
				}
			}
			if ((nColCovered >= dMat.cols) || (nColCovered >= dMat.rows)) {
				step = 7;
			}
			else {
				step = 4;
			}
		}
		void Munkres::step_four(int &step)
		{
			int row = -1;
			int col = -1;
			bool done = false;
			int c;
			while (!done)
			{
				find_a_zero(row, col);
				if (-1 == row) {
					done = true;
					step = 6;
				}
				else {
					starMat(row, col) = 2;
					c = find_star_in_row(row);
					if (c > -1) {
						col = c;
						rowCover(row) = 1;
						colCover(col) = 0;
					}
					else {
						done = true;
						step = 5;
						last_point = cv::Point(col, row);
					}
				}
			}
		}
		void Munkres::step_five(int &step)
		{
			path.clear();
			path.push_back(last_point);
			int r = -1;
			int c = -1;
			bool done = false;
			while (!done)
			{
				r = find_star_in_col(last_point.x);
				if (r > -1) {
					last_point.y = r;
					path.push_back(last_point);
				}
				else {
					done = true;
				}
				if (!done) {
					c = find_prime_in_row(last_point.y);
					last_point.x = c;
					path.push_back(last_point);
				}
			}
			augment_path();
			clear_covers();
			erase_primes();
			step = 3;
		}
		void Munkres::step_six(int &step)
		{
			double minval = find_smallest();
			if (minval == std::numeric_limits<double>::infinity()) {
				step = 7;
				return;
			}
			for (int r = 0; r < dMat.rows; ++r) {
				for (int c = 0; c < dMat.cols; ++c) {
					if (1 == rowCover(r)) {
						dMat(r, c) += minval;
					}
					if (0 == colCover(c)) {
						dMat(r, c) -= minval;
					}
				}
			}
			step = 4;
		}
		void Munkres::step_seven(int &step)
		{
			if (istransposed) {
				// dMat = dMat.t();
				starMat = starMat.t();
				// rowCover = rowCover.t();
				// colCover = colCover.t();
			}
			else {
				/* DO NOTHING */
			}
			for (int r = 0; r < starMat.rows; ++r) {
				for (int c = 0; c < starMat.cols; ++c) {
					if (1 == starMat(r, c)) {
						assignment(r) = c;
						break;
					}
				}
			}
			step = -1;
		}
		void Munkres::find_a_zero(int &row, int &col)
		{
			row = -1;
			col = -1;
			int r = 0;
			int c;
			bool done = false;
			while (!done)
			{
				c = 0;
				while (true)
				{
					if ((0 == dMat(r, c)) && (0 == rowCover(r)) && (0 == colCover(c)))
					{
						row = r;
						col = c;
						done = true;
					}
					c += 1;
					if ((c >= dMat.cols) || (done))
						break;
				}
				r += 1;
				if (r >= dMat.rows)
					done = true;
			}
		}
		int Munkres::find_star_in_row(int r)
		{
			for (int c = 0; c < starMat.cols; ++c) {
				if (1 == starMat(r, c)) {
					return c;
				}
			}
			return -1;
		}
		int Munkres::find_star_in_col(int c)
		{
			for (int r = 0; r < starMat.rows; ++r) {
				if (1 == starMat(r, c)) {
					return r;
				}
			}
			return -1;
		}
		double Munkres::find_smallest(void)
		{
			double minval = std::numeric_limits<double>::infinity();
			for (int r = 0; r < dMat.rows; ++r) {
				if (0 == rowCover(r)) {
					for (int c = 0; c < dMat.cols; ++c) {
						if (0 == colCover(c)) {
							if (minval > dMat(r, c)) {
								minval = dMat(r, c);
							}
						}
					}
				}
			}
			return minval;
		}
		int Munkres::find_prime_in_row(int r)
		{
			for (int c = 0; c < starMat.cols; ++c) {
				if (2 == starMat(r, c)) {
					return c;
				}
			}
			return -1;
		}
		void Munkres::augment_path(void)
		{
			cv::Point p;
			for (std::list<cv::Point>::const_iterator it = path.begin(); it != path.end(); ++it) {
				p = *it;
				if (1 == starMat(p.y, p.x)) {
					starMat(p.y, p.x) = 0;
				}
				else {
					starMat(p.y, p.x) = 1;
				}
			}
		}
		void Munkres::clear_covers(void) {
			rowCover = 0;
			colCover = 0;
		}
		void Munkres::erase_primes(void) {
			for (int r = 0; r < starMat.rows; ++r) {
				for (int c = 0; c < starMat.cols; ++c) {
					if (2 == starMat(r, c)) {
						starMat(r, c) = 0;
					}
				}
			}
		}
	}
}