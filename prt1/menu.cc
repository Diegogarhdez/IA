


#include "menu.h"

#include <iostream>

void Menu() {
    std::cout << "\n\nIngrese una opción:\n\n" 
              << "(c) cargar grafo.\n"
              << "(i) imprimir el grafo.\n"
              << "(p) búsqueda en profundidad.\n"
              << "(a) búsqueda en amplitud.\n"
              << "(f) finalizar el programa.\n\n";
}

void PressAnyKey() {
    std::cout << "Presione cualquier letra para continuar...";
    char a;
    std::cin >> a;
    system("clear");
}