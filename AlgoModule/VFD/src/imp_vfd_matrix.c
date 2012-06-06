#include "imp_vfd_matrix.h"

int IMP_Round(double f) 
{
	int a = (int)(f + 0.5);
	int b = (int)f;

	return a>b?a:b;
}
Imp_Rect  IMP_Rect( int x, int y, int width, int height )
{
	Imp_Rect r;
	
	r.x = x;
	r.y = y;
	r.width = width;
	r.height = height;
	
	return r;
}


Imp_Point  IMP_Point( int x, int y )
{
	Imp_Point p;
	
	p.x = x;
	p.y = y;
	
	return p;
}

Imp_Size  IMP_Size( int width, int height )
{
	Imp_Size s;
	
	s.width = width;
	s.height = height;
	
	return s;
}

IMP_Scalar  IMP_ScalarAll( double val0123 )
{
    IMP_Scalar scalar;
    scalar.val[0] = val0123;
    scalar.val[1] = val0123;
    scalar.val[2] = val0123;
    scalar.val[3] = val0123;
    return scalar;
}

void* IMP_AlignPtr( const void* ptr, int align )
{
    assert( (align & (align-1)) == 0 );
    return (void*)( ((size_t)ptr + align - 1) & ~(size_t)(align-1) );
}

int IMP_Align( int size, int align )
{
    assert( (align & (align-1)) == 0 && size < IMP_INT_MAX );
    return (size + align - 1) & -align;
}

static void IMP_CheckHuge( IMP_Mat* arr )
{
    if( (IMP_S64)arr->step*arr->rows > IMP_INT_MAX )
        arr->type &= ~IMP_MAT_CONT_FLAG;
}

// Decrements IMP_Mat data reference counter and deallocates the data if  it reaches 0 
void  IMP_DecRefData( IMP_Mat* arr )
{
    if( IMP_IS_MAT( arr ))
    {
        IMP_Mat* mat = (IMP_Mat*)arr;
        mat->data.ptr = NULL;
        if( mat->refcount != NULL && --*mat->refcount == 0 )
            free( mat->refcount );
        mat->refcount = NULL;
    }
}

 
 // Deallocates the IMP_Mat structure and underlying data
 void IMP_ReleaseMat( IMP_Mat** array )
 {
	 
	 int err = 0;
	 if( !array )
		 err = 1;
	 
	 if( *array )
	 {
		 IMP_Mat* arr = *array;
		 
		 if( !IMP_IS_MAT_HDR(arr) )
			 err = 2;
		 
		 *array = 0;
		 
		 IMP_DecRefData( arr );
		 free( arr );
	 }
}

// Creates IMP_Mat header only
IMP_Mat* IMP_CreateMatHeader( int rows, int cols, int type )
{
	int err = 0;
    IMP_Mat* arr = 0;   
	
    int min_step;
    type = IMP_MAT_TYPE(type);
	
    if( rows <= 0 || cols <= 0 )
	{
		err = 1;
        printf("Non-positive width or height\n" );
	}
	
    min_step = IMP_ELEM_SIZE(type)*cols;
    if( min_step <= 0 )
	{
		err = 2;
        printf("Invalid matrix type\n" );
	}
	
    arr = (IMP_Mat*)malloc( sizeof(*arr));
	
    arr->step = rows == 1 ? 0 : IMP_AlignPtr(min_step, IMP_DEFAULT_MAT_ROW_ALIGN);
    arr->type = IMP_MAT_MAGIC_VAL | type |
		(arr->step == 0 || arr->step == min_step ? IMP_MAT_CONT_FLAG : 0);
    arr->rows = rows;
    arr->cols = cols;
    arr->data.ptr = 0;
    arr->refcount = 0;
    arr->hdr_refcount = 1;
	
    IMP_CheckHuge( arr );
 
    if( err )
        IMP_ReleaseMat( &arr );
	
    return arr;
}

