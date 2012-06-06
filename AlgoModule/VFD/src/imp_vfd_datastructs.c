#include "imp_vfd_xtype.h"


#define IMP_FREE_PTR(storage)  \
    ((char*)(storage)->top + (storage)->block_size - (storage)->free_space)

#define IIMP_ALIGNED_SEQ_BLOCK_SIZE  \
    (int)IMP_Align(sizeof(IMP_SeqBlock), IMP_STRUCT_ALIGN)

int IMP_AlignLeft( int size, int align )
{
    return size & -align;
}

#define IMP_GET_LAST_ELEM( seq, block ) \
    ((block)->data + ((block)->count - 1)*((seq)->elem_size))

#define IMP_SWAP_ELEMS(a,b,elem_size)  \
{                                     \
    int k;                            \
    for( k = 0; k < elem_size; k++ )  \
    {                                 \
        char t0 = (a)[k];             \
        char t1 = (b)[k];             \
        (a)[k] = t1;                  \
        (b)[k] = t0;                  \
    }                                 \
}

#define IIMP_SHIFT_TAB_MAX 32
static const char gaPower2ShiftTab[] =
{
    0, 1, -1, 2, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, 4,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 5
};


IMP_Slice IMP_Slice_Construct( int start, int end )
{
    IMP_Slice slice;
    slice.start_index = start;
    slice.end_index = end;
	
    return slice;
}

/****************************************************************************************\
*                               Sequence Reader implementation                           *
\****************************************************************************************/

/* initializes sequence reader */
void IMP_StartReadSeq( const IMP_Seq *seq, IMP_SeqReader * reader, int reverse )
{
    IMP_SeqBlock *first_block;
    IMP_SeqBlock *last_block;

    if( reader )
    {
        reader->seq = 0;
        reader->block = 0;
        reader->ptr = reader->block_max = reader->block_min = 0;
    }
 

    if( !seq || !reader )
         printf( "Sting is Null Ptr\n" );

    reader->header_size = sizeof( IMP_SeqReader );
    reader->seq = (IMP_Seq*)seq;

    first_block = seq->first;

    if( first_block )
    {
        last_block = first_block->prev;
        reader->ptr = first_block->data;
        reader->prev_elem = IMP_GET_LAST_ELEM( seq, last_block );
        reader->delta_index = seq->first->start_index;

        if( reverse )
        {
            char *temp = reader->ptr;

            reader->ptr = reader->prev_elem;
            reader->prev_elem = temp;

            reader->block = last_block;
        }
        else
        {
            reader->block = first_block;
        }

        reader->block_min = reader->block->data;
        reader->block_max = reader->block_min + reader->block->count * seq->elem_size;
    }
    else
    {
        reader->delta_index = 0;
        reader->block = 0;

        reader->ptr = reader->prev_elem = reader->block_min = reader->block_max = 0;
    }

    
}


/* changes the current reading block to the previous or to the next */
void IMP_ChangeSeqBlock( void* _reader, int direction )
{
    IMP_SeqReader* reader = (IMP_SeqReader*)_reader;
    
    if( !reader )
         printf( "Sting is Null Ptr\n" );

    if( direction > 0 )
    {
        reader->block = reader->block->next;
        reader->ptr = reader->block->data;
    }
    else
    {
        reader->block = reader->block->prev;
        reader->ptr = IMP_GET_LAST_ELEM( reader->seq, reader->block );
    }
    reader->block_min = reader->block->data;
    reader->block_max = reader->block_min + reader->block->count * reader->seq->elem_size;
  
}


/* returns the current reader position */
int IMP_GetSeqReaderPos( IMP_SeqReader* reader )
{
    int elem_size;
    int index = -1;  

    if( !reader || !reader->ptr )
         printf( "Sting is Null Ptr\n" );

    elem_size = reader->seq->elem_size;
    if( elem_size <= IIMP_SHIFT_TAB_MAX && (index = gaPower2ShiftTab[elem_size - 1]) >= 0 )
        index = (int)((reader->ptr - reader->block_min) >> index);
    else
        index = (int)((reader->ptr - reader->block_min) / elem_size);

    index += reader->block->start_index - reader->delta_index;

    

    return index;
}


/* sets reader position to given absolute or relative
   (relatively to the current one) position */
void IMP_SetSeqReaderPos( IMP_SeqReader* reader, int index, int is_relative )
{
    IMP_SeqBlock *block;
    int elem_size, count, total;

    if( !reader || !reader->seq )
         printf( "Sting is Null Ptr\n" );

    total = reader->seq->total;
    elem_size = reader->seq->elem_size;

    if( !is_relative )
    {
        if( index < 0 )
        {
            if( index < -total )
                printf("string is out of range\n");
            index += total;
        }
        else if( index >= total )
        {
            index -= total;
            if( index >= total )
                printf("string is out of range\n");
        }

        block = reader->seq->first;
        if( index >= (count = block->count) )
        {
            if( index + index <= total )
            {
                do
                {
                    block = block->next;
                    index -= count;
                }
                while( index >= (count = block->count) );
            }
            else
            {
                do
                {
                    block = block->prev;
                    total -= block->count;
                }
                while( index < total );
                index -= total;
            }
        }
        reader->ptr = block->data + index * elem_size;
        if( reader->block != block )
        {
            reader->block = block;
            reader->block_min = block->data;
            reader->block_max = block->data + block->count * elem_size;
        }
    }
    else
    {
        char* ptr = reader->ptr;
        index *= elem_size;
        block = reader->block;

        if( index > 0 )
        {
            while( ptr + index >= reader->block_max )
            {
                int delta = (int)(reader->block_max - ptr);
                index -= delta;
                reader->block = block = block->next;
                reader->block_min = ptr = block->data;
                reader->block_max = block->data + block->count*elem_size;
            }
            reader->ptr = ptr + index;
        }
        else
        {
            while( ptr + index < reader->block_min )
            {
                int delta = (int)(ptr - reader->block_min);
                index += delta;
                reader->block = block = block->prev;
                reader->block_min = block->data;
                reader->block_max = ptr = block->data + block->count*elem_size;
            }
            reader->ptr = ptr + index;
        }
    }
   
}
/****************************************************************************************\
*            Functions for manipulating memory storage - list of memory blocks           *
\****************************************************************************************/

/* initializes allocated storage */
static void
IMP_InitMemStorage( IMP_MemStorage* storage, int block_size )
{
   
    if( !storage )
        printf( "Sts Null Ptr\n" );

    if( block_size <= 0 )
        block_size = IMP_STORAGE_BLOCK_SIZE;

    block_size = IMP_Align( block_size, IMP_STRUCT_ALIGN );
    assert( sizeof(IMP_MemBlock) % IMP_STRUCT_ALIGN == 0 );

    memset( storage, 0, sizeof( *storage ));
    storage->signature = IMP_STORAGE_MAGIC_VAL;
    storage->block_size = block_size;

}


/* creates root memory storage */
IMP_MemStorage* IMP_CreateMemStorage( int block_size )
{
    IMP_MemStorage *storage = 0;

    storage = (IMP_MemStorage *)malloc( sizeof( IMP_MemStorage ));
    IMP_InitMemStorage( storage, block_size );

    return storage;
}


/* creates child memory storage */
IMP_MemStorage * IMP_CreateChildMemStorage( IMP_MemStorage * parent )
{
    IMP_MemStorage *storage = 0;
   
    if( !parent )
        printf( "Sting is Null Ptr\n" );

    storage = IMP_CreateMemStorage(parent->block_size);
    storage->parent = parent;

    return storage;
}


/* releases all blocks of the storage (or returns them to parent if any) */
static void
IMP_DestroyMemStorage( IMP_MemStorage* storage )
{
    int k = 0;

    IMP_MemBlock *block;
    IMP_MemBlock *dst_top = 0;

    if( !storage )
        printf( "Sting is Null Ptr\n" );

    if( storage->parent )
        dst_top = storage->parent->top;

    for( block = storage->bottom; block != 0; k++ )
    {
        IMP_MemBlock *temp = block;

        block = block->next;
        if( storage->parent )
        {
            if( dst_top )
            {
                temp->prev = dst_top;
                temp->next = dst_top->next;
                if( temp->next )
                    temp->next->prev = temp;
                dst_top = dst_top->next = temp;
            }
            else
            {
                dst_top = storage->parent->bottom = storage->parent->top = temp;
                temp->prev = temp->next = 0;
                storage->free_space = storage->block_size - sizeof( *temp );
            }
        }
        else
        {
            free( temp );
        }
    }

    storage->top = storage->bottom = 0;
    storage->free_space = 0;

}


/* releases memory storage */
void IMP_ReleaseMemStorage( IMP_MemStorage** storage )
{
    IMP_MemStorage *st;
   
    if( !storage )
         printf( "Sting is Null Ptr\n" );

    st = *storage;
    *storage = 0;

    if( st )
    {
        IMP_DestroyMemStorage( st );
        free( st );
    }

}


