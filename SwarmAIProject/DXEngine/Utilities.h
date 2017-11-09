/*



*/
#pragma once




// Safe Release
template<typename T> void SafeRelease(T& ptr)
{
	if(ptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

// Safe Delete Array
template<typename T> void SafeDeleteArray(T& ptr)
{
	if(ptr)
	{
		delete[] ptr;
		ptr = nullptr;
	}
}


// Safe Delete
template<typename T> void SafeDelete(T& ptr)
{
	if(ptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}