// Allocates underlying array data
void IMP_CreateData( IMP_Mat* arr )
{  
    if( IMP_IS_MAT_HDR( arr ))
    {
        size_t step, total_size;
        IMP_Mat* mat = (IMP_Mat*)arr;
        step = mat->step;
		
        if( mat->data.ptr != 0 )
            printf( "Data is already allocated\n" );
		
        if( step == 0 )
            step = IMP_ELEM_SIZE(mat->type)*mat->cols;
		
        total_size = step*mat->rows + sizeof(int) + IMP_MALLOC_ALIGN;
        mat->refcount = (int*)malloc( (size_t)total_size );
        mat->data.ptr = (unsigned char*)IMP_AlignPtr( mat->refcount + 1, IMP_MALLOC_ALIGN );
        *mat->refcount = 1;
    }
    else
    {
        printf( "unrecognized or unsupported array type\n" );
    }
}
 
// Creates IMP_Mat and underlying data
IMP_Mat* IMP_CreateMat( int height, int width,  int type )
{
	IMP_Mat* arr = 0;	
    arr = IMP_CreateMatHeader( height, width, type );
    IMP_CreateData( arr );
	
    return arr;
}

// convert array (IMP_Mat) to IMP_Mat
IMP_Mat* IMP_GetMat( const IMP_Mat* array, IMP_Mat* mat, int* pCOI)
{
    IMP_Mat* result = 0;
    IMP_Mat* src = (IMP_Mat*)array;
    int coi = 0;
    
     if( !mat || !src )
        printf("NULL array pointer is passed\n" );

    if( IMP_IS_MAT_HDR(src))
    {
        if( !src->data.ptr )
            printf("The matrix has NULL data pointer\n" );
        
        result = (IMP_Mat*)src;
    }
    else
    {
        printf( "Unrecognized or unsupported array type\n" );
    }


    if( pCOI )
        *pCOI = coi;

    return result;
}
Imp_Size IMP_GetMatSize( IMP_Mat *mat )
{
	Imp_Size size;
	size.width = mat->cols;
	size.height = mat->rows;
	return size;
}

int IMP_Set_8u_C1R( unsigned char* dst, int dst_step, Imp_Size size, const void* scalar, int pix_size )
{
    int copy_len = 12*pix_size;
    unsigned char* dst_limit = dst + size.width;
    
    if( size.height-- )
    {
        while( dst + copy_len <= dst_limit )
        {
            memcpy( dst, scalar, copy_len );
            dst += copy_len;
        }
		
        memcpy( dst, scalar, dst_limit - dst );
    }
	
    if( size.height )
    {
        dst = dst_limit - size.width + dst_step;
		
        for( ; size.height--; dst += dst_step )
            memcpy( dst, dst - dst_step, size.width );
    }
	
    return IMP_TRUE;
}

int IMP_SetZero_8u_C1R( unsigned char* dst, int dststep, Imp_Size size )
{

    for( ; size.height--; dst += dststep )
        memset( dst, 0, size.width );
	
    return IMP_TRUE;
}


void IMP_SetZero( IMP_Mat* arr )
{
    IMP_Mat stub, *mat = (IMP_Mat*)arr;
    Imp_Size size;
    int mat_step;
	
    if( !IMP_IS_MAT( mat ))
    {
		
        
		int coi = 0;
		  mat = IMP_GetMat( mat, &stub, &coi );
		if( coi != 0 )
			printf( "coi is not supported\n" );
        
    }
	
    size = IMP_GetMatSize( mat );
    size.width *= IMP_ELEM_SIZE(mat->type);
    mat_step = mat->step;
	
    if( IMP_IS_MAT_CONT( mat->type ))
    {
        size.width *= size.height;
		
        if( size.width <= IMP_MAX_INLINE_MAT_OP_SIZE*(int)sizeof(double) )
        {
            memset( mat->data.ptr, 0, size.width );
            return;
        }
		
        mat_step = IMP_STUB_STEP;
        size.height = 1;
    }
	
    IMP_SetZero_8u_C1R( mat->data.ptr, mat_step, size );
	
}


IMP_Mat IMP_Mat_Construct( int rows, int cols, int type, void* data)
{
    IMP_Mat m;
	
    assert( (unsigned)IMP_MAT_DEPTH(type) <= IMP_64F );
    type = IMP_MAT_TYPE(type);
    m.type = IMP_MAT_MAGIC_VAL | IMP_MAT_CONT_FLAG | type;
    m.cols = cols;
    m.rows = rows;
    m.step = rows > 1 ? m.cols*IMP_ELEM_SIZE(type) : 0;
    m.data.ptr = (unsigned char*)data;
    m.refcount = NULL;
    m.hdr_refcount = 0;
	
    return m;
}