/* clears memory storage (returns blocks to the parent if any) */
void IMP_ClearMemStorage( IMP_MemStorage * storage )
{

    if( !storage )
         printf( "Sting is Null Ptr\n" );

    if( storage->parent )
    {
        IMP_DestroyMemStorage( storage );
    }
    else
    {
        storage->top = storage->bottom;
        storage->free_space = storage->bottom ? storage->block_size - sizeof(IMP_MemBlock) : 0;
    }
}

/* remembers memory storage position */
void IMP_SaveMemStoragePos( const IMP_MemStorage * storage, IMP_MemStoragePos * pos )
{

    if( !storage || !pos )
		printf( "Sting is Null Ptr\n" );
	
    pos->top = storage->top;
    pos->free_space = storage->free_space;    
}

/* restores memory storage position */
 void IMP_RestoreMemStoragePos( IMP_MemStorage * storage, IMP_MemStoragePos * pos )
{ 

    if( !storage || !pos )
         printf( "Sting is Null Ptr\n" );
    if( pos->free_space > storage->block_size )
        printf( "string is bad size\n" );

    /*
    // this breaks IMP_GoNextMemBlock, so comment it off for now
    if( storage->parent && (!pos->top || pos->top->next) )
    {
        IMP_MemBlock* save_bottom;
        if( !pos->top )
            save_bottom = 0;
        else
        {
            save_bottom = storage->bottom;
            storage->bottom = pos->top->next;
            pos->top->next = 0;
            storage->bottom->prev = 0;
        }
        IMP_DestroyMemStorage( storage );
        storage->bottom = save_bottom;
    }*/

    storage->top = pos->top;
    storage->free_space = pos->free_space;

    if( !storage->top )
    {
        storage->top = storage->bottom;
        storage->free_space = storage->top ? storage->block_size - sizeof(IMP_MemBlock) : 0;
    }   
}


/* moves stack pointer to next block.
   If no blocks, allocate new one and link it to the storage */
static void IMP_GoNextMemBlock( IMP_MemStorage * storage )
{
      
    if( !storage )
         printf( "Sting is Null Ptr\n" );

    if( !storage->top || !storage->top->next )
    {
        IMP_MemBlock *block;

        if( !(storage->parent) )
        {
            ( block = (IMP_MemBlock *)malloc( storage->block_size ));
        }
        else
        {
            IMP_MemStorage *parent = storage->parent;
            IMP_MemStoragePos parent_pos;

            IMP_SaveMemStoragePos( parent, &parent_pos );
            ( IMP_GoNextMemBlock( parent ));

            block = parent->top;
            IMP_RestoreMemStoragePos( parent, &parent_pos );

            if( block == parent->top )  /* the single allocated block */
            {
                assert( parent->bottom == block );
                parent->top = parent->bottom = 0;
                parent->free_space = 0;
            }
            else
            {
                /* cut the block from the parent's list of blocks */
                parent->top->next = block->next;
                if( block->next )
                    block->next->prev = parent->top;
            }
        }

        /* link block */
        block->next = 0;
        block->prev = storage->top;

        if( storage->top )
            storage->top->next = block;
        else
            storage->top = storage->bottom = block;
    }

    if( storage->top->next )
        storage->top = storage->top->next;
    storage->free_space = storage->block_size - sizeof(IMP_MemBlock);
    assert( storage->free_space % IMP_STRUCT_ALIGN == 0 );

    
}

/* Allocates continuous buffer of the specified size in the storage */
 void* IMP_MemStorageAlloc( IMP_MemStorage* storage, unsigned int size )
{
    char *ptr = 0;   

    if( !storage )
        printf( "NULL storage pointer" );

    if( size > INT_MAX )
        printf( "Too large memory block is requested" );

    assert( storage->free_space % IMP_STRUCT_ALIGN == 0 );

    if( (unsigned int)storage->free_space < size )
    {
        unsigned int max_free_space = IMP_AlignLeft(storage->block_size - sizeof(IMP_MemBlock), IMP_STRUCT_ALIGN);
        if( max_free_space < size )
            printf( "requested size is negative or too big" );

        ( IMP_GoNextMemBlock( storage ));
    }

    ptr = IMP_FREE_PTR(storage);
    assert( (unsigned int)ptr % IMP_STRUCT_ALIGN == 0 );
    storage->free_space = IMP_AlignLeft(storage->free_space - (int)size, IMP_STRUCT_ALIGN );

    return ptr;
}


IMP_String IMP_MemStorageAllocString( IMP_MemStorage* storage, const char* ptr, int len )
{
    IMP_String str;
    
    str.len = len >= 0 ? len : (int)strlen(ptr);
    ( str.ptr = (char*)IMP_MemStorageAlloc( storage, str.len + 1 ));
    memcpy( str.ptr, ptr, str.len );
    str.ptr[str.len] = '\0';

    return str;
}


/****************************************************************************************\
*                               Sequence implementation                                  *
\****************************************************************************************/

/* adjusts <delta_elems> field of sequence. It determines how much the sequence
   grows if there are no free space inside the sequence buffers */
void IMP_SetSeqBlockSize( IMP_Seq *seq, int delta_elements )
{
    int elem_size;
    int useful_block_size;

    if( !seq || !seq->storage )
         printf( "Sting is Null Ptr\n" );
    if( delta_elements < 0 )
        printf("string is out of range\n");

    useful_block_size = IMP_AlignLeft(seq->storage->block_size - sizeof(IMP_MemBlock) -
                                    sizeof(IMP_SeqBlock), IMP_STRUCT_ALIGN);
    elem_size = seq->elem_size;

    if( delta_elements == 0 )
    {
        delta_elements = (1 << 10) / elem_size;
        delta_elements = IMP_MAX( delta_elements, 1 );
    }
    if( delta_elements * elem_size > useful_block_size )
    {
        delta_elements = useful_block_size / elem_size;
        if( delta_elements == 0 )
            printf( "Storage block size is too small,to fit the sequence elements\n" );
    }

    seq->delta_elems = delta_elements;

    
}

/* creates empty sequence */
 IMP_Seq* IMP_CreateSeq( int seq_flags, int header_size, int elem_size, IMP_MemStorage * storage )
{
    IMP_Seq *seq = 0;

    if( !storage )
         printf( "Sting is Null Ptr\n" );
    if( header_size < (int)sizeof( IMP_Seq ) || elem_size <= 0 )
        printf( "String is Bad Size\n" );

    /* allocate sequence header */
    ( seq = (IMP_Seq*)IMP_MemStorageAlloc( storage, header_size ));
    memset( seq, 0, header_size );

    seq->header_size = header_size;
    seq->flags = (seq_flags & ~IMP_MAGIC_MASK) | IMP_SEQ_MAGIC_VAL;
    {
        int elemtype = IMP_MAT_TYPE(seq_flags);
        int typesize = IMP_ELEM_SIZE(elemtype);

        if( elemtype != IMP_SEQ_ELTYPE_GENERIC &&
            typesize != 0 && typesize != elem_size )
            printf( "Specified element size doesn't match to the size of the specified element type,try to use 0 for element type\n" );
    }
    seq->elem_size = elem_size;
    seq->storage = storage;

    ( IMP_SetSeqBlockSize( seq, (1 << 10)/elem_size ));

    return seq;
}





/* finds sequence element by its index */
char* IMP_GetSeqElem( const IMP_Seq *seq, int index )
{
    IMP_SeqBlock *block;
    int count, total = seq->total;

    if( (unsigned)index >= (unsigned)total )
    {
        index += index < 0 ? total : 0;
        index -= index >= total ? total : 0;
        if( (unsigned)index >= (unsigned)total )
            return 0;
    }

    block = seq->first;
    if( index + index <= total )
    {
        while( index >= (count = block->count) )
        {
            block = block->next;
            index -= count;
        }
    }
    else
    {
        do
        {
            block = block->prev;
            total -= block->count;
        }
        while( index < total );
        index -= total;
    }

    return block->data + index * seq->elem_size;
}


/* calculates index of sequence element */
int IMP_SeqElemIdx( const IMP_Seq* seq, const void* _element, IMP_SeqBlock** _block )
{
    const char *element = (const char *)_element;
    int elem_size;
    int id = -1;
    IMP_SeqBlock *first_block;
    IMP_SeqBlock *block;

    if( !seq || !element )
         printf( "Sting is Null Ptr\n" );

    block = first_block = seq->first;
    elem_size = seq->elem_size;

    for( ;; )
    {
        if( (unsigned)(element - block->data) < (unsigned) (block->count * elem_size) )
        {
            if( _block )
                *_block = block;
            if( elem_size <= IIMP_SHIFT_TAB_MAX && (id = gaPower2ShiftTab[elem_size - 1]) >= 0 )
                id = (int)((unsigned int)(element - block->data) >> id);
            else
                id = (int)((unsigned int)(element - block->data) / elem_size);
            id += block->start_index - seq->first->start_index;
            break;
        }
        block = block->next;
        if( block == first_block )
            break;
    }

   return id;
}


