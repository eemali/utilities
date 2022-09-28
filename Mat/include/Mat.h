#ifndef CV_MAT_H
#define CV_MAT_H
#include <iostream>
#include <fstream>
#ifndef CV_PI
#define CV_PI   3.1415926535897932384626433832795  
#endif // !CV_PI
namespace cv
{
	/*!
		2D point class.
		The class defines a point in 2D space.
	*/
	class Rect2d;
	class Point2i
	{
	public:
		Point2i();
		Point2i(int x_, int y_);
		Point2i(const Point2i &pt);
		Point2i& operator=(const Point2i &pt);
		int x, y;
	};
	typedef Point2i Point;
	class Point2d
	{
	public:
		// various constructors
		Point2d();
		Point2d(double x_, double y_);
		Point2d(const Point2d &pt);

		Point2d& operator=(const Point2d &pt);
		Point2d& operator+=(const Point2d &pt);
		bool operator==(const Point2d &pt);
		bool operator!=(const Point2d &pt);
		//! checks whether the point is inside the specified rectangle
		bool inside(const Rect2d &r) const;
		//! Added by Mali
		std::string str(void) const;
		double x, y; //< the point coordinates
	};
	/*!
		The 2D range class
		This is the class used to specify a continuous subsequence, i.e. part of a contour, or a column span in a matrix.
	*/
	class Range
	{
	public:
		Range();
		Range(int start_, int end_);
		int size() const;
		bool empty() const;
		bool operator==(const Range &r) const;
		bool operator!=(const Range &r) const;
		static Range all();
		int start, end;
	};
	/*!
		The 2D up-right rectangle class
	*/
	class Rect2d
	{
	public:
		//! various constructors
		Rect2d();
		Rect2d(double x_, double y_, double width_, double height_);
		Rect2d(const Rect2d &r);
		Rect2d(const Point2d &pt1, const Point2d &pt2);

		Rect2d& operator=(const Rect2d&r);
		//! the top-left corner
		Point2d tl() const;
		//! the bottom-right corner
		Point2d br() const;
		//! area (width*height) of the rectangle
		double area() const;
		//! checks whether the rectangle contains the point
		bool contains(const Point2d &pt) const;

		double x, y, width, height; //< the top-left corner, as well as width and height of the rectangle
	};
	class Mat1i;
	class Mat1d;
	/** norm types

	src1 and src2 denote input arrays.
	*/
	enum NormTypes 
	{
		/**
		\f[
		norm =  \forkthree
		{\|\texttt{src1}\|_{L_{\infty}} =  \max _I | \texttt{src1} (I)|}{if  \(\texttt{normType} = \texttt{NORM_INF}\) }
		{\|\texttt{src1}-\texttt{src2}\|_{L_{\infty}} =  \max _I | \texttt{src1} (I) -  \texttt{src2} (I)|}{if  \(\texttt{normType} = \texttt{NORM_INF}\) }
		{\frac{\|\texttt{src1}-\texttt{src2}\|_{L_{\infty}}    }{\|\texttt{src2}\|_{L_{\infty}} }}{if  \(\texttt{normType} = \texttt{NORM_RELATIVE | NORM_INF}\) }
		\f]
		*/
		NORM_INF = 1,
		/**
		\f[
		norm =  \forkthree
		{\| \texttt{src1} \| _{L_1} =  \sum _I | \texttt{src1} (I)|}{if  \(\texttt{normType} = \texttt{NORM_L1}\)}
		{ \| \texttt{src1} - \texttt{src2} \| _{L_1} =  \sum _I | \texttt{src1} (I) -  \texttt{src2} (I)|}{if  \(\texttt{normType} = \texttt{NORM_L1}\) }
		{ \frac{\|\texttt{src1}-\texttt{src2}\|_{L_1} }{\|\texttt{src2}\|_{L_1}} }{if  \(\texttt{normType} = \texttt{NORM_RELATIVE | NORM_L1}\) }
		\f]*/
		NORM_L1 = 2,
		/**
		\f[
		norm =  \forkthree
		{ \| \texttt{src1} \| _{L_2} =  \sqrt{\sum_I \texttt{src1}(I)^2} }{if  \(\texttt{normType} = \texttt{NORM_L2}\) }
		{ \| \texttt{src1} - \texttt{src2} \| _{L_2} =  \sqrt{\sum_I (\texttt{src1}(I) - \texttt{src2}(I))^2} }{if  \(\texttt{normType} = \texttt{NORM_L2}\) }
		{ \frac{\|\texttt{src1}-\texttt{src2}\|_{L_2} }{\|\texttt{src2}\|_{L_2}} }{if  \(\texttt{normType} = \texttt{NORM_RELATIVE | NORM_L2}\) }
		\f]
		*/
		NORM_L2 = 4,
		/**
		\f[
		norm =  \forkthree
		{ \| \texttt{src1} \| _{L_2} ^{2} = \sum_I \texttt{src1}(I)^2} {if  \(\texttt{normType} = \texttt{NORM_L2SQR}\)}
		{ \| \texttt{src1} - \texttt{src2} \| _{L_2} ^{2} =  \sum_I (\texttt{src1}(I) - \texttt{src2}(I))^2 }{if  \(\texttt{normType} = \texttt{NORM_L2SQR}\) }
		{ \left(\frac{\|\texttt{src1}-\texttt{src2}\|_{L_2} }{\|\texttt{src2}\|_{L_2}}\right)^2 }{if  \(\texttt{normType} = \texttt{NORM_RELATIVE | NORM_L2SQR}\) }
		\f]
		*/
		NORM_L2SQR = 5,
		/**
		In the case of one input array, calculates the Hamming distance of the array from zero,
		In the case of two input arrays, calculates the Hamming distance between the arrays.
		*/
		NORM_HAMMING = 6,
		/**
		Similar to NORM_HAMMING, but in the calculation, each two bits of the input sequence will
		be added and treated as a single bit to be used in the same calculation as NORM_HAMMING.
		*/
		NORM_HAMMING2 = 7,
		NORM_TYPE_MASK = 7, //!< bit-mask which can be used to separate norm type from norm flags
		NORM_RELATIVE = 8, //!< flag
		NORM_MINMAX = 32 //!< flag
	};
	enum ReduceTypes
	{
		REDUCE_SUM = 0, //!< the output is the sum of all rows/columns of the matrix.
		REDUCE_AVG = 1, //!< the output is the mean vector of all rows/columns of the matrix.
		REDUCE_MAX = 2, //!< the output is the maximum (column/row-wise) of all rows/columns of the matrix.
		REDUCE_MIN = 3  //!< the output is the minimum (column/row-wise) of all rows/columns of the matrix.
	};
	class MatRef {
	public:
		friend class Mat1i;
		friend class Mat1d;
	private:
		/*volatile*/ int count;
	};
	class Mat1i
	{
	public:
		//! default constructor
		Mat1i();
		//! equivalent to Mat1i(rows, cols)
		Mat1i(int rows_, int cols_);
		//! constructor that sets each matrix element to specified value c
		Mat1i(int rows_, int cols_, int c);
		//!
		Mat1i(int rows_, int cols_, int* data_);
		//! copy constructor
		Mat1i(const Mat1i &m);
		//! decreases reference counter;
		// deallocates the data when reference counter reaches 0.
		void release(void);
		Mat1i& operator=(const Mat1i &m);

