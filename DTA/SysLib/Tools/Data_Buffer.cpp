//*********************{***********************************
//	Data_Buffer.hpp - dynamically allocated memory buffer
//*********************************************************

#include "Data_Buffer.hpp"

//---------------------------------------------------------
//	Max_Size -- allocate memory buffer
//---------------------------------------------------------

bool Data_Buffer::Max_Size (size_t size) 
{
	if (size == 0) return (false);

	size_t old_size, new_size;

	old_size = _max_size;
	_max_size = 0;

	new_size = size;

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
	_max_size = new_size;

	char *start = _buffer;
	
	if (new_size > old_size) {
		start += old_size;
		new_size -= old_size;
	} else {
		if (_size > new_size) _size = new_size;
		new_size = 0;
	}
	if (new_size > 0) memset (start, '\0', new_size);
	_get_ptr = _buffer;

	return (true);
}

//---------------------------------------------------------
//	Size -- set the data size
//---------------------------------------------------------

bool Data_Buffer::Size (size_t size) 
{
	if (size > 0 && (size > _max_size || !OK ())) {
		if (!Max_Size (size)) return (false);
	}
	_size = size; 
	_get_ptr = _buffer;
	return (true);
}

//---------------------------------------------------------
//	Data -- copy data to the buffer
//---------------------------------------------------------

bool Data_Buffer::Data (void *data, size_t size) 
{
	if (!Size (size)) return (false);
		
	if (size && _buffer != data) {
		memcpy (_buffer, data, size);
	}
	_size = size;
	return (true);
}

//---------------------------------------------------------
//	Add_Data -- copy data to the end of the buffer
//---------------------------------------------------------

bool Data_Buffer::Add_Data (void *data, size_t size) 
{
	size_t old_size = Size ();
	char *start;

	if (!Size (old_size + size)) return (false);
	
	start = _buffer + old_size;

	if (size && start != data) {
		memcpy (start, data, size);
	}
	_size = old_size + size;
	return (true);
}

//---------------------------------------------------------
//	Get_Data -- copy data to the get offset
//---------------------------------------------------------

bool Data_Buffer::Get_Data (void *data, size_t size) 
{
	if ((_get_ptr + size) > (_buffer + _size)) return (false);

	memcpy (data, _get_ptr, size);
	_get_ptr += size;
	return (true);
}
