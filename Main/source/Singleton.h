//////////////////////////////////////////////////////////////////////////
//  - ΩÃ±€≈Ê -
//  
//  
//////////////////////////////////////////////////////////////////////////
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <stdint.h>

/*+++++++++++++++++++++++++++++++++++++
	CLASS.
+++++++++++++++++++++++++++++++++++++*/
template <typename T>
class Singleton
{
	static T* _Singleton;
public:
	Singleton(void)
	{
		if (_Singleton == 0)
		{
			const intptr_t base = reinterpret_cast<intptr_t>(static_cast<Singleton<T>*>((T*)1));
			const intptr_t derived = reinterpret_cast<intptr_t>(static_cast<T*>((Singleton<T>*)(T*)1));
			const intptr_t offset = derived - base;
			_Singleton = reinterpret_cast<T*>(reinterpret_cast<intptr_t>(this) + offset);
		}
	}

	virtual ~Singleton(void)
	{
		_Singleton = 0;
	}

	static T& GetSingleton(void)
	{
		return (*_Singleton);
	}

	static T* GetSingletonPtr(void)
	{
		return (_Singleton);
	}

	static bool IsInitialized(void)
	{
		return _Singleton ? true : false;
	}
};

template <typename T> T* Singleton <T>::_Singleton = 0;

#endif