		~Mat1i();
		//! returns deep copy of the matrix, i.e. the data is copied
		Mat1i clone(void) const;

		//! element access
		const int& operator()(int i, int j) const;
		int& operator()(int i, int j);

		//! 1D element access
		const int& operator()(int i) const;
		int& operator()(int i);

		//! extract the matrix row
		Mat1i row(int i) const;
		//! extract the matrix column
		Mat1i col(int j) const;
		//! transpose the matrix
		Mat1i t(void) const;
		//! ... for the specified diagonal
		// (d = 0 - the main diagonal,
		//    > 0 - a diagonal from the lower half,
		//    < 0 - a diagonal from the upper half)
		Mat1i diag(int d = 0) const;

		//! sets every matrix element to value c
		Mat1i& operator=(int c);

		friend int countNonZero(const Mat1i &a);
		friend class Mat1d;
		friend Mat1d inverse(const Mat1d &A);
		friend double determinant(const Mat1d &M);
		void write(std::ofstream &fp, std::string ls = std::string()) const;
		void read(std::ifstream &fp);

		std::string str(void) const;
		std::string str2(void) const;
		std::string ostr(void) const;
		//! Matlab-style matrix initialization
		static Mat1i zeros(int rows_, int cols_);
		static Mat1i ones(int rows_, int cols_);
		static Mat1i eye(int rows_, int cols_);
		//! constructs a square diagonal matrix which main diagonal is vector "d"
		static Mat1i diag(const Mat1i &d);

		//! the number of rows and columns
		int rows, cols;
	private:
		//! pointer to the data
		int* data;
		//! pointer to the reference counter;
		MatRef* ref;
	};
	class Mat1d
	{
	public:
		//! default constructor
		Mat1d();
		//! equivalent to Mat1d(rows, cols)
		Mat1d(int rows_, int cols_);
		//! constructor that sets each matrix element to specified value c
		Mat1d(int rows_, int cols_, double c);
		//!
		Mat1d(int rows_, int cols_, double* data_);
		//! copy constructor
		Mat1d(const Mat1d &m);
		//! decreases reference counter;
		// deallocates the data when reference counter reaches 0.
		void release(void);
		Mat1d& operator=(const Mat1d &m);

		~Mat1d();
		//! returns deep copy of the matrix, i.e. the data is copied
		Mat1d clone(void) const;

