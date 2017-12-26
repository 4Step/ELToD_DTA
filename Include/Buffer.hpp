//*********************************************************
//	Buffer.hpp - dynamically allocated memory buffer
//*********************************************************

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <string.h>
#include <stdlib.h>

#include <string>
using namespace std;

#define READ_SIZE	2

//---------------------------------------------------------
//	Buffer
//---------------------------------------------------------

class Buffer 
{
public:
	Buffer (void)                             { _size = _max_size = 0; _buffer = 0; }
	Buffer (const Buffer &data)               { _size = _max_size = 0; _buffer = 0; Data ((Buffer &) data); }
	Buffer (const char *text)                 { _size = _max_size = 0; _buffer = 0; Data (text); }
	Buffer (string &text)                     { _size = _max_size = 0; _buffer = 0; Data (text); }

	~Buffer (void)                            { Clear (); }

	bool OK (void)                            { return (_buffer != 0); }

	int  Size (void)                          { return (_size); }
	bool Size (int size);

	int  Max_Size (void)                      { return (_max_size); }
	bool Max_Size (int size);

	int  Set_Size (bool flag = true);
	int  Max_Read (void)                      { return ((OK ()) ? _max_size + READ_SIZE - 1 : 0); }

	void Clear (void)                         { if (OK ()) { free (_buffer); _buffer = 0; _size = _max_size = 0; } }
	void Fill (char fill = '\0')              { if (OK () && _max_size > 0) memset (_buffer, fill, _max_size); }
	
	bool   Data (void *data, int size);
	bool   Data (Buffer &data);
	bool   Data (const char *text)            { return ((text) ? Data ((void *) text, (int) strlen (text)) : true); }
	bool   Data (string &text)                { return (Data ((void *) (text.c_str ()), (int) text.size ())); }
	char * Pointer (void)                     { return (_buffer); }
	string String (void)                      { string s; if (_buffer) s = _buffer; return (s); }

	Buffer & Assign (Buffer &data, int offset = 0, int count = -1);
	Buffer & Assign (void *data, int size);

	Buffer & operator = (const Buffer &right) { return (Assign ((Buffer &) right)); }	
	Buffer & operator = (const char *right)   { return ((right) ? Assign ((void *) right, (int) strlen (right)) : *this); }

private:
	int _size, _max_size;
	char *_buffer;
};
#endif