int IMP_SliceLength( IMP_Slice slice, const IMP_Seq* seq )
{
    int total = seq->total;
    int length = slice.end_index - slice.start_index;
    
    if( length != 0 )
    {
        if( slice.start_index < 0 )
            slice.start_index += total;
        if( slice.end_index <= 0 )
            slice.end_index += total;

        length = slice.end_index - slice.start_index;
    }

    if( length < 0 )
    {
        length += total;
        /*if( length < 0 )
            length += total;*/
    }
    else if( length > total )
        length = total;

    return length;
}


/* copies all the sequence elements into single continuous array */
void* IMP_CvtSeqToArray( const IMP_Seq *seq, void *array, IMP_Slice slice )
{
   
    int elem_size, total;
    IMP_SeqReader reader;
    char *dst = (char*)array;

    if( !seq || !array )
         printf( "Sting is Null Ptr\n" );

    elem_size = seq->elem_size;
    total = IMP_SliceLength( slice, seq )*elem_size;
    
    if( total == 0 )
	{
		printf( "total == 0 \n" );
        exit(0);
	}
    
    IMP_StartReadSeq( seq, &reader, 0 );
    ( IMP_SetSeqReaderPos( &reader, slice.start_index, 0 ));

    do
    {
        int count = (int)(reader.block_max - reader.ptr);
        if( count > total )
            count = total;

        memcpy( dst, reader.ptr, count );
        dst += count;
        reader.block = reader.block->next;
        reader.ptr = reader.block->data;
        reader.block_max = reader.ptr + reader.block->count*elem_size;
        total -= count;
    }
    while( total > 0 );

    return array;
}


/* constructs sequence from array without copying any data.
   the resultant sequence can't grow above its initial size */
 IMP_Seq* IMP_MakeSeqHeaderForArray( int seq_flags, int header_size, int elem_size,
                         void *array, int total, IMP_Seq *seq, IMP_SeqBlock * block )
{
    IMP_Seq* result = 0;

    if( elem_size <= 0 || header_size < (int)sizeof( IMP_Seq ) || total < 0 )
        printf("string is bad size\n");

    if( !seq || ((!array || !block) && total > 0) )
         printf( "Sting is Null Ptr\n" );

    memset( seq, 0, header_size );

    seq->header_size = header_size;
    seq->flags = (seq_flags & ~IMP_MAGIC_MASK) | IMP_SEQ_MAGIC_VAL;
    {
        int elemtype = IMP_MAT_TYPE(seq_flags);
        int typesize = IMP_ELEM_SIZE(elemtype);

        if( elemtype != IMP_SEQ_ELTYPE_GENERIC &&
            typesize != 0 && typesize != elem_size )
            printf( "Element size doesn't match to the size of predefined element type, try to use 0 for sequence element type)" );
    }
    seq->elem_size = elem_size;
    seq->total = total;
    seq->block_max = seq->ptr = (char *) array + total * elem_size;

    if( total > 0 )
    {
        seq->first = block;
        block->prev = block->next = block;
        block->start_index = 0;
        block->count = total;
        block->data = (char *) array;
    }

    result = seq;

    return result;
}


/* the function allocates space for at least one more sequence element.
   if there are free sequence blocks (seq->free_blocks != 0),
   they are reused, otherwise the space is allocated in the storage */
static void IMP_GrowSeq( IMP_Seq *seq, int in_front_of )
{
    IMP_SeqBlock *block;

    if( !seq )
         printf( "Sting is Null Ptr\n" );
    block = seq->free_blocks;

    if( !block )
    {
        int elem_size = seq->elem_size;
        int delta_elems = seq->delta_elems;
        IMP_MemStorage *storage = seq->storage;

        if( seq->total >= delta_elems*4 )
            IMP_SetSeqBlockSize( seq, delta_elems*2 );

        if( !storage )
            printf("The sequence has NULL storage pointer\n");

        /* if there is a free space just after last allocated block
           and it's big enough then enlarge the last block
           (this can happen only if the new block is added to the end of sequence */
        if( (unsigned)(IMP_FREE_PTR(storage) - seq->block_max) < IMP_STRUCT_ALIGN &&
            storage->free_space >= seq->elem_size && !in_front_of )
        {
            int delta = storage->free_space / elem_size;

            delta = IMP_MIN( delta, delta_elems ) * elem_size;
            seq->block_max += delta;
            storage->free_space = IMP_AlignLeft((int)(((char*)storage->top + storage->block_size) -
                                              seq->block_max), IMP_STRUCT_ALIGN );
            exit(0);
        }
        else
        {
            int delta = elem_size * delta_elems + IIMP_ALIGNED_SEQ_BLOCK_SIZE;

            /* try to allocate <delta_elements> elements */
            if( storage->free_space < delta )
            {
                int small_block_size = IMP_MAX(1, delta_elems/3)*elem_size +
                                       IIMP_ALIGNED_SEQ_BLOCK_SIZE;
                /* try to allocate smaller part */
                if( storage->free_space >= small_block_size + IMP_STRUCT_ALIGN )
                {
                    delta = (storage->free_space - IIMP_ALIGNED_SEQ_BLOCK_SIZE)/seq->elem_size;
                    delta = delta*seq->elem_size + IIMP_ALIGNED_SEQ_BLOCK_SIZE;
                }
                else
                {
                    ( IMP_GoNextMemBlock( storage ));
                    assert( storage->free_space >= delta );
                }
            }

            ( block = (IMP_SeqBlock*)IMP_MemStorageAlloc( storage, delta ));
            block->data = (char*)IMP_AlignPtr( block + 1, IMP_STRUCT_ALIGN );
            block->count = delta - IIMP_ALIGNED_SEQ_BLOCK_SIZE;
            block->prev = block->next = 0;
        }
    }
    else
    {
        seq->free_blocks = block->next;
    }

    if( !(seq->first) )
    {
        seq->first = block;
        block->prev = block->next = block;
    }
    else
    {
        block->prev = seq->first->prev;
        block->next = seq->first;
        block->prev->next = block->next->prev = block;
    }

    /* for free blocks the <count> field means total number of bytes in the block.
       And for used blocks it means a current number of sequence
       elements in the block */
    assert( block->count % seq->elem_size == 0 && block->count > 0 );

    if( !in_front_of )
    {
        seq->ptr = block->data;
        seq->block_max = block->data + block->count;
        block->start_index = block == block->prev ? 0 :
            block->prev->start_index + block->prev->count;
    }
    else
    {
        int delta = block->count / seq->elem_size;
        block->data += block->count;

        if( block != block->prev )
        {
            assert( seq->first->start_index == 0 );
            seq->first = block;
        }
        else
        {
            seq->block_max = seq->ptr = block->data;
        }

        block->start_index = 0;

        for( ;; )
        {
            block->start_index += delta;
            block = block->next;
            if( block == seq->first )
                break;
        }
    }

    block->count = 0;
}

/* recycles a sequence block for the further use */
static void IMP_FreeSeqBlock( IMP_Seq *seq, int in_front_of )
{
   
    IMP_SeqBlock *block = seq->first;

    assert( (in_front_of ? block : block->prev)->count == 0 );

    if( block == block->prev )  /* single block case */
    {
        block->count = (int)(seq->block_max - block->data) + block->start_index * seq->elem_size;
        block->data = seq->block_max - block->count;
        seq->first = 0;
        seq->ptr = seq->block_max = 0;
        seq->total = 0;
    }
    else
    {
        if( !in_front_of )
        {
            block = block->prev;
            assert( seq->ptr == block->data );

            block->count = (int)(seq->block_max - seq->ptr);
            seq->block_max = seq->ptr = block->prev->data +
                block->prev->count * seq->elem_size;
        }
        else
        {
            int delta = block->start_index;

            block->count = delta * seq->elem_size;
            block->data -= block->count;

            /* update start indices of sequence blocks */
            for( ;; )
            {
                block->start_index -= delta;
                block = block->next;
                if( block == seq->first )
                    break;
            }

            seq->first = block->next;
        }

        block->prev->next = block->next;
        block->next->prev = block->prev;
    }

    assert( block->count > 0 && block->count % seq->elem_size == 0 );
    block->next = seq->free_blocks;
    seq->free_blocks = block;
}


/****************************************************************************************\
*                             Sequence Writer implementation                             *
\****************************************************************************************/

/* initializes sequence writer */
void IMP_StartAppendToSeq( IMP_Seq *seq, IMP_SeqWriter * writer )
{    
    if( !seq || !writer )
         printf( "Sting is Null Ptr\n" );

    memset( writer, 0, sizeof( *writer ));
    writer->header_size = sizeof( IMP_SeqWriter );

    writer->seq = seq;
    writer->block = seq->first ? seq->first->prev : 0;
    writer->ptr = seq->ptr;
    writer->block_max = seq->block_max;
    
}


