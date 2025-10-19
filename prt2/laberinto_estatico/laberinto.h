// Universidad de La Laguna
// Escuela Superior de Ingeniería y Tecnología
// Grado en Ingeniería Informática
// Asignatura: Inteligencia Artificial
// Curso: 3º
// Práctica 2
// Autor: Diego García Hernández
// Correo: alu0101633732@ull.edu.es
// Fecha: 23/10/2025
// Archivo laberinto.h
// Descripción: contiene la clase laberinto

#ifndef LABERINTO_H_
#define LABERINTO_H_

#include <string>
#include <queue>
#include <iostream>
#include <cmath>
#include <vector>
#include <climits>
#include <algorithm>

struct Nodo {
  std::pair<int, int> posicion;
  int g; // coste acumulado
  int h; // heurística
  int f; // coste total
  std::pair<int, int> padre;

  bool operator>(const Nodo& otro) const {
    return f > otro.f;
  }
};

std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<int>>& matriz) {
  for (const auto& fila : matriz) {
    for (const auto& elemento : fila) {
      os << elemento << " ";
    }
    os << std::endl;
  }
  return os;
}

class Laberinto {
 public:
  Laberinto(const std::vector<std::vector<int>> laberinto)
      : laberinto_(laberinto), inicio_({0, 0}), fin_({0, 0}) {
        for (size_t i = 0; i < laberinto.size(); ++i) {
          for (size_t j = 0; j < laberinto[i].size(); ++j) {
            if (laberinto[i][j] == 3) {
              inicio_ = {i, j};
            } else if (laberinto[i][j] == 4) {
              fin_ = {i, j};
            } else if (laberinto[i][j] == 1) {
              laberinto_[i][j] = 1; // pared
            } else {
              laberinto_[i][j] = 0; // camino
            }
          }
        }
      };

  void MostrarLaberinto(std::ofstream& os) const {
    os << laberinto_;
  }
  bool ResolverLaberinto(std::ofstream&);
  int ContarMuros();

  std::pair<int, int> GetInicio() const { return inicio_; }
  std::pair<int, int> GetFin() const { return fin_; }
  std::vector<std::vector<int>> GetLaberinto() const { return laberinto_; }
  void SetInicio(const std::pair<int, int>& inicio) { inicio_ = inicio; }
  void SetFin(const std::pair<int, int>& fin) { fin_ = fin; }
  void SetPosicion(const std::pair<int, int>& posicion, int valor) {
    laberinto_[posicion.first][posicion.second] = valor;
  }

 private:
  std::vector<std::vector<int>> laberinto_;
  std::pair<int, int> inicio_;
  std::pair<int, int> fin_;
};

int Laberinto::ContarMuros() {
  int contador = 0;
  for (auto a : laberinto_) {
    for (auto b : a) {
      if (b == 1) ++contador;
    }
  }
  return contador;
}

bool Laberinto::ResolverLaberinto(std::ofstream& output_file) {

  const int COSTE_HORIZONTAL_VERTICAL = 5;
  const int COSTE_DIAGONAL = 7;
  const int FACTOR_HEURISTICO = 3;

  std::vector<std::pair<int, int>> direcciones = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0},     // horizontal y vertical
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}    // diagonales
  };

  int num_filas = laberinto_.size();
  int num_columnas = laberinto_[0].size();

  int g = 0;
  int h = FACTOR_HEURISTICO * (std::abs(inicio_.first - fin_.first) + std::abs(inicio_.second - fin_.second));
  int f = g + h;
  Nodo nodo_inicio = {inicio_, g, h, f, {-1, -1}};

  std::priority_queue<Nodo, std::vector<Nodo>, std::greater<Nodo>> open_list;
  std::vector<std::vector<bool>> closed_list(num_filas, std::vector<bool>(num_columnas, false));
  std::vector<std::vector<std::pair<int, int>>> padres(num_filas, std::vector<std::pair<int, int>>(num_columnas, {-1, -1}));
  open_list.push(nodo_inicio);
  padres[inicio_.first][inicio_.second] = {-1, -1};
  closed_list[inicio_.first][inicio_.second] = true;
  bool encontrado = false;
  while (!open_list.empty()) {
    Nodo nodo_actual = open_list.top();
    open_list.pop();

    if (nodo_actual.posicion == fin_) {
      encontrado = true;
      break;
    }

    for (const auto& direccion : direcciones) {
      int nueva_fila = nodo_actual.posicion.first + direccion.first;
      int nueva_columna = nodo_actual.posicion.second + direccion.second;

      if (nueva_fila >= 0 && nueva_fila < num_filas &&
          nueva_columna >= 0 && nueva_columna < num_columnas &&
          laberinto_[nueva_fila][nueva_columna] != 1 &&
          !closed_list[nueva_fila][nueva_columna]) {

        // Cambio: la diagonal NO se considera línea recta para g.
        // Para g, una diagonal se cuenta como dos movimientos ortogonales.
        bool es_movimiento_ortogonal = (direccion.first == 0 || direccion.second == 0);
        int coste_movimiento_para_g = es_movimiento_ortogonal ? COSTE_HORIZONTAL_VERTICAL
                                                              : 2 * COSTE_HORIZONTAL_VERTICAL; // diagonal = 2 * lateral
        int nuevo_g = nodo_actual.g + coste_movimiento_para_g;

        int nuevo_h = FACTOR_HEURISTICO * (std::abs(nueva_fila - fin_.first) + std::abs(nueva_columna - fin_.second));
        int nuevo_f = nuevo_g + nuevo_h;

        Nodo nuevo_nodo = {{nueva_fila, nueva_columna}, nuevo_g, nuevo_h, nuevo_f, nodo_actual.posicion};
        open_list.push(nuevo_nodo);
        padres[nueva_fila][nueva_columna] = nodo_actual.posicion;
        closed_list[nueva_fila][nueva_columna] = true;
      }
    }
  }
  if (encontrado) {
    std::pair<int, int> paso = fin_;
    while (paso != inicio_) {
      if (laberinto_[paso.first][paso.second] != 4) {
        laberinto_[paso.first][paso.second] = 2; // marcar el camino
      }
      paso = padres[paso.first][paso.second];
    }
    laberinto_[inicio_.first][inicio_.second] = 3; // marcar la entrada
    laberinto_[fin_.first][fin_.second] = 4; // marcar la salida
    output_file << "Laberinto resuelto:" << std::endl;
    MostrarLaberinto(output_file);
    return true;
  } else {
    std::cout << "No se encontró solución para el laberinto." << std::endl;
    return false;
  }

}


#endif  // LABERINTO_H_