		//! element access
		const double& operator()(int i, int j) const;
		double& operator()(int i, int j);

		//! 1D element access
		const double& operator()(int i) const;
		double& operator()(int i);

		Mat1d operator()(Range rowRange, Range colRange) const;

		//! extract the matrix row
		Mat1d row(int i) const;
		//! extract the matrix column
		Mat1d col(int j) const;
		//! transpose the matrix
		Mat1d t(void) const;
		//! ... for the specified diagonal
		// (d = 0 - the main diagonal,
		//    > 0 - a diagonal from the lower half,
		//    < 0 - a diagonal from the upper half)
		Mat1d diag(int d = 0) const;
		//! computes trace of a matrix
		double trace(void) const;
		//! compute determinant of a matrix 
		double det(void) const;
		//! matrix inversion
		Mat1d inv(void) const;

		//! add two matrices element-wise
		Mat1d operator+(const Mat1d &b) const;
		//! subtract two matrices element-wise
		Mat1d operator-(const Mat1d &b) const;
		//! multiply two matrices element-wise
		Mat1d mul(const Mat1d &b) const;
		//! divide two matrices element-wise
		Mat1d operator/(const Mat1d &b) const;
		//! matrix multiplication
		Mat1d operator*(const Mat1d &b) const;
		//! matrix nagation
		Mat1d operator-(void) const;
		//! add scalar
		Mat1d operator+(double c) const;
		friend Mat1d operator+(double c, const Mat1d &b);
		//! substract scalar
		Mat1d operator-(double c) const;
		friend Mat1d operator-(double c, const Mat1d &b);
		//! multiply by scalar
		Mat1d operator*(double c) const;
		friend Mat1d operator*(double c, const Mat1d &b);
		//! divide by scalar 
		Mat1d operator/(double c) const;
		friend Mat1d operator/(double c, const Mat1d &b);
		//! sets every matrix element to value c
		Mat1d& operator=(double c);

		Mat1i operator<(double c) const;
		Mat1i operator!=(const Mat1d &b) const;

		void setTo(int r, int c, const Mat1d &a);
		friend Mat1d exp(const Mat1d &a);
		friend Mat1d pow(const Mat1d &a, double power);
		friend Mat1d abs(const Mat1d &a);
		friend double sum(const Mat1d &a);
		friend Mat1d max(const Mat1d &a, double b);
		friend Mat1d min(const Mat1d &a, double b);
		friend Mat1d repeat(const Mat1d &a, int ny, int nx);
		friend int countNonZero(const Mat1d &a);
		friend void minMaxLoc(const Mat1d &a, double* minVal, double* maxVal, Point2i* minLoc, Point2i* maxLoc);
		friend double norm(const Mat1d &a, int normType);
		friend Mat1d inverse(const Mat1d &A);
		friend double determinant(const Mat1d &M);
		friend Mat1d chol(const Mat1d &A);
		void write(std::ofstream &fp, std::string ls = std::string()) const;
		void read(std::ifstream &fp);

		std::string str(void) const;
		std::string str2(void) const;
		std::string str3(void) const;
		std::string str4(void) const;
		std::string ostr(void) const;
		void disp(void) const;
		//! Matlab-style matrix initialization
		static Mat1d zeros(int rows_, int cols_);
		static Mat1d ones(int rows_, int cols_);
		static Mat1d eye(int rows_, int cols_);
		//! constructs a square diagonal matrix which main diagonal is vector "d"
		static Mat1d diag(const Mat1d &d);
		//! the number of rows and columns
		int rows, cols;
	private:
		//! pointer to the data
		double* data;
		//! pointer to the reference counter;
		MatRef* ref;
		Mat1d scalarOp(char op, double c) const;
		Mat1d arrayOp(char op, const Mat1d &b) const;
	};
	Mat1d operator+(double c, const Mat1d &b);
	Mat1d operator-(double c, const Mat1d &b);
	Mat1d operator*(double c, const Mat1d &b);
	Mat1d operator/(double c, const Mat1d &b);
	Mat1d exp(const Mat1d &a);
	Mat1d pow(const Mat1d &a, double power);
	Mat1d abs(const Mat1d &a);
	Mat1d max(const Mat1d &a, double b);
	Mat1d min(const Mat1d &a, double b);
	double sum(const Mat1d &a);
	Mat1d repeat(const Mat1d &a, int ny, int nx);
	int countNonZero(const Mat1i &a);
	int countNonZero(const Mat1d &a);
	void minMaxLoc(const Mat1d &a, double* minVal, double* maxVal = nullptr, Point2i* minLoc = nullptr, Point2i* maxLoc = nullptr);
	double norm(const Mat1d &a, int normType = NORM_L2);
	Mat1d inverse(const Mat1d &A);
	double determinant(const Mat1d &M);
	Mat1d chol(const Mat1d &A);
}
#endif // !CV_MAT_H