/* initializes sequence writer */
void IMP_StartWriteSeq( int seq_flags, int header_size,
                 int elem_size, IMP_MemStorage * storage, IMP_SeqWriter * writer )
{
    IMP_Seq *seq = 0;
   
    if( !storage || !writer )
         printf( "Sting is Null Ptr\n" );

    ( seq = IMP_CreateSeq( seq_flags, header_size, elem_size, storage ));
    IMP_StartAppendToSeq( seq, writer );
}


/* updates sequence header */
void IMP_FlushSeqWriter( IMP_SeqWriter * writer )
{
    IMP_Seq *seq = 0;
   
    if( !writer )
         printf( "Sting is Null Ptr\n" );

    seq = writer->seq;
    seq->ptr = writer->ptr;

    if( writer->block )
    {
        int total = 0;
        IMP_SeqBlock *first_block = writer->seq->first;
        IMP_SeqBlock *block = first_block;

        writer->block->count = (int)((writer->ptr - writer->block->data) / seq->elem_size);
        assert( writer->block->count > 0 );

        do
        {
            total += block->count;
            block = block->next;
        }
        while( block != first_block );

        writer->seq->total = total;
    }    
}


/* calls IMP_FlushSeqWriter and finishes writing process */
IMP_Seq * IMP_EndWriteSeq( IMP_SeqWriter * writer )
{
    IMP_Seq *seq = 0;   

    if( !writer )
         printf( "Sting is Null Ptr\n" );

    ( IMP_FlushSeqWriter( writer ));
    seq = writer->seq;

    /* truncate the last block */
    if( writer->block && writer->seq->storage )
    {
        IMP_MemStorage *storage = seq->storage;
        char *storage_block_max = (char *) storage->top + storage->block_size;

        assert( writer->block->count > 0 );

        if( (unsigned)((storage_block_max - storage->free_space)
            - seq->block_max) < IMP_STRUCT_ALIGN )
        {
            storage->free_space = IMP_AlignLeft((int)(storage_block_max - seq->ptr), IMP_STRUCT_ALIGN);
            seq->block_max = seq->ptr;
        }
    }

    writer->ptr = 0; 

    return seq;
}


/* creates new sequence block */
 void IMP_CreateSeqBlock( IMP_SeqWriter * writer )
{
   IMP_Seq *seq;

    if( !writer || !writer->seq )
         printf( "Sting is Null Ptr\n" );

    seq = writer->seq;

    IMP_FlushSeqWriter( writer );

    ( IMP_GrowSeq( seq, 0 ));

    writer->block = seq->first->prev;
    writer->ptr = seq->ptr;
    writer->block_max = seq->block_max;
}

/* pushes element to the sequence */
char* IMP_SeqPush( IMP_Seq *seq, void *element )
{
    char *ptr = 0;
    unsigned int elem_size;
   
    if( !seq )
         printf( "Sting is Null Ptr\n" );

    elem_size = seq->elem_size;
    ptr = seq->ptr;

    if( ptr >= seq->block_max )
    {
        ( IMP_GrowSeq( seq, 0 ));

        ptr = seq->ptr;
        assert( ptr + elem_size <= seq->block_max /*&& ptr == seq->block_min */  );
    }

    if( element )
        IMP_MEMCPY_AUTO( ptr, element, elem_size );
    seq->first->prev->count++;
    seq->total++;
    seq->ptr = ptr + elem_size;   

    return ptr;
}


/* pops the last element out of the sequence */
void IMP_SeqPop( IMP_Seq *seq, void *element )
{
    char *ptr;
    int elem_size;

    if( !seq )
         printf( "Sting is Null Ptr\n" );
    if( seq->total <= 0 )
        printf("string is bad size\n");

    elem_size = seq->elem_size;
    seq->ptr = ptr = seq->ptr - elem_size;

    if( element )
        IMP_MEMCPY_AUTO( element, ptr, elem_size );
    seq->ptr = ptr;
    seq->total--;

    if( --(seq->first->prev->count) == 0 )
    {
        IMP_FreeSeqBlock( seq, 0 );
        assert( seq->ptr == seq->block_max );
    }    
}


/* pushes element to the front of the sequence */
char* IMP_SeqPushFront( IMP_Seq *seq, void *element )
{
    char* ptr = 0;
    int elem_size;
    IMP_SeqBlock *block;

    if( !seq )
         printf( "Sting is Null Ptr\n" );

    elem_size = seq->elem_size;
    block = seq->first;

    if( !block || block->start_index == 0 )
    {
        ( IMP_GrowSeq( seq, 1 ));

        block = seq->first;
        assert( block->start_index > 0 );
    }

    ptr = block->data -= elem_size;

    if( element )
        IMP_MEMCPY_AUTO( ptr, element, elem_size );
    block->count++;
    block->start_index--;
    seq->total++;

    return ptr;
}


/* pulls out the first element of the sequence */
void IMP_SeqPopFront( IMP_Seq *seq, void *element )
{
    int elem_size;
    IMP_SeqBlock *block;

    if( !seq )
         printf( "Sting is Null Ptr\n" );
    if( seq->total <= 0 )
        printf("string is bad size\n");

    elem_size = seq->elem_size;
    block = seq->first;

    if( element )
        IMP_MEMCPY_AUTO( element, block->data, elem_size );
    block->data += elem_size;
    block->start_index++;
    seq->total--;

    if( --(block->count) == 0 )
    {
        IMP_FreeSeqBlock( seq, 1 );
    }
}

/* inserts new element in the middle of the sequence */
char* IMP_SeqInsert( IMP_Seq *seq, int before_index, void *element )
{
    int elem_size;
    int block_size;
    IMP_SeqBlock *block;
    int delta_index;
    int total;
    char* ret_ptr = 0;

    if( !seq )
         printf( "Sting is Null Ptr\n" );

    total = seq->total;
    before_index += before_index < 0 ? total : 0;
    before_index -= before_index > total ? total : 0;

    if( (unsigned)before_index > (unsigned)total )
        printf("string is out of range\n");

    if( before_index == total )
    {
        ( ret_ptr = IMP_SeqPush( seq, element ));
    }
    else if( before_index == 0 )
    {
        ( ret_ptr = IMP_SeqPushFront( seq, element ));
    }
    else
    {
        elem_size = seq->elem_size;

        if( before_index >= total >> 1 )
        {
            char *ptr = seq->ptr + elem_size;

            if( ptr > seq->block_max )
            {
                ( IMP_GrowSeq( seq, 0 ));

                ptr = seq->ptr + elem_size;
                assert( ptr <= seq->block_max );
            }

            delta_index = seq->first->start_index;
            block = seq->first->prev;
            block->count++;
            block_size = (int)(ptr - block->data);

            while( before_index < block->start_index - delta_index )
            {
                IMP_SeqBlock *prev_block = block->prev;

                memmove( block->data + elem_size, block->data, block_size - elem_size );
                block_size = prev_block->count * elem_size;
                memcpy( block->data, prev_block->data + block_size - elem_size, elem_size );
                block = prev_block;

                /* check that we don't fall in the infinite loop */
                assert( block != seq->first->prev );
            }

            before_index = (before_index - block->start_index + delta_index) * elem_size;
            memmove( block->data + before_index + elem_size, block->data + before_index,
                     block_size - before_index - elem_size );

            ret_ptr = block->data + before_index;

            if( element )
                memcpy( ret_ptr, element, elem_size );
            seq->ptr = ptr;
        }
        else
        {
            block = seq->first;

            if( block->start_index == 0 )
            {
                ( IMP_GrowSeq( seq, 1 ));

                block = seq->first;
            }

            delta_index = block->start_index;
            block->count++;
            block->start_index--;
            block->data -= elem_size;

            while( before_index > block->start_index - delta_index + block->count )
            {
                IMP_SeqBlock *next_block = block->next;

                block_size = block->count * elem_size;
                memmove( block->data, block->data + elem_size, block_size - elem_size );
                memcpy( block->data + block_size - elem_size, next_block->data, elem_size );
                block = next_block;
                /* check that we don't fall in the infinite loop */
                assert( block != seq->first );
            }

            before_index = (before_index - block->start_index + delta_index) * elem_size;
            memmove( block->data, block->data + elem_size, before_index - elem_size );

            ret_ptr = block->data + before_index - elem_size;

            if( element )
                memcpy( ret_ptr, element, elem_size );
        }

        seq->total = total + 1;
    }

    return ret_ptr;
}


