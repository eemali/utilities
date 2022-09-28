#include "../include/Mat.h"
#include <cassert>   // for assert
#include <cstring>   // for std::memcpy
#include <iostream>  // for std::memcpy
#include <sstream>   // for std::ostringstream
#include <iomanip>   // for std::steprecision
#include <algorithm> // for std::min
#include <stdexcept> // for std::runtime_error
#include <cmath>
#include <climits>
#include <omp.h>
namespace cv
{
	Point2i::Point2i() : x(0), y(0) {}
	Point2i::Point2i(int x_, int y_) : x(x_), y(y_) {}
	Point2i::Point2i(const Point2i &pt) : x(pt.x), y(pt.y) {}
	Point2i& Point2i::operator=(const Point2i &pt) { x = pt.x; y = pt.y; return *this; }
	/*=======================================================================================================*/
	Point2d::Point2d() : x(0), y(0) {}
	Point2d::Point2d(double x_, double y_) : x(x_), y(y_) {}
	Point2d::Point2d(const Point2d &pt) : x(pt.x), y(pt.y) {}
	Point2d& Point2d::operator=(const Point2d &pt) { x = pt.x; y = pt.y; return *this; }
	Point2d& Point2d::operator+=(const Point2d &pt)
	{
		x = x + pt.x;
		y = y + pt.y;
		return *this;
	}
	bool Point2d::operator==(const Point2d &pt) { return (x == pt.x) && (y == pt.y); }
	bool Point2d::operator!=(const Point2d & pt) { return (x != pt.x) || (y != pt.y); }
	bool Point2d::inside(const Rect2d &r) const { return r.contains(*this); }
	std::string Point2d::str(void) const
	{
		std::ostringstream out; out << std::fixed << std::setprecision(16);
		out << "(" << x << ", " << y << ")";
		return out.str();
	}
	/*=======================================================================================================*/
	Range::Range() : start(0), end(0) {}
	Range::Range(int start_, int end_) : start(start_), end(end_) {}
	int Range::size() const { return end - start; }
	bool Range::empty() const { return start == end; }
	bool Range::operator==(const Range &r) const { return (start == r.start) && (end == r.end); }
	bool Range::operator!=(const Range &r) const { return !(*this == r); }
	Range Range::all() { return Range(INT_MIN, INT_MAX); }
	/*=======================================================================================================*/
	Rect2d::Rect2d() : x(0), y(0), width(0), height(0) {}
	Rect2d::Rect2d(double x_, double y_, double width_, double height_)
		: x(x_), y(y_), width(width_), height(height_) {}
	Rect2d::Rect2d(const Rect2d &r) : x(r.x), y(r.y), width(r.width), height(r.height) {}
	Rect2d::Rect2d(const Point2d &pt1, const Point2d &pt2)
	{
		x = std::min(pt1.x, pt2.x); y = std::min(pt1.y, pt2.y);
		width = std::max(pt1.x, pt2.x) - x; height = std::max(pt1.y, pt2.y) - y;
	}
	Rect2d& Rect2d::operator=(const Rect2d &r)
	{
		x = r.x; y = r.y; width = r.width; height = r.height; return *this;
	}
	Point2d Rect2d::tl() const { return Point2d(x, y); }
	Point2d Rect2d::br() const { return Point2d(x + width, y + height); }
	double Rect2d::area() const { return width * height; }
	bool Rect2d::contains(const Point2d &pt) const
	{
		return (x <= pt.x) && (pt.x < x + width) && (y <= pt.y) && (pt.y < y + height);
	}
	/*=======================================================================================================*/
    Mat1i::Mat1i() : rows(0), cols(0), data(nullptr), ref(nullptr) {}
    Mat1i::Mat1i(int rows_, int cols_) : rows(rows_), cols(cols_), data(nullptr), ref(nullptr)
    {
        assert((rows >= 0) && (cols >= 0));
        int len = rows * cols;
        if (len > 0) {
            data = new int[int(len)];
            assert(nullptr != data);
            ref = new MatRef;
            assert(nullptr != ref);
            ref->count = 1;
        }
    }
    Mat1i::Mat1i(int rows_, int cols_, int c) : rows(rows_), cols(cols_), data(nullptr), ref(nullptr)
    {
        assert((rows >= 0) && (cols >= 0));
        int len = rows * cols;
        if (len > 0) {
            data = new int[int(len)];
            assert(nullptr != data);
            for (int i = 0; i < len; ++i) {
                data[int(i)] = c;
            }
            ref = new MatRef;
            assert(nullptr != ref);
            ref->count = 1;
        }
    }
	Mat1i::Mat1i(int rows_, int cols_, int * data_) : rows(rows_), cols(cols_), data(nullptr), ref(nullptr)
	{
		assert(nullptr != data_);
		assert((rows >= 0) && (cols >= 0));
		int len = rows * cols;
		if (len > 0) {
			data = new int[int(len)];
			assert(nullptr != data);
			for (int i = 0; i < len; ++i) {
				data[int(i)] = data_[int(i)];
			}
			ref = new MatRef;
			assert(nullptr != ref);
			ref->count = 1;
		}
	}
    Mat1i::Mat1i(const Mat1i &m) : rows(m.rows), cols(m.cols), data(m.data), ref(m.ref)
    {
        if (nullptr != ref) {
#pragma omp atomic
            ref->count++;
        }
    }
    void Mat1i::release(void)
    {
        if (nullptr != ref) {
#pragma omp atomic
            ref->count--;
            if (0 == ref->count) {
                if (nullptr != data) {
                    delete[] data;
                }
                delete[] ref;
            }
        }
        rows = 0; cols = 0; data = nullptr; ref = nullptr;
    }
    Mat1i& Mat1i::operator=(const Mat1i &m)
    {
        release();
        rows = m.rows; cols = m.cols; data = m.data; ref = m.ref;
        if (nullptr != ref) {
#pragma omp atomic
            ref->count++;
        }
        return *this;
    }
    Mat1i::~Mat1i()
    {
        release();
    }
    Mat1i Mat1i::clone(void) const
    {
        size_t len = size_t(rows * cols) * sizeof(int);
        Mat1i m(rows, cols);
        std::memcpy(m.data, data, len);
        return m;
    }
    const int& Mat1i::operator()(int i, int j) const
    {
        assert(((unsigned)i < (unsigned)rows) && ((unsigned)j < (unsigned)cols));
        assert(nullptr != data);
        return data[int(i * cols + j)];
    }
    int& Mat1i::operator()(int i, int j)
    {
        assert(((unsigned)i < (unsigned)rows) && ((unsigned)j < (unsigned)cols));
        assert(nullptr != data);
        return data[int(i * cols + j)];
    }
    const int& Mat1i::operator()(int i) const
    {
        assert(((1 == rows) || (1 == cols)) && ((unsigned)i < (unsigned)(rows + cols - 1)));
        assert(nullptr != data);
        return data[int(i)];
    }
    int& Mat1i::operator()(int i)
    {
        assert(((1 == rows) || (1 == cols)) && ((unsigned)i < (unsigned)(rows + cols - 1)));
        assert(nullptr != data);
        return data[int(i)];
    }
    Mat1i Mat1i::row(int i) const
    {
        Mat1i m(1, cols);
        int len = cols * int(sizeof(int));
        if (len > 0) {
            assert(nullptr != data);
            /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
            assert(nullptr != m.data);
            std::memcpy(m.data, &data[int(i * cols)], len);
        }
        return m;
    }
    Mat1i Mat1i::col(int j) const
    {
        Mat1i m(rows, 1);
        if (rows > 0) {
            assert(nullptr != data);
            /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
            assert(nullptr != m.data);
            for (int i = 0; i < rows; ++i) {
                m.data[int(i)] = data[int(i * cols + j)];
            }
        }
        return m;
    }
    Mat1i Mat1i::t(void) const
    {
        Mat1i m(cols, rows);
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            for (int i = 0; i < m.rows; ++i) {
                for (int j = 0; j < m.cols; ++j) {
                    m.data[int(i * m.cols + j)] = data[int(j * cols + i)];
                }
            }
        }
        return m;
    }
    Mat1i Mat1i::diag(int d) const
    {
        int len = (d >= 0) ? std::min(rows, cols - d) : std::min(rows + d, cols);
        Mat1i m(len, 1);
        if (len > 0) {
            assert(nullptr != data);
            for (int i = 0; i < len; ++i) {
                if (d >= 0) {
                    m.data[int(i)] = data[int(i * cols + (i + d))];
                }
                else {
                    m.data[int(i)] = data[int((i - d) * cols + i)];
                }
            }
        }
        return m;
    }
    Mat1i& Mat1i::operator=(int c)
    {
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            for (int k = 0; k < len; ++k) {
                data[int(k)] = c;
            }
        }
        return *this;
    }
    std::string Mat1i::str(void) const
    {
        std::ostringstream out; out << std::fixed << std::setprecision(16);
        out << "[ ";
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    out << data[int(i * cols + j)];
                    if (j == cols - 1) {
                        if (i == rows - 1) {
                            /* DO NOTHING */
                        }
                        else {
                            out << "; ";
                        }
                    }
                    else {
                        out << " ";
                    }
                }
            }
        }
        out << " ]";
        return out.str();
    }
    std::string Mat1i::str2(void) const
    {
        std::ostringstream out; out << std::fixed << std::setprecision(16);
        out << "[ ";
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    out << data[int(i * cols + j)];
                    if (j == cols - 1) {
                        if (i == rows - 1) { /* DO NOTHING */ }
                        else { out << "\n "; }
                    }
                    else {
                        out << " ";
                    }
                }
            }
        }
        out << " ]";
        return out.str();
    }
    std::string Mat1i::ostr(void) const
    {
        std::ostringstream out; out << std::fixed << std::setprecision(16);
        out << "[";
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            for (int k = 0; k < len; ++k) {
                out << " " << data[int(k)];
                if (k == len - 1) { out << " "; }
                else { out << ","; }
            }
        }
        out << "]";
        return out.str();
    }
    Mat1i Mat1i::zeros(int rows_, int cols_)
    {
        Mat1i m(rows_, cols_, 0);
        return m;
    }
    Mat1i Mat1i::ones(int rows_, int cols_)
    {
        Mat1i m(rows_, cols_, 1);
        return m;
    }
    Mat1i Mat1i::eye(int rows_, int cols_)
    {
        Mat1i m(rows_, cols_, 0);
        int d = (m.rows < m.cols) ? m.rows : m.cols;
        int len = m.rows * m.cols;
        if (len > 0) {
            for (int i = 0; i < d; ++i) {
                m.data[int(i * m.cols + i)] = 1;
            }
        }
        return m;
    }
    Mat1i Mat1i::diag(const Mat1i &d)
    {
        assert((1 == d.rows) || (1 == d.cols));
        int len = d.rows + d.cols - 1;
        Mat1i m(len, len, 0);
        if (len > 0) {
            for (int i = 0; i < len; ++i) {
                m.data[int(i * m.cols + i)] = d.data[int(i)];
            }
        }
        return m;
    }
	void Mat1i::write(std::ofstream &fp, std::string prefix) const
	{
		fp << std::fixed << std::setprecision(16);
		fp << prefix << "rows: " << rows << std::endl
		   << prefix << "cols: " << cols << std::endl
		   << prefix << "dt: " << "i" << std::endl
		   << prefix << "data: " << this->ostr() << std::endl;
	}
    void Mat1i::read(std::ifstream &fp)
    {
        std::string str, dt;
        fp >> str >> rows >> str >> cols >> str >> dt >> str;
        *this = Mat1i(rows, cols);
        int len = rows * cols;
        for (int k = 0; k < len; ++k) {
            fp >> str >> data[int(k)];
        }
        fp >> str;
    }
	/*=======================================================================================================*/
    Mat1d::Mat1d() : rows(0), cols(0), data(nullptr), ref(nullptr) {}
    Mat1d::Mat1d(int rows_, int cols_) : rows(rows_), cols(cols_), data(nullptr), ref(nullptr)
    {
        assert((rows >= 0) && (cols >= 0));
        int len = rows * cols;
        if (len > 0) {
            data = new double[int(len)];
            assert(nullptr != data);
            ref = new MatRef;
            assert(nullptr != ref);
            ref->count = 1;
        }
    }
    Mat1d::Mat1d(int rows_, int cols_, double c) : rows(rows_), cols(cols_), data(nullptr), ref(nullptr)
    {
        assert((rows >= 0) && (cols >= 0));
        int len = rows * cols;
        if (len > 0) {
            data = new double[int(len)];
            assert(nullptr != data);
            for (int i = 0; i < len; ++i) {
                data[int(i)] = c;
            }
            ref = new MatRef;
            assert(nullptr != ref);
            ref->count = 1;
        }
    }
	Mat1d::Mat1d(int rows_, int cols_, double* data_) : rows(rows_), cols(cols_), data(nullptr), ref(nullptr)
	{
		assert(nullptr != data_);
		assert((rows >= 0) && (cols >= 0));
		int len = rows * cols;
		if (len > 0) {
			data = new double[int(len)];
			assert(nullptr != data);
			for (int i = 0; i < len; ++i) {
				data[int(i)] = data_[int(i)];
			}
			ref = new MatRef;
			assert(nullptr != ref);
			ref->count = 1;
		}
	}
    Mat1d::Mat1d(const Mat1d &m) : rows(m.rows), cols(m.cols), data(m.data), ref(m.ref)
    {
        if (nullptr != ref) {
#pragma omp atomic
            ref->count++;
        }
    }
    void Mat1d::release(void)
    {
        if (nullptr != ref) {
#pragma omp atomic
            ref->count--;
            if (0 == ref->count) {
                if (nullptr != data) {
                    delete[] data;
                }
                delete[] ref;
            }
        }
        rows = 0; cols = 0; data = nullptr; ref = nullptr;
    }
    Mat1d& Mat1d::operator=(const Mat1d &m)
    {
        release();
        rows = m.rows; cols = m.cols; data = m.data; ref = m.ref;
        if (nullptr != ref) {
#pragma omp atomic
            ref->count++;
        }
        return *this;
    }
    Mat1d::~Mat1d()
    {
        release();
    }
    Mat1d Mat1d::clone(void) const
    {
        size_t len = size_t(rows * cols) * sizeof(double);
        Mat1d m(rows, cols);
        std::memcpy(m.data, data, len);
        return m;
    }
    const double& Mat1d::operator()(int i, int j) const
    {
        assert(((unsigned)i < (unsigned)rows) && ((unsigned)j < (unsigned)cols));
        assert(nullptr != data);
        return data[int(i * cols + j)];
    }
    double& Mat1d::operator()(int i, int j)
    {
        assert(((unsigned)i < (unsigned)rows) && ((unsigned)j < (unsigned)cols));
        assert(nullptr != data);
        return data[int(i * cols + j)];
    }
    const double& Mat1d::operator()(int i) const
    {
        assert(((1 == rows) || (1 == cols)) && ((unsigned)i < (unsigned)(rows + cols - 1)));
        assert(nullptr != data);
        return data[int(i)];
    }
    double& Mat1d::operator()(int i)
    {
        assert(((1 == rows) || (1 == cols)) && ((unsigned)i < (unsigned)(rows + cols - 1)));
        assert(nullptr != data);
        return data[int(i)];
    }
    Mat1d Mat1d::operator()(Range rowRange, Range colRange) const
    {
        if (rowRange != Range::all()) {
            assert((0 <= rowRange.start) && (rowRange.start <= rowRange.end) && (rowRange.end <= rows));
        }
        else {
            rowRange = Range(0, rows);
        }
        if (colRange != Range::all()) {
            assert((0 <= colRange.start) && (colRange.start <= colRange.end) && (colRange.end <= cols));
        }
        else {
            colRange = Range(0, cols);
        }
        Mat1d m(rowRange.size(), colRange.size());
        size_t len = size_t(m.cols) * sizeof(double);
        double *src_ptr = nullptr;
        double *dst_ptr = nullptr;
        for (int i = 0; i < m.rows; ++i) {
            src_ptr = &data[int(rowRange.start + i * cols + colRange.start)];
            dst_ptr = &m.data[int(i * m.cols)]; // CAVEAT: missing m in m.cols was corrupting heap
            std::memcpy(dst_ptr, src_ptr, len);
        }
        return m;
    }
    Mat1d Mat1d::row(int i) const
    {
        Mat1d m(1, cols);
        int len = cols * int(sizeof(double));
        if (len > 0) {
            assert(nullptr != data);
            /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation <justification end> */
            assert(nullptr != m.data);
            std::memcpy(m.data, &data[int(i * cols)], len);
        }
        return m;
    }
    Mat1d Mat1d::col(int j) const
    {
        Mat1d m(rows, 1);
        if (rows > 0) {
            assert(nullptr != data);
            for (int i = 0; i < rows; ++i) {
                m.data[int(i)] = data[int(i * cols + j)];
            }
        }
        return m;
    }
    Mat1d Mat1d::t(void) const
    {
        Mat1d m(cols, rows);
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            for (int i = 0; i < m.rows; ++i) {
                for (int j = 0; j < m.cols; ++j) {
                    m.data[int(i * m.cols + j)] = data[int(j * cols + i)];
                }
            }
        }
        return m;
    }
    Mat1d Mat1d::diag(int d) const
    {
        int len = (d >= 0) ? std::min(rows, cols - d) : std::min(rows + d, cols);
        Mat1d m(len, 1);
        if (len > 0) {
            assert(nullptr != data);
            for (int i = 0; i < len; ++i) {
                if (d >= 0) {
                    m.data[int(i)] = data[int(i * cols + (i + d))];
                }
                else {
                    m.data[int(i)] = data[int((i - d) * cols + i)];
                }
            }
        }
        return m;
    }
    double Mat1d::trace(void) const
    {
        int len = (rows < cols) ? rows : cols;
        double s = 0.0;
        if (len > 0) {
            assert(nullptr != data);
            for (int i = 0; i < len; ++i) {
                s = s + data[int(i * cols + i)];
            }
        }
        return s;
    }
    double Mat1d::det(void) const
    {
        assert(rows == cols);
#if 0
        double d;
        switch (rows)
        {
        case 1:
            d = data[0];
            break;
        case 2:
            d = data[0] * data[3] - data[1] * data[2];
            break;
        case 3:
            d = data[0] * (data[4] * data[8] - data[5] * data[7]) -
                data[1] * (data[3] * data[8] - data[5] * data[6]) +
                data[2] * (data[3] * data[7] - data[4] * data[6]);
            break;
        default:
			{
					std::ostringstream errorMsg;
					errorMsg << "EXCEPTION OCCURED AT" << std::endl 
							 << "FILE: " << __FILE__ << std::endl 
							 << "LINE: " << __LINE__ << std::endl 
							 << "FUNCTION: " << __FUNCTION__ << std::endl
							 << "ERROR: " << "invalid matrix dimension";
					throw std::runtime_error(errorMsg.str());
			}
			break;
        }
        return d;
#else
        return determinant(*this);
#endif // 0
    }
    Mat1d Mat1d::inv(void) const
    {
        assert(rows == cols);
#if 0
        double d = this->det();
        Mat1d m;
        if (0.0 != d) {
            m = Mat1d(rows, cols);
            switch (rows)
            {
            case 1:
                m.data[0] = 1. / d;
                break;
            case 2:
                m.data[0] = data[3] / d;
                m.data[1] = -data[1] / d;
                m.data[2] = -data[2] / d;
                m.data[3] = data[0] / d;
                break;
            case 3:
                m.data[0] = (data[4] * data[8] - data[5] * data[7]) / d;
                m.data[1] = (data[2] * data[7] - data[1] * data[8]) / d;
                m.data[2] = (data[1] * data[5] - data[2] * data[4]) / d;

                m.data[3] = (data[5] * data[6] - data[3] * data[8]) / d;
                m.data[4] = (data[0] * data[8] - data[2] * data[6]) / d;
                m.data[5] = (data[2] * data[3] - data[0] * data[5]) / d;

                m.data[6] = (data[3] * data[7] - data[4] * data[6]) / d;
                m.data[7] = (data[1] * data[6] - data[0] * data[7]) / d;
                m.data[8] = (data[0] * data[4] - data[1] * data[3]) / d;
                break;
            default:
				{
					std::ostringstream errorMsg;
					errorMsg << "EXCEPTION OCCURED AT" << std::endl 
							 << "FILE: " << __FILE__ << std::endl 
							 << "LINE: " << __LINE__ << std::endl 
							 << "FUNCTION: " << __FUNCTION__ << std::endl
							 << "ERROR: " << "invalid matrix dimension";
					throw std::runtime_error(errorMsg.str());
				}
                break;
            }
        }
        return m;
#else
        return inverse(*this);
#endif // 0
    }
    Mat1d Mat1d::operator+(const Mat1d &b) const
    {
        return this->arrayOp('+', b);
    }
    Mat1d Mat1d::operator-(const Mat1d &b) const
    {
        return this->arrayOp('-', b);
    }
    Mat1d Mat1d::mul(const Mat1d &b) const
    {
        return this->arrayOp('*', b);
    }
    Mat1d Mat1d::operator/(const Mat1d &b) const
    {
        return this->arrayOp('/', b);
    }
    Mat1d Mat1d::operator*(const Mat1d &b) const
    {
        assert(cols == b.rows);
        Mat1d m(rows, b.cols);
        double m_ij;
        int len = m.rows * m.cols;
        if (len > 0) {
            assert(nullptr != data);
            assert(nullptr != b.data);
            for (int i = 0; i < m.rows; ++i) {
                for (int j = 0; j < m.cols; ++j) {
                    m_ij = 0;
                    for (int k = 0; k < cols; ++k) {
                        m_ij = m_ij + data[int(i * cols + k)] * b.data[int(k * b.cols + j)];
                    }
                    m.data[int(i * m.cols + j)] = m_ij;
                }
            }
        }
        return m;
    }
    Mat1d Mat1d::operator-(void) const
    {
        Mat1d m(rows, cols);
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            for (int k = 0; k < len; ++k) {
                m.data[int(k)] = -data[int(k)];
            }
        }
        return m;
    }
    Mat1d Mat1d::operator+(double c) const
    {
        return this->scalarOp('+', c);
    }
    Mat1d Mat1d::operator-(double c) const
    {
        return this->scalarOp('-', c);
    }
    Mat1d Mat1d::operator*(double c) const
    {
        return this->scalarOp('*', c);
    }
    Mat1d Mat1d::operator/(double c) const
    {
        return this->scalarOp('/', c);
    }
    Mat1d& Mat1d::operator=(double c)
    {
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            for (int k = 0; k < len; ++k) {
                data[int(k)] = c;
            }
        }
        return *this;
    }
    Mat1i Mat1d::operator<(double c) const
    {
        Mat1i m(rows, cols);
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            for (int k = 0; k < len; ++k) {
                m.data[int(k)] = int(data[int(k)] < c);
            }
        }
        return m;
    }
    Mat1i Mat1d::operator!=(const Mat1d & b) const
    {
        Mat1i m(rows, cols);
        int len = rows * cols;
        if (len > 0) {
            assert(nullptr != data);
            assert(nullptr != b.data);
            for (int k = 0; k < len; ++k) {
                m.data[int(k)] = int(data[int(k)] != b.data[int(k)]);
            }
        }
        return m;
    }
    void Mat1d::setTo(int r, int c, const Mat1d &a)
    {
        assert((0 <= r) && (r + a.rows <= rows));
        assert((0 <= c) && (c + a.cols <= cols));
        size_t len = size_t(a.cols) * sizeof(double);
        for (int i = 0; i < a.rows; ++i) {
            std::memcpy(&data[int((r + i) * cols + c)], &a.data[int(i * a.cols)], len);
        }
    }
    void Mat1d::write(std::ofstream &fp, std::string prefix) const
    {
        fp << std::fixed << std::setprecision(16);
        fp << prefix << "rows: " << rows << std::endl
           << prefix << "cols: " << cols << std::endl
		   << prefix << "dt: " << "d" << std::endl
           << prefix << "data: " << this->ostr() << std::endl;
    }
    void Mat1d::read(std::ifstream &fp)
    {
        std::string str, dt;
        fp >> str >> rows >> str >> cols >> str >> dt >> str;
        *this = Mat1d(rows, cols);
        int len = rows * cols;
        for (int k = 0; k < len; ++k) {
            fp >> str >> data[int(k)];
        }
        fp >> str;
    }
    std::string Mat1d::str(void) const
    {
        std::ostringstream out; out << std::fixed << std::setprecision(16);
        out << "[";
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                out << data[int(i * cols + j)];
                if (j == cols - 1) {
                    if (i == rows - 1) {
                        /* DO NOTHING */
                    }
                    else {
                        out << "; ";
                    }
                }
                else {
                    out << " ";
                }
            }
        }
        out << "]";
        return out.str();
    }
    std::string Mat1d::str2(void) const
    {
        std::ostringstream out; out << std::fixed << std::setprecision(16);
        out << "[";
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                out << data[int(i * cols + j)];
                if (j == cols - 1) {
                    if (i == rows - 1) {
                        /* DO NOTHING */
                    }
                    else {
                        out << ";\n ";
                    }
                }
                else {
                    out << ", ";
                }
            }
        }
        out << "]";
        return out.str();
    }
    std::string Mat1d::str3(void) const
    {
        std::ostringstream out; out << std::fixed;
        out << "[";
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                out << data[int(i * cols + j)];
                if (j == cols - 1) {
                    if (i == rows - 1) {
                        /* DO NOTHING */
                    }
                    else {
                        out << "; ";
                    }
                }
                else {
                    out << " ";
                }
            }
        }
        out << "]";
        return out.str();
    }
    std::string Mat1d::str4(void) const
    {
        std::ostringstream out;
        out << "[";
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                out << data[int(i * cols + j)];
                if (j == cols - 1) {
                    if (i == rows - 1) {
                        /* DO NOTHING */
                    }
                    else {
                        out << ";\n ";
                    }
                }
                else {
                    out << ", ";
                }
            }
        }
        out << "]";
        return out.str();
    }
    std::string Mat1d::ostr(void) const
    {
        std::ostringstream out; out << std::fixed << std::setprecision(16);
        out << "[ ";
        int len = rows * cols;
        for (int k = 0; k < len; ++k) {
            out << data[int(k)];
            if (k == len - 1) {
                /* DO NOTHING */
            }
            else {
                out << ", ";
            }
        }
        out << " ]";
        return out.str();
    }
	void Mat1d::disp(void) const
	{
		std::cout << this->str4() << std::endl;
	}
    Mat1d Mat1d::zeros(int rows_, int cols_)
    {
        Mat1d m(rows_, cols_, 0.);
        return m;
    }
    Mat1d Mat1d::ones(int rows_, int cols_)
    {
        Mat1d m(rows_, cols_, 1.);
        return m;
    }
    Mat1d Mat1d::eye(int rows_, int cols_)
    {
        Mat1d m(rows_, cols_, 0.);
        int d = (m.rows < m.cols) ? m.rows : m.cols;
        for (int i = 0; i < d; ++i) {
            m.data[int(i * m.cols + i)] = 1;
        }
        return m;
    }
    Mat1d Mat1d::diag(const Mat1d &d)
    {
        assert((1 == d.rows) || (1 == d.cols));
        int len = d.rows + d.cols - 1;
        Mat1d m(len, len, 0.);
        for (int i = 0; i < len; ++i) {
            m.data[int(i * m.cols + i)] = d.data[int(i)];
        }
        return m;
    }
    Mat1d Mat1d::scalarOp(char op, double c) const
    {
        Mat1d m(rows, cols);
        int len = rows * cols;
        for (int k = 0; k < len; ++k) {
            switch (op)
            {
            case '+':
                m.data[int(k)] = data[int(k)] + c;
                break;
            case '-':
                m.data[int(k)] = data[int(k)] - c;
                break;
            case '*':
                m.data[int(k)] = data[int(k)] * c;
                break;
            case '/':
				{
					if (0. != c) {
						m.data[int(k)] = data[int(k)] / c;
					}
					else {
						m.data[int(k)] = NAN;
					}
				}
				break;
            default:
				{
					std::ostringstream errorMsg;
					errorMsg << "EXCEPTION OCCURED AT" << std::endl 
							 << "FILE: " << __FILE__ << std::endl 
							 << "LINE: " << __LINE__ << std::endl 
							 << "FUNCTION: " << __FUNCTION__ << std::endl
							 << "ERROR: " << "invalid scalar matrix operator, should be one of {+, -, '*', '/'}";
					throw std::runtime_error(errorMsg.str());
				}
				break;
            }
        }
        return m;
    }
    Mat1d Mat1d::arrayOp(char op, const Mat1d &b) const
    {
        assert((rows == b.rows) && (cols == b.cols));
        Mat1d m(rows, cols);
        int len = rows * cols;
        for (int k = 0; k < len; ++k) {
            switch (op)
            {
            case '+':
                m.data[int(k)] = data[int(k)] + b.data[int(k)];
                break;
            case '-':
                m.data[int(k)] = data[int(k)] - b.data[int(k)];
                break;
            case '*':
                m.data[int(k)] = data[int(k)] * b.data[int(k)];
                break;
            case '/':
				{
					if (0. != b.data[int(k)]) {
						m.data[int(k)] = data[int(k)] / b.data[int(k)];
					}
					else {
						m.data[int(k)] = NAN;
					}
				}
				break;
            default:
				{
					std::ostringstream errorMsg;
					errorMsg << "EXCEPTION OCCURED AT" << std::endl 
							 << "FILE: " << __FILE__ << std::endl 
							 << "LINE: " << __LINE__ << std::endl 
							 << "FUNCTION: " << __FUNCTION__ << std::endl
							 << "ERROR: " << "invalid array operator, should be one of {+, -, '*', '/'}";
					throw std::runtime_error(errorMsg.str());
				}
				break;
            }
        }
        return m;
    }
    Mat1d operator+(double k, const Mat1d &b)
    {
        return b + k;
    }
    Mat1d operator-(double k, const Mat1d &b)
    {
        return -b + k;
    }
    Mat1d operator*(double k, const Mat1d &b)
    {
        return b * k;
    }
    Mat1d operator/(double k, const Mat1d & b)
    {
        return Mat1d(b.rows, b.cols, k) / b;
    }
    Mat1d exp(const Mat1d &a)
    {
        Mat1d m(a.rows, a.cols);
        int len = a.rows * a.cols;
        for (int k = 0; k < len; ++k) {
            m.data[int(k)] = std::exp(a.data[int(k)]);
        }
        return m;
    }
    Mat1d pow(const Mat1d &a, double power)
    {
        Mat1d m(a.rows, a.cols);
        int len = a.rows * a.cols;
        for (int k = 0; k < len; ++k) {
            m.data[int(k)] = std::pow(a.data[int(k)], power);
        }
        return m;
    }
	Mat1d abs(const Mat1d &a)
	{
		Mat1d m(a.rows, a.cols);
		int len = a.rows * a.cols;
		for (int k = 0; k < len; ++k) {
			m.data[int(k)] = std::abs(a.data[int(k)]);
		}
		return m;
	}
	Mat1d max(const Mat1d &a, double b)
	{
		Mat1d m(a.rows, a.cols);
		int len = a.rows * a.cols;
		for (int k = 0; k < len; ++k) {
			m.data[int(k)] = std::max(a.data[int(k)], b);
		}
		return m;
	}
	Mat1d min(const Mat1d &a, double b)
	{
		Mat1d m(a.rows, a.cols);
		int len = a.rows * a.cols;
		for (int k = 0; k < len; ++k) {
			m.data[int(k)] = std::min(a.data[int(k)], b);
		}
		return m;
	}
    double sum(const Mat1d &a)
    {
        double s = 0.0;
        int len = a.rows * a.cols;
        for (int k = 0; k < len; ++k) {
            s = s + a.data[int(k)];
        }
        return s;
    }
    Mat1d repeat(const Mat1d &a, int ny, int nx)
    {
        assert((ny > 0) && (nx > 0));
        Mat1d m(ny * a.rows, nx * a.cols);
        double *dst_ptr = m.data;
        double *src_ptr = nullptr;
        for (int i = 0; i < a.rows; ++i) {
            src_ptr = &a.data[int(i * a.cols)];
            for (int x = 0; x < nx; ++x) {
                std::memcpy(dst_ptr, src_ptr, size_t(a.cols) * sizeof(double));
                dst_ptr = &dst_ptr[int(a.cols)];
            }
        }
        /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
        assert(nullptr != m.data);
        src_ptr = m.data;
        dst_ptr = &m.data[int(a.rows * m.cols)];
        for (int y = 1; y < ny; ++y) {
            std::memcpy(dst_ptr, src_ptr, size_t(a.rows * m.cols) * sizeof(double));
            dst_ptr = &dst_ptr[int(a.rows * m.cols)];
        }
        return m;
    }
    int countNonZero(const Mat1i &a)
    {
        int nnz = 0;
        int len = a.rows * a.cols;
        for (int k = 0; k < len; ++k) {
            if (0 != a.data[int(k)]) {
                nnz = nnz + 1;
            }
        }
        return nnz;
    }
    int countNonZero(const Mat1d &a)
    {
        int nnz = 0;
        int len = a.rows * a.cols;
        for (int k = 0; k < len; ++k) {
            if (0 != a.data[int(k)]) {
                nnz = nnz + 1;
            }
        }
        return nnz;
    }
    void minMaxLoc(const Mat1d &a, double *minVal, double *maxVal, Point2i *minLoc, Point2i *maxLoc)
    {

        double minval = a.data[0];
        double maxval = a.data[0];
        Point2i minloc(0, 0);
        Point2i maxloc(0, 0);
        double a_ij;
        for (int i = 0; i < a.rows; ++i) {
            for (int j = 0; j < a.cols; ++j) {
                a_ij = a.data[int(i * a.cols + j)];
                if (a_ij < minval) {
                    minval = a_ij;
                    minloc = Point2i(j, i);
                }
                if (a_ij > maxval) {
                    maxval = a_ij;
                    maxloc = Point2i(j, i);
                }
            }
        }
        if (nullptr != minVal) {
            *minVal = minval;
        }
        if (nullptr != maxVal) {
            *maxVal = maxval;
        }
        if (nullptr != minLoc) {
            *minLoc = minloc;
        }
        if (nullptr != maxLoc) {
            *maxLoc = maxloc;
        }
    }
    double norm(const Mat1d &a, int normType)
    {
        assert((NORM_INF == normType) || (NORM_L1 == normType) || (NORM_L2 == normType));
        assert((1 == a.rows) || (1 == a.cols));
        double abs_aij;
        double v = 0;
        int len = a.rows * a.cols;
        for (int k = 0; k < len; ++k) {
            abs_aij = std::abs(a.data[int(k)]);
            switch (normType)
            {
            case NORM_INF:
                v = v < abs_aij ? abs_aij : v;
                break;
            case NORM_L1:
                v = v + abs_aij;
                break;
            case NORM_L2:
                v = v + std::pow(abs_aij, 2);
                break;
            default:
                break;
            }
        }
        return NORM_L2 == normType ? std::sqrt(v) : v;
    }
    Mat1d inverse(const Mat1d &x)
    {
        Mat1d A = x.clone();
        Mat1d y = Mat1d::zeros(x.rows, x.cols);
        int n;
        int i1;
        int yk;
        int b_n;
        int k;
        int j;
        int mmj;
        int c;
        int ix;
        double smax;
        int i2;
        int jy;
        double s;
        int ijA;
        n = x.rows;
        yk = x.rows;
        if (yk < 1) {
            b_n = 0;
        }
        else {
            b_n = yk;
        }

        cv::Mat1i ipiv(1, b_n);
        /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
        assert(nullptr != ipiv.data);
        if (b_n > 0) {
            ipiv.data[0] = 1;
            yk = 1;
            for (k = 2; k <= b_n; k++) {
                yk++;
                ipiv.data[int(k - 1)] = yk;
            }
        }

        if (x.rows < 1) {
            /* DO NOTHING */
        }
        else {
            if (x.rows - 1 <= x.rows) {
                i1 = x.rows - 1;
            }
            else {
                i1 = x.rows;
            }
            /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
            assert(nullptr != A.data);
            for (j = 0; j + 1 <= i1; j++) {
                mmj = n - j;
                c = j * (n + 1);
                if (mmj < 1) {
                    yk = -1;
                }
                else {
                    yk = 0;
                    if (mmj > 1) {
                        ix = c;
                        /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
                        smax = std::abs(A.data[int(c)]);
                        for (k = 2; k <= mmj; k++) {
                            ix++;
                            s = std::abs(A.data[int(ix)]);
                            if (s > smax) {
                                yk = k - 1;
                                smax = s;
                            }
                        }
                    }
                }
                if (A.data[int(c + yk)] != 0.0) {
                    if (yk != 0) {
                        ipiv.data[int(j)] = (j + yk) + 1;
                        ix = j;
                        yk += j;
                        for (k = 1; k <= n; k++) {
                            /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
                            smax = A.data[int(ix)];
                            A.data[int(ix)] = A.data[int(yk)];
                            A.data[int(yk)] = smax;
                            ix += n;
                            yk += n;
                        }
                    }

                    i2 = c + mmj;
                    for (jy = c + 1; jy + 1 <= i2; jy++) {
                        A.data[int(jy)] /= A.data[int(c)];
                    }
                }

                yk = n - j;
                b_n = (c + n) + 1;
                jy = c + n;
                for (k = 1; k < yk; k++) {
                    /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
                    smax = A.data[int(jy)];
                    if (A.data[int(jy)] != 0.0) {
                        ix = c + 1;
                        i2 = mmj + b_n;
                        for (ijA = b_n; ijA + 1 < i2; ijA++) {
                            A.data[int(ijA)] += A.data[int(ix)] * -smax;
                            ix++;
                        }
                    }

                    jy += n;
                    b_n += n;
                }
            }
        }

        if (x.rows < 1) {
            b_n = 0;
        }
        else {
            b_n = x.rows;
        }

        cv::Mat1i p(1, b_n);
        /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
        assert(nullptr != p.data);
        if (b_n > 0) {
            p.data[0] = 1;
            yk = 1;
            for (k = 2; k <= b_n; k++) {
                yk++;
                p.data[int(k - 1)] = yk;
            }
        }

        for (k = 0; k < ipiv.cols; k++) {
            if (ipiv.data[int(k)] > 1 + k) {
                /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
                yk = p.data[int(ipiv.data[int(k)] - 1)];
                p.data[int(ipiv.data[int(k)] - 1)] = p.data[int(k)];
                p.data[int(k)] = yk;
            }
        }
        /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
        assert(nullptr != y.data);
        for (k = 0; k + 1 <= n; k++) {
            c = p.data[int(k)] - 1;
            /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
            y.data[int(k + y.rows * (p.data[int(k)] - 1))] = 1.0;
            for (j = k; j + 1 <= n; j++) {
                if (y.data[int(j + y.rows * c)] != 0.0) {
                    for (jy = j + 1; jy + 1 <= n; jy++) {
                        y.data[int(jy + y.rows * c)] -= y.data[int(j + y.rows * c)] * A.data[int(jy + A.rows * j)];
                    }
                }
            }
        }

        if ((x.rows == 0) || ((y.rows == 0) || (y.cols == 0))) {
            /* DO NOTHING */
        }
        else {
            for (j = 1; j <= n; j++) {
                yk = n * (j - 1) - 1;
                for (k = n; k > 0; k--) {
                    b_n = n * (k - 1) - 1;
                    if (y.data[int(k + yk)] != 0.0) {
                        smax = y.data[int(k + yk)];
                        s = A.data[int(k + b_n)];
                        assert(0. != s);
                        y.data[int(k + yk)] = smax / s;
                        for (jy = 1; jy < k; jy++) {
                            /* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
                            y.data[int(jy + yk)] -= y.data[int(k + yk)] * A.data[int(jy + b_n)];
                        }
                    }
                }
            }
        }
        return y;
    }
    double determinant(const Mat1d &M)
    {
        double d;
        int m;
        int n;
        int i0;
        int yk;
        int jA;
        int jy;
        int j;
        int mmj;
        int c;
        bool isodd;
        int ix;
        double smax;
        int i1;
        int b_j;
        double s;
        int ijA;
        if ((M.rows == 0) || (M.cols == 0)) {
            d = 1.0;
        }
        else {
            m = M.rows;
            n = M.cols;
            Mat1d A = M.clone();
            if (M.rows <= M.cols) {
                jA = M.rows;
            }
            else {
                jA = M.cols;
            }

            Mat1i ipiv(1, jA);
            ipiv.rows = 1;
            ipiv.cols = jA;
            ipiv.data[0] = 1;
            yk = 1;
            for (jy = 2; jy <= jA; jy++) {
                yk++;
                ipiv.data[int(jy - 1)] = yk;
            }

            if (M.rows - 1 <= M.cols) {
                i0 = M.rows - 1;
            }
            else {
                i0 = M.cols;
            }

            for (j = 0; j + 1 <= i0; j++) {
                mmj = m - j;
                c = j * (m + 1);
                if (mmj < 1) {
                    yk = -1;
                }
                else {
                    yk = 0;
                    if (mmj > 1) {
                        ix = c;
                        smax = std::abs(A.data[int(c)]);
                        for (jy = 2; jy <= mmj; jy++) {
                            ix++;
                            s = std::abs(A.data[int(ix)]);
                            if (s > smax) {
                                yk = jy - 1;
                                smax = s;
                            }
                        }
                    }
                }

                if (A.data[int(c + yk)] != 0.0) {
                    if (yk != 0) {
                        ipiv.data[int(j)] = (j + yk) + 1;
                        ix = j;
                        yk += j;
                	 	/* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
                     	assert(nullptr != A.data);
                        for (jy = 1; jy <= n; jy++) {
                            smax = A.data[int(ix)];
                            A.data[int(ix)] = A.data[int(yk)];
                            A.data[int(yk)] = smax;
                            ix += m;
                            yk += m;
                        }
                    }

                    i1 = c + mmj;
                    for (yk = c + 1; yk + 1 <= i1; yk++) {
                        A.data[int(yk)] /= A.data[int(c)];
                    }
                }

                yk = n - j;
                jA = (c + m) + 1;
                jy = c + m;
         		/* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
                assert(nullptr != A.data);
                for (b_j = 1; b_j < yk; b_j++) {
                    smax = A.data[int(jy)];
                    if (A.data[int(jy)] != 0.0) {
                        ix = c + 1;
                        i1 = mmj + jA;
                        for (ijA = jA; ijA + 1 < i1; ijA++) {
                            A.data[int(ijA)] += A.data[int(ix)] * -smax;
                            ix++;
                        }
                    }

                    jy += m;
                    jA += m;
                }
            }
    		/* LDRA_EXCLUDE 45 D <justification start> LDRA is reporting wrong violation: The Pointer has been checked for null before use. <justification end> */
            assert(nullptr != A.data);
            d = A.data[0];
            for (jy = 1; jy - 1 <= A.rows - 2; jy++) {
                d *= A.data[int(jy + A.rows * jy)];
            }

            isodd = false;
            for (jy = 0; jy <= ipiv.cols - 2; jy++) {
                if (ipiv.data[int(jy)] > 1 + jy) {
                    isodd = !isodd;
                }
            }

            if (isodd) {
                d = -d;
            }
        }
        return d;
    }
	Mat1d chol(const Mat1d &A)
	{
		int idxAjj;
		int i;
		int n;
		double ssq;
		int iy;
		int nmj;
		int iac;
		double c;
		cv::Mat1d A_chol = A.clone();
		idxAjj = A.rows * A.cols;

		n = A.cols;
		if (A.cols != 0) {
			int info;
			int j;
			bool exitg1;
			info = -1;
			j = 0;
			exitg1 = false;
			while ((!exitg1) && (j <= n - 1)) {
				int idxA1j;
				int ix;
				idxA1j = j * n;
				idxAjj = idxA1j + j;
				ssq = 0.0;
				if (j >= 1) {
					ix = idxA1j;
					iy = idxA1j;
					for (nmj = 0; nmj < j; nmj++) {
						ssq += A_chol.data[ix] * A_chol.data[iy];
						ix++;
						iy++;
					}
				}

				ssq = A_chol.data[idxAjj] - ssq;
				if (ssq > 0.0) {
					ssq = std::sqrt(ssq);
					A_chol.data[idxAjj] = ssq;
					if (j + 1 < n) {
						int ia0;
						int idxAjjp1;
						nmj = (n - j) - 2;
						ia0 = (idxA1j + n) + 1;
						idxAjjp1 = idxAjj + n;
						if ((j != 0) && (nmj + 1 != 0)) {
							iy = idxAjjp1;
							i = ia0 + n * nmj;
							for (iac = ia0; n < 0 ? iac >= i : iac <= i; iac += n) {
								ix = idxA1j;
								c = 0.0;
								idxAjj = (iac + j) - 1;
								for (int ia = iac; ia <= idxAjj; ia++) {
									c += A_chol.data[ia - 1] * A_chol.data[ix];
									ix++;
								}

								A_chol.data[iy] = A_chol.data[iy] + -c;
								iy += n;
							}
						}

						ssq = 1.0 / ssq;
						i = (idxAjjp1 + n * nmj) + 1;
						for (nmj = idxAjjp1 + 1; n < 0 ? nmj >= i : nmj <= i; nmj += n) {
							A_chol.data[nmj - 1] = ssq * A_chol.data[nmj - 1];
						}
					}

					j++;
				}
				else {
					A_chol.data[idxAjj] = ssq;
					info = j;
					exitg1 = true;
				}
			}

			if (info + 1 == 0) {
				idxAjj = A.cols;
			}
			else {
				idxAjj = info;
			}

			for (j = 0; j < idxAjj; j++) {
				i = j + 2;
				for (nmj = i; nmj <= idxAjj; nmj++) {
					A_chol.data[(nmj + A_chol.rows * j) - 1] = 0.0;
				}
			}
		}
		return A_chol;
	}
}









