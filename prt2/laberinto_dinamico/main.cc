// Universidad de La Laguna
// Escuela Superior de Ingeniería y Tecnología
// Grado en Ingeniería Informática
// Asignatura: Inteligencia Artificial
// Curso: 3º
// Práctica 2
// Autor: Diego García Hernández
// Correo: alu0101633732@ull.edu.es
// Fecha: 23/10/2025
// Archivo main.cc
// Descripción: contiene la función main del programa

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "laberinto.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
    return 1;
  }

  std::ifstream input_file(argv[1]);
  if (!input_file.is_open()) {
    std::cerr << "Error opening file: " << argv[1] << std::endl;
    return 1;
  }

  int fila, columna;
  input_file >> fila >> columna;
  std::vector<std::vector<int>> matriz(fila, std::vector<int>(columna));
  for (int i = 0; i < fila; ++i) {
    for (int j = 0; j < columna; ++j) {
      input_file >> matriz[i][j];
    }
  }
  Laberinto laberinto = Laberinto(matriz);

  std::ofstream output_file(argv[2]);
  if (!output_file.is_open()) {
    std::cerr << "Error opening file: " << argv[2] << std::endl;
    return 1;
  }

  while (true) {
    int option;
    std::cout << "1. Mostrar laberinto" << std::endl;
    std::cout << "2. Resolver laberinto" << std::endl;
    std::cout << "3. Cambiar entrada y salida" << std::endl;
    std::cout << "4. Resolución dinámica del laberinto" << std::endl;
    std::cout << "10. Salir" << std::endl;
    std::cout << "Seleccione una opción: ";
    std::cin >> option;
    switch (option) {
    case 1:
      // Mostrar laberinto
      laberinto.MostrarLaberinto(output_file);

      break;
    case 2:
      // Resolver laberinto
      laberinto.ResolverLaberinto(output_file);
      break;
    case 3: {
      // Cambiar entrada y salida
      int inicio_fila, inicio_columna, fin_fila, fin_columna;
      std::cout << "Introduzca la fila y columna de la nueva entrada: ";
      std::cin >> inicio_fila >> inicio_columna;
      std::cout << "Introduzca la fila y columna de la nueva salida: ";
      std::cin >> fin_fila >> fin_columna;
      laberinto.SetPosicion(laberinto.GetInicio(), 1); // Quitar la antigua entrada
      laberinto.SetPosicion(laberinto.GetFin(), 1);    // Quitar la antigua salida
      laberinto.SetInicio({inicio_fila, inicio_columna});
      laberinto.SetFin({fin_fila, fin_columna});
      laberinto.SetPosicion(laberinto.GetInicio(), 3); // Marcar la nueva entrada
      laberinto.SetPosicion(laberinto.GetFin(), 4);    // Marcar la nueva salida
      std::cout << "Entrada y salida actualizadas." << std::endl;
      break;
    }
    case 4: {
      // Resolución dinámica del laberinto
      int contador = 0;
      while (!laberinto.ResolverLaberintoDinamico(output_file) && contador < 5) {
        ++contador;
        output_file << "Reintentando resolución dinámica del laberinto (intento "
                    << contador + 1 << ")..." << std::endl;
      }
      break;
    }
    case 10:
      // Salir
      std::cout << "Saliendo..." << std::endl;
      return 0;
    default:
      break;
    }
  }

  return 0;

}