/* removes element from the sequence */
void IMP_SeqRemove( IMP_Seq *seq, int index )
{
    char *ptr;
    int elem_size;
    int block_size;
    IMP_SeqBlock *block;
    int delta_index;
    int total, front = 0;

    if( !seq )
         printf( "Sting is Null Ptr\n" );

    total = seq->total;

    index += index < 0 ? total : 0;
    index -= index >= total ? total : 0;

    if( (unsigned) index >= (unsigned) total )
        printf("Invalid index\n" );

    if( index == total - 1 )
    {
        IMP_SeqPop( seq, 0 );
    }
    else if( index == 0 )
    {
        IMP_SeqPopFront( seq, 0 );
    }
    else
    {
        block = seq->first;
        elem_size = seq->elem_size;
        delta_index = block->start_index;
        while( block->start_index - delta_index + block->count <= index )
            block = block->next;

        ptr = block->data + (index - block->start_index + delta_index) * elem_size;

        front = index < total >> 1;
        if( !front )
        {
            block_size = block->count * elem_size - (int)(ptr - block->data);

            while( block != seq->first->prev )  /* while not the last block */
            {
                IMP_SeqBlock *next_block = block->next;

                memmove( ptr, ptr + elem_size, block_size - elem_size );
                memcpy( ptr + block_size - elem_size, next_block->data, elem_size );
                block = next_block;
                ptr = block->data;
                block_size = block->count * elem_size;
            }

            memmove( ptr, ptr + elem_size, block_size - elem_size );
            seq->ptr -= elem_size;
        }
        else
        {
            ptr += elem_size;
            block_size = (int)(ptr - block->data);

            while( block != seq->first )
            {
                IMP_SeqBlock *prev_block = block->prev;

                memmove( block->data + elem_size, block->data, block_size - elem_size );
                block_size = prev_block->count * elem_size;
                memcpy( block->data, prev_block->data + block_size - elem_size, elem_size );
                block = prev_block;
            }

            memmove( block->data + elem_size, block->data, block_size - elem_size );
            block->data += elem_size;
            block->start_index++;
        }

        seq->total = total - 1;
        if( --block->count == 0 )
            IMP_FreeSeqBlock( seq, front );
    }    
}


/* adds several elements to the end or in the beginning of sequence */
void IMP_SeqPushMulti( IMP_Seq *seq, void *_elements, int count, int front )
{
    char *elements = (char *) _elements;   
    int elem_size;

    if( !seq )
        printf("NULL sequence pointer" );
    if( count < 0 )
        printf( "number of removed elements is negative" );

    elem_size = seq->elem_size;

    if( !front )
    {
        while( count > 0 )
        {
            int delta = (int)((seq->block_max - seq->ptr) / elem_size);

            delta = IMP_MIN( delta, count );
            if( delta > 0 )
            {
                seq->first->prev->count += delta;
                seq->total += delta;
                count -= delta;
                delta *= elem_size;
                if( elements )
                {
                    memcpy( seq->ptr, elements, delta );
                    elements += delta;
                }
                seq->ptr += delta;
            }

            if( count > 0 )
                ( IMP_GrowSeq( seq, 0 ));
        }
    }
    else
    {
        IMP_SeqBlock* block = seq->first;
        
        while( count > 0 )
        {
            int delta;
            
            if( !block || block->start_index == 0 )
            {
                ( IMP_GrowSeq( seq, 1 ));

                block = seq->first;
                assert( block->start_index > 0 );
            }

            delta = IMP_MIN( block->start_index, count );
            count -= delta;
            block->start_index -= delta;
            block->count += delta;
            seq->total += delta;
            delta *= elem_size;
            block->data -= delta;

            if( elements )
                memcpy( block->data, elements + count*elem_size, delta );
        }
    }    
}


/* removes several elements from the end of sequence */
void IMP_SeqPopMulti( IMP_Seq *seq, void *_elements, int count, int front )
{
    char *elements = (char *) _elements;

    if( !seq )
        printf("NULL sequence pointer" );
    if( count < 0 )
        printf( "number of removed elements is negative" );

    count = IMP_MIN( count, seq->total );

    if( !front )
    {
        if( elements )
            elements += count * seq->elem_size;

        while( count > 0 )
        {
            int delta = seq->first->prev->count;

            delta = IMP_MIN( delta, count );
            assert( delta > 0 );

            seq->first->prev->count -= delta;
            seq->total -= delta;
            count -= delta;
            delta *= seq->elem_size;
            seq->ptr -= delta;

            if( elements )
            {
                elements -= delta;
                memcpy( elements, seq->ptr, delta );
            }

            if( seq->first->prev->count == 0 )
                IMP_FreeSeqBlock( seq, 0 );
        }
    }
    else
    {
        while( count > 0 )
        {
            int delta = seq->first->count;

            delta = IMP_MIN( delta, count );
            assert( delta > 0 );

            seq->first->count -= delta;
            seq->total -= delta;
            count -= delta;
            seq->first->start_index += delta;
            delta *= seq->elem_size;

            if( elements )
            {
                memcpy( elements, seq->first->data, delta );
                elements += delta;
            }

            seq->first->data += delta;
            if( seq->first->count == 0 )
                IMP_FreeSeqBlock( seq, 1 );
        }
    }    
}


/* removes all elements from the sequence */
void IMP_ClearSeq( IMP_Seq *seq )
{  
    if( !seq )
         printf( "Sting is Null Ptr\n" );
    // IMP_SeqPopMulti( seq, 0, seq->total );   
	IMP_SeqPopMulti( seq, 0, seq->total,0 );   
}


IMP_Seq*
IMP_SeqSlice( const IMP_Seq* seq, IMP_Slice slice, IMP_MemStorage* storage, int copy_data )
{
    IMP_Seq* subseq = 0;
    
    int elem_size, count, length;
    IMP_SeqReader reader;
    IMP_SeqBlock *block, *first_block = 0, *last_block = 0;
    
    if( !IMP_IS_SEQ(seq) )
        printf("Invalid sequence header\n" );

    if( !storage )
    {
        storage = seq->storage;
        if( !storage )
            printf("NULL storage pointer" );
    }

    elem_size = seq->elem_size;
    length = IMP_SliceLength( slice, seq );
    if( slice.start_index < 0 )
        slice.start_index += seq->total;
    else if( slice.start_index >= seq->total )
        slice.start_index -= seq->total;
    if( (unsigned)length > (unsigned)seq->total ||
        ((unsigned)slice.start_index >= (unsigned)seq->total && length != 0) )
        printf("Bad sequence slice" );

    ( subseq = IMP_CreateSeq( seq->flags, seq->header_size, elem_size, storage ));

    if( length > 0 )
    {
        IMP_StartReadSeq( seq, &reader, 0 );
        IMP_SetSeqReaderPos( &reader, slice.start_index, 0 );
        count = (int)((reader.block_max - reader.ptr)/elem_size);

        do
        {
            int bl = IMP_MIN( count, length );
            
            if( !copy_data )
            {
                block = (IMP_SeqBlock*)IMP_MemStorageAlloc( storage, sizeof(*block) );
                if( !first_block )
                {
                    first_block = subseq->first = block->prev = block->next = block;
                    block->start_index = 0;
                }
                else
                {
                    block->prev = last_block;
                    block->next = first_block;
                    last_block->next = first_block->prev = block;
                    block->start_index = last_block->start_index + last_block->count;
                }
                last_block = block;
                block->data = reader.ptr;
                block->count = bl;
                subseq->total += bl;
            }
            else
                IMP_SeqPushMulti( subseq, reader.ptr, bl, 0 );
            length -= bl;
            reader.block = reader.block->next;
            reader.ptr = reader.block->data;
            count = reader.block->count;
        }
        while( length > 0 );
    }
    
    return subseq;
}


// Remove slice from the middle of the sequence
// !!! TODO !!! Implement more efficient algorithm
 void
IMP_SeqRemoveSlice( IMP_Seq* seq, IMP_Slice slice )
{    
    int total, length;

    if( !IMP_IS_SEQ(seq) )
        printf("Invalid sequence header\n" );

    length = IMP_SliceLength( slice, seq );
    total = seq->total;

    if( slice.start_index < 0 )
        slice.start_index += total;
    else if( slice.start_index >= total )
        slice.start_index -= total;

    if( (unsigned)slice.start_index >= (unsigned)total )
        printf("start slice index is out of range\n" );

    slice.end_index = slice.start_index + length;

    if( slice.end_index < total )
    {
        IMP_SeqReader reader_to, reader_from;
        int elem_size = seq->elem_size;

        IMP_StartReadSeq( seq, &reader_to, 0 );
        IMP_StartReadSeq( seq, &reader_from, 0 );

        if( slice.start_index > total - slice.end_index )
        {
            int i, count = seq->total - slice.end_index;
            IMP_SetSeqReaderPos( &reader_to, slice.start_index, 0 );
            IMP_SetSeqReaderPos( &reader_from, slice.end_index, 0 );

            for( i = 0; i < count; i++ )
            {
                IMP_MEMCPY_AUTO( reader_to.ptr, reader_from.ptr, elem_size );
                IMP_NEXT_SEQ_ELEM( elem_size, reader_to );
                IMP_NEXT_SEQ_ELEM( elem_size, reader_from );
            }

            IMP_SeqPopMulti( seq, 0, slice.end_index - slice.start_index, 0 );
        }
        else
        {
            int i, count = slice.start_index;
            IMP_SetSeqReaderPos( &reader_to, slice.end_index, 0 );
            IMP_SetSeqReaderPos( &reader_from, slice.start_index, 0 );

            for( i = 0; i < count; i++ )
            {
                IMP_PREV_SEQ_ELEM( elem_size, reader_to );
                IMP_PREV_SEQ_ELEM( elem_size, reader_from );

                IMP_MEMCPY_AUTO( reader_to.ptr, reader_from.ptr, elem_size );
            }

            IMP_SeqPopMulti( seq, 0, slice.end_index - slice.start_index, 1 );
        }
    }
    else
    {
        IMP_SeqPopMulti( seq, 0, total - slice.start_index, 0 );
        IMP_SeqPopMulti( seq, 0, slice.end_index - total, 1 );
    }
}


