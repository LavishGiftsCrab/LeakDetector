#define NEW_OVERLOAD_IMPLEMENTATION_
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<cstring>
#include "LeakDetector.h"

size_t LeakDetector::_callCount = 0;

typedef struct MemoryList {
	struct MemoryList* _prev;
	struct MemoryList* _next;
	size_t _size;
	bool _isArray;
	char* _file;
	size_t _line;
}MemoryList;

static MemoryList memoryListHead = { &memoryListHead, &memoryListHead, 0, false, NULL, 0 };
static size_t memoryAllocated = 0;


void* AllocateMemory(size_t size, bool array, char* file, size_t line) {
	size_t newSize = size + sizeof(MemoryList);
	MemoryList* newElem = (MemoryList*)malloc(newSize);

	newElem->_prev = &memoryListHead;
	newElem->_next = memoryListHead._next;
	newElem->_size = size;
	newElem->_isArray = array;

	if (NULL != file) {
		newElem->_file = (char*)malloc(strlen(file) + 1);
		strcpy(newElem->_file, file);
	}
	else
		newElem->_file = NULL;


	newElem->_line = line;
	memoryListHead._next->_prev = newElem;
	memoryListHead._next = newElem;
	memoryAllocated += size;
	return (char*)newElem + sizeof(memoryListHead);
}


void DeleteMemory(void* ptr, bool array)
{
	MemoryList* curElem = (MemoryList*)((char*)ptr - sizeof(MemoryList));
	if (curElem->_isArray != array)
		return;

	curElem->_next->_prev = curElem->_prev;
	curElem->_prev->_next = curElem->_next;
	memoryAllocated -= curElem->_size;
	if (NULL != curElem->_file)
		free(curElem->_file);
	free(curElem);
}


void *operator new(size_t size, char* file, size_t line) {
	return AllocateMemory(size, false, file, line);
}


void* operator new[](size_t size, char* file, size_t line) {
	return AllocateMemory(size, true, file, line);
}
void operator delete(void* ptr) {
	DeleteMemory(ptr, false);
}

void operator delete[](void* ptr) {
	DeleteMemory(ptr, true);
}
void LeakDetector::_LeakDetector() {
	if (0 == memoryAllocated) {
		std::cout << "没有内存泄露" << std::endl;
		return;
	}

	size_t count = 0;

	MemoryList* ptr = memoryListHead._next;
	while ((NULL != ptr) && (&memoryListHead != ptr)){ 
		if (true == ptr->_isArray)
		{
			std::cout << "new[] 空间未释放, ";
		}
		else			
			std::cout << "new 空间未释放, "; 		
		std::cout << "指针: " << ptr << " 大小: " << ptr->_size; 		
		if (NULL != ptr->_file)			
			std::cout << " 位于 " << ptr->_file << " 第 " << ptr->_line << " 行";		
		else			
			std::cout << " (无文件信息)"; 		
		std::cout << std::endl; 
		system("pause");
		ptr = ptr->_next;		
		++count;
	} 	
	std::cout << "存在" << count << "处内存泄露, 共包括 " << memoryAllocated << " byte." << std::endl;
	return;

	
}