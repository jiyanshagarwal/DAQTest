#include <iostream>
#include "sadi_labview.h"

int main() {
	if (connectSadi() == 1) {
		std::cout << "hi";
	}
}


//typedef UINT(CALLBACK* LPFNDLLFUNC1)();
//
//HINSTANCE hDLL;               // Handle to DLL
//LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer
//UINT  uReturnVal;
//
//hDLL = LoadLibraryA("SADI_LabVIEW.dll");
//
//if (hDLL != NULL) {
//	lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL, "connectSadi");
//	if (!lpfnDllFunc1) {
//		// Handle the error
//		FreeLibrary(hDLL);
//		return -1;
//	}
//	else {
//		// Call the function
//		uReturnVal = lpfnDllFunc1();
//		std::cout << uReturnVal << std::endl;
//
//		LPFNDLLFUNC1 lpfnDllFunc2 = (LPFNDLLFUNC1)GetProcAddress(hDLL, "getState");
//		std::cout << lpfnDllFunc2() << std::endl;
//
//		LPFNDLLFUNC1 lpfnDllFunc3 = (LPFNDLLFUNC1)GetProcAddress(hDLL, "disconnectSadi");
//		std::cout << lpfnDllFunc3() << std::endl;
//	}
//}