// Inserts a new sequence into the middle of another sequence
// !!! TODO !!! Implement more efficient algorithm
void IMP_SeqInsertSlice( IMP_Seq* seq, int index, const IMP_Mat* from_arr )
{
    IMP_SeqReader reader_to, reader_from;
    int i, elem_size, total, from_total;
  
    IMP_Seq from_header, *from = (IMP_Seq*)from_arr;
    IMP_SeqBlock block;

    if( !IMP_IS_SEQ(seq) )
        printf("Invalid destination sequence header" );

    if( !IMP_IS_SEQ(from))
    {
        IMP_Mat* mat = (IMP_Mat*)from;
        if( !IMP_IS_MAT(mat))
            printf("Source is not a sequence nor matrix" );

        if( !IMP_IS_MAT_CONT(mat->type) || (mat->rows != 1 && mat->cols != 1) )
            printf("The source array must be 1d coninuous vector" );

        ( from = IMP_MakeSeqHeaderForArray( IMP_SEQ_KIND_GENERIC, sizeof(from_header),
                                                 IMP_ELEM_SIZE(mat->type),
                                                 mat->data.ptr, mat->cols + mat->rows - 1,
                                                 &from_header, &block ));
    }

    if( seq->elem_size != from->elem_size )
        printf( "Sizes of source and destination sequences, elements are different\n" );

    from_total = from->total;

    if( from_total == 0 )
        exit(0);

    total = seq->total;
    index += index < 0 ? total : 0;
    index -= index > total ? total : 0;

    if( (unsigned)index > (unsigned)total )
        printf("string is out of range\n");

    elem_size = seq->elem_size;

    if( index < (total >> 1) )
    {
        IMP_SeqPushMulti( seq, 0, from_total, 1 );

        IMP_StartReadSeq( seq, &reader_to, 0 );
        IMP_StartReadSeq( seq, &reader_from,0 );
        IMP_SetSeqReaderPos( &reader_from, from_total, 0 );

        for( i = 0; i < index; i++ )
        {
            IMP_MEMCPY_AUTO( reader_to.ptr, reader_from.ptr, elem_size );
            IMP_NEXT_SEQ_ELEM( elem_size, reader_to );
            IMP_NEXT_SEQ_ELEM( elem_size, reader_from );
        }
    }
    else
    {
        IMP_SeqPushMulti( seq, 0, from_total, 0 );

        IMP_StartReadSeq( seq, &reader_to, 0 );
        IMP_StartReadSeq( seq, &reader_from, 0 );
        IMP_SetSeqReaderPos( &reader_from, total, 0 );
        IMP_SetSeqReaderPos( &reader_to, seq->total, 0 );

        for( i = 0; i < total - index; i++ )
        {
            IMP_PREV_SEQ_ELEM( elem_size, reader_to );
            IMP_PREV_SEQ_ELEM( elem_size, reader_from );
            IMP_MEMCPY_AUTO( reader_to.ptr, reader_from.ptr, elem_size );
        }
    }

    IMP_StartReadSeq( from, &reader_from, 0 );
    IMP_SetSeqReaderPos( &reader_to, index, 0 );

    for( i = 0; i < from_total; i++ )
    {
        IMP_MEMCPY_AUTO( reader_to.ptr, reader_from.ptr, elem_size );
        IMP_NEXT_SEQ_ELEM( elem_size, reader_to );
        IMP_NEXT_SEQ_ELEM( elem_size, reader_from );
    }    
}



typedef struct IMP_SeqReaderPos
{
    IMP_SeqBlock* block;
    char* ptr;
    char* block_min;
    char* block_max;
}
IMP_SeqReaderPos;

#define IMP_SAVE_READER_POS( reader, pos )   \
{                                           \
    (pos).block = (reader).block;           \
    (pos).ptr = (reader).ptr;               \
    (pos).block_min = (reader).block_min;   \
    (pos).block_max = (reader).block_max;   \
}

#define IMP_RESTORE_READER_POS( reader, pos )\
{                                           \
    (reader).block = (pos).block;           \
    (reader).ptr = (pos).ptr;               \
    (reader).block_min = (pos).block_min;   \
    (reader).block_max = (pos).block_max;   \
}

char* IMP_Med3( char* a, char* b, char* c, IMP_CmpFunc cmp_func, void* aux )
{
    return cmp_func(a, b, aux) < 0 ?
      (cmp_func(b, c, aux) < 0 ? b : cmp_func(a, c, aux) < 0 ? c : a)
     :(cmp_func(b, c, aux) > 0 ? b : cmp_func(a, c, aux) < 0 ? a : c);
}

