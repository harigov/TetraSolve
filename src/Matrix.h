#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <cstdlib>
#include <vector>

using namespace std;

/////////////////////////////////////////////////////
// MATRIX CLASS DECLARATION BEGIN

template<class T = double>
class Matrix
{
private:
	//vector<T> matrix;
	T *matrix;
	int rows,columns;
public:
	Matrix(int r,int c);
	Matrix(const Matrix&);
	~Matrix();

	T& getElement(int row,int col);
	T getElement(int row,int col) const;
	void setElement(int row,int col,T);
	void setElement(int row,int col,T) const;
	int getRows();
	int getColumns();
	bool isElementPresent(T value);

	Matrix getTranspose();
	Matrix getInverse();
	void operator=(const Matrix&);
	Matrix* operator+(const Matrix&) const;
	Matrix* operator*(const Matrix&) const;
	T& operator()(int i,int j);

	void LoadIdentityMatrix();
	void LoadNullMatrix();
	void Power(double);
	void PrintElements();
};

template<class T>
Matrix<T>* Multiply (Matrix<T>* m1, Matrix<T>* m2);
template<class T>
Matrix<T>* Add (Matrix<T>* m1, Matrix<T>* m2);

// MATRIX CLASS DECLARATION END
/////////////////////////////////////////////////////



template <class T>
Matrix<T>::Matrix( int r, int c)//:matrix(r*c)
{
	rows=r;
	columns=c;

	try {
		matrix = new T[r*c];
	} catch (bad_alloc) {
		cerr << "Memory allocation error.\n";
		exit (1);
	}
	
	LoadNullMatrix();
}

template <class T>
Matrix<T>::Matrix(const Matrix<T>& m):matrix(m.r*m.c)
{
	rows=m.rows;
	columns=m.columns;
	//matrix.insert(matrix.begin(), m.matrix.begin(), m.matrix.end());
	for (int i = 0; i < rows*columns; i++)
		matrix[i] = m.matrix[i];

	cout << "init";
}

template <class T>
Matrix<T>::~Matrix()
{
	DBG_MSG("destructor");
	delete[] matrix;
	//matrix.clear();
}

template <class T>
T& Matrix<T>::getElement(int i, int j)
{
	if (i < rows && j < columns)
		return matrix[i*columns + j];
	else {
		cerr << "Index out of bounds.\n";
		exit (1);
	}
}

template <class T>
void Matrix<T>::setElement(int i,int j,T value)
{	
	if (i < rows && j < columns)
		matrix[i*columns + j] = value;
	else {
		cerr << "Index out of bounds.\n";
		exit (1);
	}
}

template <class T>
T Matrix<T>::getElement(int i,int j) const
{
	if (i < rows && j < columns)
		return matrix[i*columns + j];
	else {
		cerr << "Index out of bounds.\n";
		exit (1);
	}
}

template <class T>
void Matrix<T>::setElement(int i,int j,T value) const
{		
	if (i < rows && j < columns)
		matrix[i*columns + j] = value;
	else {
		cerr << "Index out of bounds.\n";
		exit (1);
	}
}

template <class T>
int Matrix<T>::getRows()
{
	return rows;
}

template <class T>
int Matrix<T>::getColumns()
{
	return columns;
}

template<class T>
bool Matrix<T>::isElementPresent(T value)
{
	for(int i = 0 ; i < rows ; i++)
	{
		for(int j = 0 ; j < columns ; j++)
		{
			if(value == getElement (i,j))
				return true;
		}
	}
	return false;
}

template <class T>
Matrix<T> Matrix<T>::getTranspose()
{
	Matrix<T> *resultant = new Matrix<T>(columns,rows);
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			resultant->setElement(j,i,getElement(i,j));
		}
	}
	return *resultant;
}

template <class T>
Matrix<T> Matrix<T>::getInverse()
{
	Matrix<T> *resultant(rows,columns);
	return *resultant;
}

template <class T>
void Matrix<T>::operator=(const Matrix<T>& m1)
{
	if(rows == m1.rows && columns == m1.columns)
	{
		for(int i=0;i<m1.rows;i++)
		{
			for(int j=0;j<m1.columns;j++)
			{
				setElement(i,j,m1.getElement(i,j));
			}
		}	
	}
}

template <class T>
Matrix<T>* Matrix<T>::operator+(const Matrix<T>& m) const
{
	Matrix<T> *res = new Matrix<T> ( rows, columns);
	if((rows == m.rows) && (columns == m.columns))
	{
		for(int i = 0; i < rows; i++)
		{
			for(int j = 0; j < columns; j++)
			{
				res->setElement( i, j, getElement(i,j) + 
					m.getElement(i,j));
			}
		}
	}
	else
	{
		cerr << "ERROR: Addition of two unequal size matrices." << endl;
		exit (1);
	}

	return res;
}

//	Multiply Function

template <class T>
Matrix<T>* Matrix<T>::operator*(const Matrix<T>& m) const
{
	Matrix<T> *res = new Matrix<T>(rows,m.columns);
	if(columns==m.rows)
	{
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<m.columns;j++)
			{
				res->setElement(i,j,0);
				for(int k=0;k<columns;k++)
				{
					res->setElement(i,j,res->getElement(i,j) + 
							(getElement(i,k))*(m.getElement(k,j)));
				}
			}
		}
	}
	else
		res->LoadIdentityMatrix();
	//printElements ();
	return res;
}

template<class T>
T& Matrix<T>::operator()(int i,int j)
{
	return getElement(i,j);
}

template <class T>
void Matrix<T>::LoadIdentityMatrix()
{
	LoadNullMatrix();
	for(int i=0;i<rows;i++)
	{
		setElement(i,i,T(1.0));
	}
}

template <class T>
void Matrix<T>::LoadNullMatrix()
{
	for(int i=0;i<rows;i++)
		for(int j=0;j<columns;j++)
			setElement(i,j,T());
}

template <class T>
void Matrix<T>::Power(double power)
{
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			setElement(i,j,pow(getElement(i,j),power));
		}
	}
}

template <class T>
void Matrix<T>::PrintElements()
{
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
			cout << getElement(i,j) << "\t";
		cout << endl;
	}
}

template <class T>
Matrix<T>* Multiply (Matrix<T>* m1, Matrix<T>* m2)
{
	return ((*m1)*(*m2));
}

template <class T>
Matrix<T>* Add (Matrix<T>* m1, Matrix<T>* m2)
{
	return ((*m1)+(*m2));
}


#endif /* __MATRIX_H__ */
