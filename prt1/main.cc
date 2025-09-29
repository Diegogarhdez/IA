// Universidad de La Laguna
// Escuela Superior de Ingeniería y Tecnología
// Grado en Ingeniería Informática
// Asignatura: Inteligencia Artificial
// Curso: 3º
// Práctica 1
// Autor: Diego García Hernández
// Correo: alu0101633732@ull.edu.es
// Fecha: 25/10/2025
// Archivo main.cc
// Descripción: contiene la función main del programa

#include <fstream>
#include <iostream>
#include <set>
#include <string>

#include "grafo.h"
#include "menu.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <nombre_del_archivo_salida>\n";
    return 1;
  }
  std::ofstream outfile(argv[1]);
  if (!outfile) {
    std::cerr << "Error al abrir el archivo de salida.\n";
    return 1;
  }
  std::cout << "Bienvenido al programa de grafos.\n";
  Grafo grafo;
  std::set<Arista> aristas;
  while (true) {
    PressAnyKey();
    Menu();
    char opcion;
    std::cin >> opcion;
    std::cin.ignore();
    std::string filename;
    std::ifstream file;
    switch (opcion) {
      case 'c': {
        std::cout << "Cargando grafo seleccionado.\n";
        std::cout << "Introduzca el nombre del archivo: ";
        std::cin >> filename;
        file = std::ifstream(filename);
        if (!file) {
          std::cerr << "Error al abrir el archivo.\n";
          break;
        }
        std::string line;
        int n;
        file >> n;
        aristas.clear();
        for (int i = 1; i <= n; ++i) {
          for (int j = i + 1; j <= n; ++j) {
            double coste;
            file >> coste;
            if (coste != -1.00 && i != j) {
              aristas.insert(Arista(i, j, coste));
            }
          }
        }
        grafo = Grafo(aristas);
        grafo.SetNumVertices(n);
        file.close();
        std::cout << "Grafo cargado correctamente.\n";
        break;
      }
      case 'i': {
        std::cout << "Imprimiendo el grafo.\n";
        outfile << grafo;
        std::cout << "Grafo impreso en el archivo " << argv[1] << ".\n";
        break;
      }
      case 'a': {
        std::cout << "Búsqueda en amplitud seleccionada.\n";
        std::cout << "Elija un nodo de inicio y un nodo final.\n";
        int nodo_inicio, nodo_fin;
        std::cout << "Nodo de inicio: ";
        std::cin >> nodo_inicio;
        std::cout << "Nodo final: ";
        std::cin >> nodo_fin;
        if (nodo_inicio < 1 || nodo_inicio > grafo.GetNumVertices() ||
            nodo_fin < 1 || nodo_fin > grafo.GetNumVertices()) {
          std::cerr << "Nodos inválidos. Deben estar entre 1 y "
                    << grafo.GetNumVertices() << ".\n";
          break;
        }
        grafo.BusquedaAmplitud(nodo_inicio, nodo_fin, outfile);
        break;
      }
      case 'p': {
        std::cout << "Búsqueda en profundidad seleccionada.\n";
        std::cout << "Elija un nodo de inicio y un nodo final.\n";
        int nodo_inicio, nodo_fin;
        std::cout << "Nodo de inicio: ";
        std::cin >> nodo_inicio;
        std::cout << "Nodo final: ";
        std::cin >> nodo_fin;
        if (nodo_inicio < 1 || nodo_inicio > grafo.GetNumVertices() ||
            nodo_fin < 1 || nodo_fin > grafo.GetNumVertices()) {
          std::cerr << "Nodos inválidos. Deben estar entre 1 y "
                    << grafo.GetNumVertices() << ".\n";
          break;
        }
        grafo.BusquedaProfundidad(nodo_inicio, nodo_fin, outfile);
        break;
      }
      case 'f': {
        std::cout << "Finalizando el programa.\n";
        return 0;
      }
      default:
        break;
    }
  }

  return 0;
}