void IMP_SeqSort( IMP_Seq* seq, IMP_CmpFunc cmp_func, void* aux )
{         
    int elem_size;
    int isort_thresh = 7;
    IMP_SeqReader left, right;
    int sp = 0;

    struct
    {
        IMP_SeqReaderPos lb;
        IMP_SeqReaderPos ub;
    }
    stack[48];
    
    if( !IMP_IS_SEQ(seq) )
        printf( "Bad input sequence\n" );

    if( !cmp_func )
        printf( "Null compare function\n" );

    if( seq->total <= 1 )
        exit(0);

    elem_size = seq->elem_size;
    isort_thresh *= elem_size;

    IMP_StartReadSeq( seq, &left, 0 );
    right = left;
    IMP_SAVE_READER_POS( left, stack[0].lb );
    IMP_PREV_SEQ_ELEM( elem_size, right );
    IMP_SAVE_READER_POS( right, stack[0].ub );

    while( sp >= 0 )
    {
        IMP_RESTORE_READER_POS( left, stack[sp].lb );
        IMP_RESTORE_READER_POS( right, stack[sp].ub );
        sp--;

        for(;;)
        {
            int i, n, m;
            IMP_SeqReader ptr, ptr2;

            if( left.block == right.block )
                n = (int)(right.ptr - left.ptr) + elem_size;
            else
            {
                n = IMP_GetSeqReaderPos( &right );
                n = (n - IMP_GetSeqReaderPos( &left ) + 1)*elem_size;
            }

            if( n <= isort_thresh )
            {
            insert_sort:
                ptr = ptr2 = left;
                IMP_NEXT_SEQ_ELEM( elem_size, ptr );
                IMP_NEXT_SEQ_ELEM( elem_size, right );
                while( ptr.ptr != right.ptr )
                {
                    ptr2.ptr = ptr.ptr;
                    if( ptr2.block != ptr.block )
                    {
                        ptr2.block = ptr.block;
                        ptr2.block_min = ptr.block_min;
                        ptr2.block_max = ptr.block_max;
                    }
                    while( ptr2.ptr != left.ptr )
                    {
                        char* cur = ptr2.ptr;
                        IMP_PREV_SEQ_ELEM( elem_size, ptr2 );
                        if( cmp_func( ptr2.ptr, cur, aux ) <= 0 )
                            break;
                        IMP_SWAP_ELEMS( ptr2.ptr, cur, elem_size );
                    }
                    IMP_NEXT_SEQ_ELEM( elem_size, ptr );
                }
                break;
            }
            else
            {
                IMP_SeqReader left0, left1, right0, right1;
                IMP_SeqReader tmp0, tmp1;
                char *m1, *m2, *m3, *pivot;
                int swap_cnt = 0;
                int l, l0, l1, r, r0, r1;

                left0 = tmp0 = left;
                right0 = right1 = right;
                n /= elem_size;
                
                if( n > 40 )
                {
                    int d = n / 8;
                    char *p1, *p2, *p3;
                    p1 = tmp0.ptr;
                    IMP_SetSeqReaderPos( &tmp0, d, 1 );
                    p2 = tmp0.ptr;
                    IMP_SetSeqReaderPos( &tmp0, d, 1 );
                    p3 = tmp0.ptr;
                    m1 = IMP_Med3( p1, p2, p3, cmp_func, aux );
                    IMP_SetSeqReaderPos( &tmp0, (n/2) - d*3, 1 );
                    p1 = tmp0.ptr;
                    IMP_SetSeqReaderPos( &tmp0, d, 1 );
                    p2 = tmp0.ptr;
                    IMP_SetSeqReaderPos( &tmp0, d, 1 );
                    p3 = tmp0.ptr;
                    m2 = IMP_Med3( p1, p2, p3, cmp_func, aux );
                    IMP_SetSeqReaderPos( &tmp0, n - 1 - d*3 - n/2, 1 );
                    p1 = tmp0.ptr;
                    IMP_SetSeqReaderPos( &tmp0, d, 1 );
                    p2 = tmp0.ptr;
                    IMP_SetSeqReaderPos( &tmp0, d, 1 );
                    p3 = tmp0.ptr;
                    m3 = IMP_Med3( p1, p2, p3, cmp_func, aux );
                }
                else
                {
                    m1 = tmp0.ptr;
                    IMP_SetSeqReaderPos( &tmp0, n/2, 1 );
                    m2 = tmp0.ptr;
                    IMP_SetSeqReaderPos( &tmp0, n - 1 - n/2, 1 );
                    m3 = tmp0.ptr;
                }

                pivot = IMP_Med3( m1, m2, m3, cmp_func, aux );
                left = left0;
                if( pivot != left.ptr )
                {
                    IMP_SWAP_ELEMS( pivot, left.ptr, elem_size );
                    pivot = left.ptr;
                }
                IMP_NEXT_SEQ_ELEM( elem_size, left );
                left1 = left;

                for(;;)
                {
                    while( left.ptr != right.ptr && (r = cmp_func(left.ptr, pivot, aux)) <= 0 )
                    {
                        if( r == 0 )
                        {
                            if( left1.ptr != left.ptr )
                                IMP_SWAP_ELEMS( left1.ptr, left.ptr, elem_size );
                            swap_cnt = 1;
                            IMP_NEXT_SEQ_ELEM( elem_size, left1 );
                        }
                        IMP_NEXT_SEQ_ELEM( elem_size, left );
                    }

                    while( left.ptr != right.ptr && (r = cmp_func(right.ptr,pivot, aux)) >= 0 )
                    {
                        if( r == 0 )
                        {
                            if( right1.ptr != right.ptr )
                                IMP_SWAP_ELEMS( right1.ptr, right.ptr, elem_size );
                            swap_cnt = 1;
                            IMP_PREV_SEQ_ELEM( elem_size, right1 );
                        }
                        IMP_PREV_SEQ_ELEM( elem_size, right );
                    }

                    if( left.ptr == right.ptr )
                    {
                        r = cmp_func(left.ptr, pivot, aux);
                        if( r == 0 )
                        {
                            if( left1.ptr != left.ptr )
                                IMP_SWAP_ELEMS( left1.ptr, left.ptr, elem_size );
                            swap_cnt = 1;
                            IMP_NEXT_SEQ_ELEM( elem_size, left1 );
                        }
                        if( r <= 0 )
                        {
                            IMP_NEXT_SEQ_ELEM( elem_size, left );
                        }
                        else
                        {
                            IMP_PREV_SEQ_ELEM( elem_size, right );
                        }
                        break;
                    }

                    IMP_SWAP_ELEMS( left.ptr, right.ptr, elem_size );
                    IMP_NEXT_SEQ_ELEM( elem_size, left );
                    r = left.ptr == right.ptr;
                    IMP_PREV_SEQ_ELEM( elem_size, right );
                    swap_cnt = 1;
                    if( r )
                        break;
                }

                if( swap_cnt == 0 )
                {
                    left = left0, right = right0;
                    goto insert_sort;
                }

                l = IMP_GetSeqReaderPos( &left );
                if( l == 0 )
                    l = seq->total;
                l0 = IMP_GetSeqReaderPos( &left0 );
                l1 = IMP_GetSeqReaderPos( &left1 );
                if( l1 == 0 )
                    l1 = seq->total;
                
                n = IMP_MIN( l - l1, l1 - l0 );
                if( n > 0 )
                {
                    tmp0 = left0;
                    tmp1 = left;
                    IMP_SetSeqReaderPos( &tmp1, 0-n, 1 );
                    for( i = 0; i < n; i++ )
                    {
                        IMP_SWAP_ELEMS( tmp0.ptr, tmp1.ptr, elem_size );
                        IMP_NEXT_SEQ_ELEM( elem_size, tmp0 );
                        IMP_NEXT_SEQ_ELEM( elem_size, tmp1 );
                    }
                }

                r = IMP_GetSeqReaderPos( &right );
                r0 = IMP_GetSeqReaderPos( &right0 );
                r1 = IMP_GetSeqReaderPos( &right1 );
                m = IMP_MIN( r0 - r1, r1 - r );
                if( m > 0 )
                {
                    tmp0 = left;
                    tmp1 = right0;
                    IMP_SetSeqReaderPos( &tmp1, 1-m, 1 );
                    for( i = 0; i < m; i++ )
                    {
                        IMP_SWAP_ELEMS( tmp0.ptr, tmp1.ptr, elem_size );
                        IMP_NEXT_SEQ_ELEM( elem_size, tmp0 );
                        IMP_NEXT_SEQ_ELEM( elem_size, tmp1 );
                    }
                }

                n = l - l1;
                m = r1 - r;
                if( n > 1 )
                {
                    if( m > 1 )
                    {
                        if( n > m )
                        {
                            sp++;
                            IMP_SAVE_READER_POS( left0, stack[sp].lb );
                            IMP_SetSeqReaderPos( &left0, n - 1, 1 );
                            IMP_SAVE_READER_POS( left0, stack[sp].ub );
                            left = right = right0;
                            IMP_SetSeqReaderPos( &left, 1 - m, 1 );
                        }
                        else
                        {
                            sp++;
                            IMP_SAVE_READER_POS( right0, stack[sp].ub );
                            IMP_SetSeqReaderPos( &right0, 1 - m, 1 );
                            IMP_SAVE_READER_POS( right0, stack[sp].lb );
                            left = right = left0;
                            IMP_SetSeqReaderPos( &right, n - 1, 1 );
                        }
                    }
                    else
                    {
                        left = right = left0;
                        IMP_SetSeqReaderPos( &right, n - 1, 1 );
                    }
                }
                else if( m > 1 )
                {
                    left = right = right0;
                    IMP_SetSeqReaderPos( &left, 1 - m, 1 );
                }
                else
                    break;
            }
        }
    }    
}


 char*
IMP_SeqSearch( IMP_Seq* seq, const void* _elem, IMP_CmpFunc cmp_func,
             int is_sorted, int* _idx, void* userdata )
{
    char* result = 0;
    const char* elem = (const char*)_elem;
    int idx = -1;
    
   
    int elem_size, i, j, total;

    if( !IMP_IS_SEQ(seq) )
        printf( "Bad input sequence\n" );

    if( !elem )
        printf( "Null element pointer\n" );

    elem_size = seq->elem_size;
    total = seq->total;

    if( total == 0 )
        exit(0);

    if( !is_sorted )
    {
        IMP_SeqReader reader;
        IMP_StartReadSeq( seq, &reader, 0 );

        if( cmp_func )
        {
            for( i = 0; i < total; i++ )
            {
                if( cmp_func( elem, reader.ptr, userdata ) == 0 )
                    break;
                IMP_NEXT_SEQ_ELEM( elem_size, reader );
            }
        }
        else if( (elem_size & (sizeof(int)-1)) == 0 )
        {
            for( i = 0; i < total; i++ )
            {
                for( j = 0; j < elem_size; j += sizeof(int) )
                {
                    if( *(const int*)(reader.ptr + j) != *(const int*)(elem + j) )
                        break;
                }
                if( j == elem_size )
                    break;
                IMP_NEXT_SEQ_ELEM( elem_size, reader );
            }
        }
        else
        {
            for( i = 0; i < total; i++ )
            {
                for( j = 0; j < elem_size; j++ )
                {
                    if( reader.ptr[j] != elem[j] )
                        break;
                }
                if( j == elem_size )
                    break;
                IMP_NEXT_SEQ_ELEM( elem_size, reader );
            }
        }

        idx = i;
        if( i < total )
            result = reader.ptr;
    }
    else
    {
        if( !cmp_func )
            printf("Null compare function\n" );

        i = 0, j = total;
        
        while( j > i )
        {
            int k = (i+j)>>1, code;
            char* ptr = IMP_GetSeqElem( seq, k );
            code = cmp_func( elem, ptr, userdata );
            if( !code )
            {
                result = ptr;
                idx = k;
                exit(0);
            }
            if( code < 0 )
                j = k;
            else
                i = k+1;
        }
        idx = j;
    }
    

    if( _idx )
        *_idx = idx;

    return result;
}


 void
IMP_SeqInvert( IMP_Seq* seq )
{
    
    IMP_SeqReader left_reader, right_reader;
    int elem_size;
    int i, count;

    ( IMP_StartReadSeq( seq, &left_reader, 0 ));
    ( IMP_StartReadSeq( seq, &right_reader, 1 ));
    elem_size = seq->elem_size;
    count = seq->total >> 1;

    for( i = 0; i < count; i++ )
    {
        IMP_SWAP_ELEMS( left_reader.ptr, right_reader.ptr, elem_size );
        IMP_NEXT_SEQ_ELEM( elem_size, left_reader );
        IMP_PREV_SEQ_ELEM( elem_size, right_reader );
    }   
}


