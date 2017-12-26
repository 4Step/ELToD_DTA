//*********************************************************
//	Data_Buffer.hpp - dynamically allocated memory buffer
//*********************************************************

#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP

#include "String.hpp"
#include "Buffer.hpp"

#include <string.h>
#include <stdlib.h>

//---------------------------------------------------------
//	Data_Buffer
//---------------------------------------------------------

class Data_Buffer 
{
public:
	Data_Buffer (void)                             { Set (); }
	Data_Buffer (void *data, int size)             { Set (); Data (data, size); }
	Data_Buffer (void *data, size_t size)          { Set (); Data (data, size); }
	Data_Buffer (const Data_Buffer &data)          { Set (); Data ((Data_Buffer &) data); }
	Data_Buffer (const Buffer &data)               { Set (); Data ((Buffer &) data); }

	~Data_Buffer (void)                            { Clear (); }
	
	bool   OK (void)                               { return (_buffer != 0); }
	void   Set (void)                              { _size = _max_size = 0; _buffer = _get_ptr = 0; }

	size_t Size (void)                             { return (_size); }
	bool   Size (int size)                          { return (Size ((size_t) size)); }
	bool   Size (size_t size);

	size_t Max_Size (void)                         { return (_max_size); }
	bool   Max_Size (int size)                     { return (Max_Size ((size_t) size)); }
	bool   Max_Size (size_t size);

	void Clear (void)                              { if (OK ()) { free (_buffer); _buffer = _get_ptr = 0; _size = _max_size = 0; } }
	void Fill (char fill = '\0')                   { if (OK () && _max_size > 0) memset (_buffer, fill, _max_size); }

	char * Data (void)                             { return (_buffer); }	

	bool Data (void *data, size_t size);
	bool Data (void *data, int size)               { return (Data (data, (size_t) size)); }
	bool Data (Data_Buffer &data)                  { return (Data ((char *) data.Data (), data.Size ())); }
	bool Data (Buffer &data)                       { return (Data (data.Pointer (), data.Size ())); }

	bool Add_Data (void *data, size_t size);
	bool Add_Data (void *data, int size)           { return (Add_Data (data, (size_t) size)); }
	bool Add_Data (Data_Buffer &data)              { return (Add_Data ((char *) data.Data (), data.Size ())); }
	bool Add_Data (Buffer &data)                   { return (Add_Data (data.Pointer (), data.Size ())); }

	Data_Buffer & Assign (void *data, size_t size) { Data (data, size); return (*this); }
	Data_Buffer & Assign (void *data, int size)    { return (Assign (data, (size_t) size)); }
	Data_Buffer & Assign (Data_Buffer &data)       { return (Assign ((char *) data.Data (), data.Size ())); }
	Data_Buffer & Assign (Buffer &data)            { return (Assign (data.Pointer (), data.Size ())); }

	Data_Buffer & Append (void *data, size_t size) { Add_Data (data, size); return (*this); }
	Data_Buffer & Append (void *data, int size)    { return (Append (data, (size_t) size)); }
	Data_Buffer & Append (Data_Buffer &data)       { return (Append ((char *) data.Data (), data.Size ())); }
	Data_Buffer & Append (Buffer &data)            { return (Append (data.Pointer (), data.Size ())); }

	Data_Buffer & operator = (const Data_Buffer &right) { return (Assign ((Data_Buffer &) right)); }	
	Data_Buffer & operator + (const Data_Buffer &right) { return (Append ((Data_Buffer &) right)); }

	bool Message (String message)                  { return (Data ((void *) message.c_str (), message.size () + 1)); }
	
	bool Get_Data (void *data, size_t size);
	bool Get_Data (void *data, int size)           { return (Get_Data (data, (size_t) size)); }

	void Set_First (void)                          { _get_ptr = _buffer; }
	bool Get_First (void *data, size_t size)       { Set_First (); return (Get_Data (data, size)); }

private:
	size_t _size, _max_size;
	char * _buffer;
	char * _get_ptr;
};
#endif
