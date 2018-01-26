
/*
	参考：
	http://blog.csdn.net/wudaijun/article/details/9273339
*/

#include <iostream>
#include <map>
// using namespace std;

class A
{
public:
	A()
	{
		std::cout << "call a Constructor" << std::endl;
	}
	~A()
	{
		std::cout << "Call A Deconstrutor" << std::endl;
	}
	A(int a)
	{
		std::cout << "call a Constructor with param a=" << a << std::endl;
	}
};

// 定义全局operator new
void* operator new(size_t size)
{
	std::cout << "call global operator new:" << size << std::endl;
	return malloc(size);
}
void operator delete(void* p)
{
	std::cout << " free " << std::endl;
	free(p);
}
void* operator new[](size_t size)
{
	std::cout << "call global operator new array:" << size << std::endl;
	return malloc(size);
}
void operator delete[](void* p)
{
	std::cout << " free array " << std::endl;
	free(p);
}

// 自定义类型的new
void* operator new(size_t size, const char* file)
{
	std::cout << "call global operator new, define:" << file << ", size=" << size << std::endl;
	return malloc(size);
}

/************************************************************************/
/* 收集当前new和delete信息，这样就可以检测内存泄漏                      */
/************************************************************************/
std::map<void*, std::pair<const char*, int>> MemoryInfo;
void* operator new(size_t size, const char* file, int line)
{
	void* p = (void*)malloc(size);
	MemoryInfo[p] = std::make_pair(file, line);
	return p;
}
void BeforeDelete(void* p)
{
	MemoryInfo.erase(MemoryInfo.find(p));
}
void DumpMemoryInfo()
{
	std::cout << "*** memory leak count:" << MemoryInfo.size() << std::endl;
	for (auto it = MemoryInfo.begin(); it != MemoryInfo.end(); it++)
	{
		std::cout << "    leak memory:" << it->second.first << "[" << it->second.second << "]" << std::endl;
	}
}
#define New(type) new(__FILE__, __LINE__) type
#define Delete(p) do {BeforeDelete(p);delete p; p = NULL;} while(0)
#define NewArray(type) "not define"	// 数组的没定义呢！
#define DeleteArray(p) "delete array"

/************************************************************************/
/* 内存池                      */
/************************************************************************/


int main()
{
	{
		A* a = new A();
		delete a;
	}

	{
		A* a = new A(5);
		delete a;
	}

	{
		int* a = new int[5];
		delete []a;
	}

	// 自定义new
	{
		A* a = new(__FILE__) A();
		delete a;

		A* b = new(__FILE__) A(5);
		delete b;
	}

	// 检测内存泄漏
	{
		DumpMemoryInfo();
		A* a = New(A)(5);
		DumpMemoryInfo();
		Delete(a);
		DumpMemoryInfo();
	}

	return 0;
}