typedef struct IMP_PTreeNode
{
    struct IMP_PTreeNode* parent;
    char* element;
    int rank;
}
IMP_PTreeNode;


// the function splits the input sequence or set into one or more equivalence classes.
// is_equal(a,b,...) returns non-zero if the two sequence elements
// belong to the same class. the function returns sequence of integers -
// 0-based class indexes for each element.
//
// The algorithm is described in "Introduction to Algorithms"
// by Cormen, Leiserson and Rivest, chapter "Data structures for disjoint sets"
int IMP_SeqPartition( const IMP_Seq* seq, IMP_MemStorage* storage, IMP_Seq** labels,
                IMP_CmpFunc is_equal, void* userdata )
{
    IMP_Seq* result = 0;
    IMP_MemStorage* temp_storage = 0;
    int class_idx = 0;
    
    IMP_SeqWriter writer;
    IMP_SeqReader reader, reader0;
    IMP_Seq* nodes;
    int i, j;
    int is_set; 

    if( !labels )
         printf( "Sting is Null Ptr\n" );

    if( !seq || !is_equal )
         printf( "Sting is Null Ptr\n" );

    if( !storage )
        storage = seq->storage;

    if( !storage )
         printf( "Sting is Null Ptr\n" );

    is_set = IMP_IS_SET(seq);

    temp_storage = IMP_CreateChildMemStorage( storage );

    nodes = IMP_CreateSeq( 0, sizeof(IMP_Seq), sizeof(IMP_PTreeNode), temp_storage );

    IMP_StartReadSeq( seq, &reader,0 );
    memset( &writer, 0, sizeof(writer));
    IMP_StartAppendToSeq( nodes, &writer ); 

    // Initial O(N) pass. Make a forest of single-vertex trees.
    for( i = 0; i < seq->total; i++ )
    {
        IMP_PTreeNode node = { 0, 0, 0 };
        if( !is_set || IMP_IS_SET_ELEM( reader.ptr ))
            node.element = reader.ptr;
        IMP_WRITE_SEQ_ELEM( node, writer );
        IMP_NEXT_SEQ_ELEM( seq->elem_size, reader );
    }

    IMP_EndWriteSeq( &writer );

    // because in the next loop we will iterate
    // through all the sequence nodes each time,
    // we do not need to initialize reader every time
    IMP_StartReadSeq( nodes, &reader,0 );
    IMP_StartReadSeq( nodes, &reader0, 0 );

    // The main O(N^2) pass. Merge connected components.
    for( i = 0; i < nodes->total; i++ )
    {
        IMP_PTreeNode* node = (IMP_PTreeNode*)(reader0.ptr);
        IMP_PTreeNode* root = node;
        IMP_NEXT_SEQ_ELEM( nodes->elem_size, reader0 );

        if( !node->element )
            continue;

        // find root
        while( root->parent )
            root = root->parent;

        for( j = 0; j < nodes->total; j++ )
        {
            IMP_PTreeNode* node2 = (IMP_PTreeNode*)reader.ptr;
            
            if( node2->element && node2 != node &&
                is_equal( node->element, node2->element, userdata ))
            {
                IMP_PTreeNode* root2 = node2;
                
                // unite both trees
                while( root2->parent )
                    root2 = root2->parent;

                if( root2 != root )
                {
                    if( root->rank > root2->rank )
                        root2->parent = root;
                    else
                    {
                        root->parent = root2;
                        root2->rank += root->rank == root2->rank;
                        root = root2;
                    }
                    assert( root->parent == 0 );

                    // compress path from node2 to the root
                    while( node2->parent )
                    {
                        IMP_PTreeNode* temp = node2;
                        node2 = node2->parent;
                        temp->parent = root;
                    }

                    // compress path from node to the root
                    node2 = node;
                    while( node2->parent )
                    {
                        IMP_PTreeNode* temp = node2;
                        node2 = node2->parent;
                        temp->parent = root;
                    }
                }
            }

            IMP_NEXT_SEQ_ELEM( sizeof(*node), reader );
        }
    }

    // Final O(N) pass (Enumerate classes)
    // Reuse reader one more time
    result = IMP_CreateSeq( 0, sizeof(IMP_Seq), sizeof(int), storage );
    IMP_StartAppendToSeq( result, &writer );

    for( i = 0; i < nodes->total; i++ )
    {
        IMP_PTreeNode* node = (IMP_PTreeNode*)reader.ptr;
        int idx = -1;
        
        if( node->element )
        {
            while( node->parent )
                node = node->parent;
            if( node->rank >= 0 )
                node->rank = ~class_idx++;
            idx = ~node->rank;
        }

        IMP_NEXT_SEQ_ELEM( sizeof(*node), reader );
        IMP_WRITE_SEQ_ELEM( idx, writer );
    }

    IMP_EndWriteSeq( &writer );    

    if( labels )
        *labels = result;

    IMP_ReleaseMemStorage( &temp_storage );
    return class_idx;
}


/****************************************************************************************\
*                                      Set implementation                                *
\****************************************************************************************/

/* creates empty set */
IMP_SET_S* IMP_CreateSet( int set_flags, int header_size, int elem_size, IMP_MemStorage * storage )
{
    IMP_SET_S *set = 0;
   
    if( !storage )
         printf( "Sting is Null Ptr\n" );
    if( header_size < (int)sizeof( IMP_SET_S ) ||
        elem_size < (int)sizeof(void*)*2 ||
        (elem_size & (sizeof(void*)-1)) != 0 )
        printf("string is bad size\n");

    set = (IMP_SET_S*) IMP_CreateSeq( set_flags, header_size, elem_size, storage );
    set->flags = (set->flags & ~IMP_MAGIC_MASK) | IMP_SET_MAGIC_VAL;
    return set;
}


/* adds new element to the set */
int IMP_SetAdd( IMP_SET_S* set, IMP_SetElem* element, IMP_SetElem** inserted_element )
{
    int id = -1;
  
    IMP_SetElem *free_elem;

    if( !set )
         printf( "Sting is Null Ptr\n" );

    if( !(set->free_elems) )
    {
        int count = set->total;
        int elem_size = set->elem_size;
        char *ptr;
        ( IMP_GrowSeq( (IMP_Seq *) set, 0 ));

        set->free_elems = (IMP_SetElem*) (ptr = set->ptr);
        for( ; ptr + elem_size <= set->block_max; ptr += elem_size, count++ )
        {
            ((IMP_SetElem*)ptr)->flags = count | IMP_SET_ELEM_FREE_FLAG;
            ((IMP_SetElem*)ptr)->next_free = (IMP_SetElem*)(ptr + elem_size);
        }
        assert( count <= IMP_SET_ELEM_IDX_MASK+1 );
        ((IMP_SetElem*)(ptr - elem_size))->next_free = 0;
        set->first->prev->count += count - set->total;
        set->total = count;
        set->ptr = set->block_max;
    }

    free_elem = set->free_elems;
    set->free_elems = free_elem->next_free;

    id = free_elem->flags & IMP_SET_ELEM_IDX_MASK;
    if( element )
        IMP_MEMCPY_INT( free_elem, element, (unsigned int)set->elem_size/sizeof(int) );

    free_elem->flags = id;
    set->active_count++;

    if( inserted_element )
        *inserted_element = free_elem;
    return id;
}

/* Returns a set element by index. If the element doesn't belong to the set,
   NULL is returned */
IMP_SetElem* IMP_GetSetElem( const IMP_SET_S* set_header, int index )
{
    IMP_SetElem* elem = (IMP_SetElem*)IMP_GetSeqElem( (IMP_Seq*)set_header, index );
    return elem && IMP_IS_SET_ELEM( elem ) ? elem : 0;
}

/* Removes set element given its pointer */
void IMP_SetRemoveByPtr( IMP_SET_S* set_header, void* elem )
{
    IMP_SetElem* _elem = (IMP_SetElem*)elem;
    assert( _elem->flags >= 0 );
    _elem->next_free = set_header->free_elems;
    _elem->flags = (_elem->flags & IMP_SET_ELEM_IDX_MASK) | IMP_SET_ELEM_FREE_FLAG;
    set_header->free_elems = _elem;
    set_header->active_count--;
}

/* removes element from the set given its index */
void IMP_SetRemove( IMP_SET_S* set, int index )
{
    IMP_SetElem* elem = IMP_GetSetElem( set, index );
    if( elem )
        IMP_SetRemoveByPtr( set, elem );
    else if( !set )
         printf( "Sting is Null Ptr\n" );

}


/* removes all elements from the set */
 void
IMP_ClearSet( IMP_SET_S* set )
{
    ( IMP_ClearSeq( (IMP_Seq*)set ));
    set->free_elems = 0;
    set->active_count = 0;    
}


/* End of file. */
