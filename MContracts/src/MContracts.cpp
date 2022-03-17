#include <cstdlib> /* Aquí contiene la función system("pause"); */
#include <iostream> /* Importamos esta librería para hacer uso de cout y así ver el resultado en pantalla. */
#include <string>
#ifndef _WIN32
    #define EXPORT
    #else
    #define EXPORT __declspec(dllexport) 
#endif
extern "C" EXPORT std::string Name() { 
    char* nombre;
    std::string str_obj("segundo");
    nombre = &str_obj[0];
    return str_obj;
} 