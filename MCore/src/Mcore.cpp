#include <cstdlib> /* Aquí contiene la función system("pause"); */
#include <iostream> /* Importamos esta librería para hacer uso de cout y así ver el resultado en pantalla. */
#include <string>

extern "C" __declspec(dllexport) std::string Name() {
    char* nombre;
    std::string str_obj("GeeksForGeeks");
    nombre = &str_obj[0];
    return str_obj;
} 