// Converts IMP_Scalar to specified type
void IMP_ScalarToRawData( const IMP_Scalar* scalar, void* data, int type, int extend_to_12 )
{	
	int cn,depth;
    type = IMP_MAT_TYPE(type);  
    cn = IMP_MAT_CN( type );
    depth = type & IMP_MAT_DEPTH_MASK;
	
    assert( scalar && data );
    if( (unsigned)(cn - 1) >= 4 )
        printf("The number of channels must be 1, 2, 3 or 4\n" );
	
    switch( depth )
    {
    case IMP_8UC1:
        while( cn-- )
        {
            int t = IMP_Round( scalar->val[cn] );
            ((unsigned char*)data)[cn] = IMP_CAST_8U(t);
        }
        break;
    case IMP_8SC1:
        while( cn-- )
        {
            int t = IMP_Round( scalar->val[cn] );
            ((char*)data)[cn] = IMP_CAST_8S(t);
        }
        break;
    case IMP_16UC1:
        while( cn-- )
        {
            int t = IMP_Round( scalar->val[cn] );
            ((unsigned short*)data)[cn] = IMP_CAST_16U(t);
        }
        break;
    case IMP_16SC1:
        while( cn-- )
        {
            int t = IMP_Round( scalar->val[cn] );
            ((short*)data)[cn] = IMP_CAST_16S(t);
        }
        break;
    case IMP_32SC1:
        while( cn-- )
            ((int*)data)[cn] = IMP_Round( scalar->val[cn] );
        break;
    case IMP_32FC1:
        while( cn-- )
            ((float*)data)[cn] = (float)(scalar->val[cn]);
        break;
    case IMP_64FC1:
        while( cn-- )
            ((double*)data)[cn] = (double)(scalar->val[cn]);
        break;
    default:
        assert(0);
    }
	
    if( extend_to_12 )
    {
        int pix_size = IMP_ELEM_SIZE(type);
        int offset = IMP_ELEM_SIZE1(depth)*12;
		
        do
        {
            offset -= pix_size;
            IMP_MEMCPY_AUTO( (char*)data + offset, data, pix_size );
        }
        while( offset > pix_size );
    }
	
}

// dst(idx) = value 
void IMP_Set( void* arr, IMP_Scalar value, const void* maskarr )
{
	int mat_step;
	Imp_Size size;
    IMP_Mat stub;
	int pix_size, type;
    double buf[12];  
    
	IMP_Mat *mat = (IMP_Mat*)arr;


    if( !value.val[0] && !value.val[1] &&
        !value.val[2] && !value.val[3] && !maskarr )
    {
        IMP_SetZero( arr );
        return;
    }
	
    if( !IMP_IS_MAT(mat))
    {
		int coi = 0;
		mat = IMP_GetMat( mat, &stub, &coi );
		
		if( coi != 0 )
			printf( "Bad COI\n" );		
    }

    type = IMP_MAT_TYPE( mat->type );
    pix_size = IMP_ELEM_SIZE(type);
    size = IMP_GetMatSize( mat );
    mat_step = mat->step;

    if( !maskarr )
    {
        if( IMP_IS_MAT_CONT( mat->type ))
        {
            size.width *= size.height;
        
            if( size.width <= (int)(IMP_MAX_INLINE_MAT_OP_SIZE*sizeof(double)))
            {
                if( type == IMP_32FC1 )
                {
                    float* dstdata = (float*)(mat->data.ptr);
                    float val = (float)value.val[0];

                    do
                    {
                        dstdata[size.width-1] = val;
                    }
                    while( --size.width );

                    return;
                }

                if( type == IMP_64FC1 )
                {
                    double* dstdata = (double*)(mat->data.ptr);
                    double val = value.val[0];

                    do
                    {
                        dstdata[size.width-1] = val;
                    }
                    while( --size.width );

                    return;
                }
            }

            mat_step = IMP_STUB_STEP;
            size.height = 1;
        }
        
        size.width *= pix_size;
        IMP_ScalarToRawData( &value, buf, type, 1 );

        IMP_Set_8u_C1R( mat->data.ptr, mat_step, size, buf, IMP_ELEM_SIZE1(type));
    }
}
