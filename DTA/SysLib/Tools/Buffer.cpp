//*********************{***********************************
//	Buffer.hpp - dynamically allocated memory buffer
//*********************************************************

#include "Buffer.hpp"

//---------------------------------------------------------
//	Max_Size -- allocate memory buffer
//---------------------------------------------------------

bool Buffer::Max_Size (int size) 
{
	if (size < 0) return (false);

	int old_size, new_size;

	old_size = _max_size + READ_SIZE;
	_max_size = 0;

	new_size = size + READ_SIZE;

	char *temp = (char *) realloc (_buffer, new_size);
	if (!temp) {
		if (_buffer) {
			free (_buffer);
			_buffer = 0;
		}
		_size = 0;
		return (false);
	} else {
		_buffer = temp;
	}
	_max_size = size;

	char *start = _buffer;
	
	if (old_size > READ_SIZE) {
		if (new_size > old_size) {
			start += old_size;
			new_size -= old_size;
		} else {
			if (_size > size) _size = size;
			new_size = 0;
		}
	}
	if (new_size > 0) memset (start, '\0', new_size);

	return (true);
}

//---------------------------------------------------------
//	Size -- set the data size
//---------------------------------------------------------

bool Buffer::Size (int size) 
{
	if (size > 0 && (size > _max_size || !OK ())) {
		if (!Max_Size (size)) return (false);
	}
	_size = size; 
	return (size == _size);
}

//---------------------------------------------------------
//	Set_Size -- set the data size
//---------------------------------------------------------

int Buffer::Set_Size (bool flag) 
{
	if (!_buffer) return (0);

	if (flag) {
		_size = (unsigned short) strlen (_buffer);
		if (_size > _max_size) {
			_size = _max_size;
			_buffer [_size] = '\0';
		}
	} else {
		_size = _max_size;
	}
	return (_size);
}

//---------------------------------------------------------
//	Data -- copy data to the buffer
//---------------------------------------------------------

bool Buffer::Data (void *data, int size) 
{
	if (!Size (size)) return (false);
		
	if (size && _buffer != data) {
		memcpy (_buffer, data, size);
	}
	if (OK ()) _buffer [size] = '\0';
	_size = (unsigned short) size;
	return (true);
}

bool Buffer::Data (Buffer &data)
{
	if (!Size (data._size)) return (false);
	_size = data._size;
	if (_size > 0) {
		memcpy (_buffer, data._buffer, _size);
	}
	if (OK ()) _buffer [_size] = '\0';
	return (true);
}

//---------------------------------------------------------
//	Assign -- copy data to the buffer
//---------------------------------------------------------

Buffer & Buffer::Assign (void *data, int size) 
{
	if (Size (size)) {
		if (size && _buffer != data) {
			memcpy (_buffer, data, size);
		}
		if (OK ()) _buffer [size] = '\0';
		_size = (unsigned short) size;
	}
	return (*this);
}

Buffer & Buffer::Assign (Buffer &data, int offset, int count) 
{
	if (count < 0) {
		count = data._size - offset;
	}
	if (offset >= 0 && offset + count <= data._size) {
		if (Size (count)) {
			if (count) {
				memcpy (_buffer, data._buffer + offset, count);
			}
			if (OK ()) _buffer [count] = '\0';
			_size = (unsigned short) count;
		}
	}
	return (*this);
}
