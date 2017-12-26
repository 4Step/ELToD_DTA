//*********************************************************
//	Data_Types.hpp - data types and classes
//*********************************************************

#include "String.hpp"

#include <vector>
#include <map>
using namespace std;

//---- array of integers ----

typedef vector <int>                 Integers;
typedef Integers::iterator           Int_Itr;
typedef Integers::reverse_iterator   Int_RItr;
typedef Integers::pointer            Int_Ptr;

//---- array of integer arrays ----

typedef vector <Integers>            Ints_Array;
typedef Ints_Array::iterator         Ints_Itr;
typedef Ints_Array::reverse_iterator Ints_RItr;
typedef Ints_Array::pointer          Ints_Ptr;

//---- an integer sort index ----

typedef map <int, int>               Int_Map;
typedef pair <int, int>              Int_Map_Data;
typedef Int_Map::iterator            Int_Map_Itr;
typedef Int_Map::reverse_iterator    Int_Map_RItr;
typedef pair <Int_Map_Itr, bool>     Int_Map_Stat;

//---- two integer sort index ----

typedef pair <int, int>              Int2_Key;
typedef map <Int2_Key, int>          Int2_Map;
typedef pair <Int2_Key, int>         Int2_Map_Data;
typedef Int2_Map::iterator           Int2_Map_Itr;
typedef Int2_Map::reverse_iterator   Int2_Map_RItr;
typedef pair <Int2_Map_Itr, bool>    Int2_Map_Stat;

//---- array of floats ----

typedef vector <float>               Floats;
typedef Floats::iterator             Flt_Itr;
typedef Floats::reverse_iterator     Flt_RItr;
typedef Floats::pointer              Flt_Ptr;

//---- array of float arrays ----

typedef vector <Floats>              Flts_Array;
typedef Flts_Array::iterator         Flts_Itr;
typedef Flts_Array::reverse_iterator Flts_RItr;
typedef Flts_Array::pointer          Flts_Ptr;

//---- array of strings ----

typedef vector <String>              Strings;
typedef Strings::iterator            Str_Itr;
typedef Strings::reverse_iterator    Str_RItr;
typedef Strings::pointer             Str_Ptr;

//---- a string key map ----

typedef map <String, String>         Str_Key_Map;
typedef pair <String, String>        Str_Key_Map_Data;
typedef Str_Key_Map::iterator        Str_Key_Map_Itr;
typedef pair <Str_Key_Map_Itr, bool> Str_Key_Map_Stat;

//---- a string id map ----

typedef map <String, int>            Str_ID;
typedef pair <String, int>           Str_ID_Data;
typedef Str_ID::iterator             Str_ID_Itr;
typedef pair <Str_ID_Itr, bool>      Str_ID_Stat;