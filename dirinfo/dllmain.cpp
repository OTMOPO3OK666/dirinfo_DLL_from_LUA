// dllmain.cpp: определяет точку входа для приложения DLL.
#include "stdafx.h"
#include <cstddef>

#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>

#define LUA_LIB
#define LUA_BUILD_AS_DLL

namespace fs = std::experimental::filesystem;

//Заголовочные файлы LUA из дистрибутива LUA
extern "C" {
#include "contrib/lauxlib.h"
#include "contrib/lua.h"
}

//Стандартная точка входа для DLL.
BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	return TRUE;
}

std::string joinList(std::vector<std::string> arr, std::string delimiter)
{
	if (arr.empty()) return "";

	std::string str;
	for (auto i : arr)
		str += i + delimiter;
	str = str.substr(0, str.size() - delimiter.size());
	return str;
}


//Наша функция которую мы запустим из lua, перемножает два числа.
/*static int MyLuaDllFunction(lua_State *L)
{
	//Получаем первый и второй параметры вызова функции из стека с проверкой каждого на число
	
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	lua_pushnumber(L, x*y);//Возвращаемое значение
	return (1);//Даем lua знать, количество значений возвращаемое данной функцией
}*/


//Наша функция которую мы запустим из lua, перемножает два числа.
static int GetListNameFiles(lua_State *L)
{
	size_t l = 0;
	size_t *r = &l;
	const char *path = luaL_checklstring(L, 1, r);
	const char *ext = luaL_checklstring(L, 2, r);
	const char *delim = luaL_checklstring(L, 3, r);
	std::vector<std::string> listName;
	char const * chRezult = "";

	for (auto& p : fs::recursive_directory_iterator(path))
	{
		if (p.path().extension() == ext)
		{
			listName.push_back(p.path().filename().string());
		}
	}

	std::string strRezult = joinList(listName, delim);
	chRezult = strRezult.data();

	lua_pushlstring(L, chRezult, strlen(chRezult));
	return (1);//Даем lua знать, количество значений возвращаемое данной функцией
}

//Регистрация реализованных в dll функций, что бы те стали доступны из lua.
static struct luaL_reg ls_lib[] = {
	{ "GetListNameFiles", GetListNameFiles },
	{ NULL, NULL }
};
//Эту функцию lua будет искать при подключении dll, её название заканчиваться названием dll, luaopen_ИМЯВАШЕЙDLL
extern "C" LUALIB_API int luaopen_dirinfo(lua_State *L) {
	luaL_openlib(L, "dirinfo", ls_lib, 0);
	return 0;
}