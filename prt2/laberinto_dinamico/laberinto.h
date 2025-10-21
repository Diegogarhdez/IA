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

#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

struct Nodo {
  std::pair<int, int> posicion;
  int g;  // coste acumulado
  int h;  // heurística
  int f;  // coste total
  std::pair<int, int> padre;

  bool operator>(const Nodo& otro) const { return f > otro.f; }
};

std::ostream& operator<<(std::ostream& os,
                         const std::vector<std::vector<int>>& matriz) {
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
          laberinto_[i][j] = 1;  // pared
        } else {
          laberinto_[i][j] = 0;  // camino
        }
      }
    }
  };

  int ContarMuros() const;
  double PorcentajeMuros() const {
    int total = laberinto_.size() * laberinto_[0].size();
    return (static_cast<double>(ContarMuros()) / total) * 100.0;
  }
  void MostrarLaberinto(std::ofstream& os, int coste, int generados,
                        int inspeccionados) const {
    os << "Número de muros: " << ContarMuros() << std::endl;
    os << "Porcentaje de muros: " << PorcentajeMuros() << "%" << std::endl;
    os << "Entrada: (" << inicio_.first << ", " << inicio_.second << ")"
       << std::endl;
    os << "Salida: (" << fin_.first << ", " << fin_.second << ")" << std::endl;
    os << "Coste: " << coste << std::endl;
    os << "Número de nodos generados: " << generados << std::endl;
    os << "Número de nodos inspeccionados: " << inspeccionados << std::endl;
    os << "Laberinto actual:" << std::endl;
    os << laberinto_;
  }
  void MostrarLaberinto(std::ofstream& os) const { os << laberinto_; }
  bool ResolverLaberinto(std::ofstream&);
  bool ResolverLaberintoDinamico(std::ofstream&);

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

int Laberinto::ContarMuros() const {
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
      {0, 1}, {1, 0},  {0, -1}, {-1, 0},  // horizontal y vertical
      {1, 1}, {1, -1}, {-1, 1}, {-1, -1}  // diagonales
  };

  int num_filas = laberinto_.size();
  int num_columnas = laberinto_[0].size();

  int g = 0;
  int h = FACTOR_HEURISTICO * (std::abs(inicio_.first - fin_.first) +
                               std::abs(inicio_.second - fin_.second));
  int f = g + h;
  Nodo nodo_inicio = {inicio_, g, h, f, {-1, -1}};

  std::priority_queue<Nodo, std::vector<Nodo>, std::greater<Nodo>> open_list;
  std::vector<std::vector<bool>> closed_list(
      num_filas, std::vector<bool>(num_columnas, false));
  std::vector<std::vector<std::pair<int, int>>> padres(
      num_filas, std::vector<std::pair<int, int>>(num_columnas, {-1, -1}));
  open_list.push(nodo_inicio);
  padres[inicio_.first][inicio_.second] = {-1, -1};
  closed_list[inicio_.first][inicio_.second] = true;

  // Matriz que indica las casillas por las que EL AGENTE ya ha pasado (para
  // impresión coherente)
  std::vector<std::vector<bool>> visitado_por_agente(
      num_filas, std::vector<bool>(num_columnas, false));
  std::pair<int, int> posicion_agente = inicio_;
  visitado_por_agente[inicio_.first][inicio_.second] = true;

  // Nuevas variables solicitadas
  int nodos_generados = 0;
  int nodos_inspeccionados = 0;
  int coste_total = INT_MAX;

  bool encontrado = false;
  int iteracion = 0;
  while (!open_list.empty()) {
    Nodo nodo_actual = open_list.top();
    open_list.pop();
    ++nodos_inspeccionados;
    ++iteracion;

    // si la celda extraída ahora es muro (no procesable), la descartamos
    if (laberinto_[nodo_actual.posicion.first][nodo_actual.posicion.second] ==
        1) {
      continue;
    }

    // detectar si este nodo corresponde al siguiente movimiento elegido por el
    // agente
    bool agente_se_mueve_a_este_nodo =
        (nodo_actual.posicion != posicion_agente &&
         nodo_actual.padre == posicion_agente);

    if (agente_se_mueve_a_este_nodo) {
      // actualizar posición del agente y marcar como visitada
      posicion_agente = nodo_actual.posicion;
      visitado_por_agente[posicion_agente.first][posicion_agente.second] = true;

      // imprimir estado: solo cuando el agente efectúa un movimiento
      {
        std::vector<std::vector<int>> temp_matriz = laberinto_;
        // reconstruir camino recorrido por el agente hasta su nueva posición
        // usando 'padres'
        std::pair<int, int> p = posicion_agente;
        while (!(p.first == -1 && p.second == -1) && !(p == inicio_)) {
          if (temp_matriz[p.first][p.second] == 0)
            temp_matriz[p.first][p.second] = 2;
          p = padres[p.first][p.second];
        }
        temp_matriz[inicio_.first][inicio_.second] = 3;
        temp_matriz[fin_.first][fin_.second] = 4;

        output_file << "Iteración (movimiento) " << iteracion
                    << " - Agente se mueve a: (" << nodo_actual.posicion.first
                    << ", " << nodo_actual.posicion.second << ")"
                    << "  g=" << nodo_actual.g << std::endl;
        output_file << "Camino recorrido por el agente (marcado con 2):"
                    << std::endl;
        output_file << temp_matriz << std::endl;
      }
    }

    if (nodo_actual.posicion == fin_) {
      encontrado = true;
      coste_total = nodo_actual.g;
      break;
    }

    for (const auto& direccion : direcciones) {
      int nueva_fila = nodo_actual.posicion.first + direccion.first;
      int nueva_columna = nodo_actual.posicion.second + direccion.second;

      if (nueva_fila >= 0 && nueva_fila < num_filas && nueva_columna >= 0 &&
          nueva_columna < num_columnas &&
          laberinto_[nueva_fila][nueva_columna] != 1 &&
          !closed_list[nueva_fila][nueva_columna]) {
        bool es_movimiento_ortogonal =
            (direccion.first == 0 || direccion.second == 0);
        int coste_movimiento_para_g =
            es_movimiento_ortogonal
                ? COSTE_HORIZONTAL_VERTICAL
                : COSTE_DIAGONAL;  // diagonal = COSTE_DIAGONAL
        int nuevo_g = nodo_actual.g + coste_movimiento_para_g;

        int nuevo_h =
            FACTOR_HEURISTICO * (std::abs(nueva_fila - fin_.first) +
                                 std::abs(nueva_columna - fin_.second));
        int nuevo_f = nuevo_g + nuevo_h;

        Nodo nuevo_nodo = {{nueva_fila, nueva_columna},
                           nuevo_g,
                           nuevo_h,
                           nuevo_f,
                           nodo_actual.posicion};
        open_list.push(nuevo_nodo);
        // contar nodo generado (insertado en open_list)
        ++nodos_generados;
        padres[nueva_fila][nueva_columna] = nodo_actual.posicion;
        closed_list[nueva_fila][nueva_columna] = true;
      }
    }
  }
  if (encontrado) {
    std::pair<int, int> paso = fin_;
    while (paso != inicio_) {
      if (laberinto_[paso.first][paso.second] != 4) {
        laberinto_[paso.first][paso.second] = 2;  // marcar el camino final
      }
      paso = padres[paso.first][paso.second];
    }
    laberinto_[inicio_.first][inicio_.second] = 3;  // marcar la entrada
    laberinto_[fin_.first][fin_.second] = 4;        // marcar la salida
    output_file << "Laberinto resuelto:" << std::endl;
    // llamar a MostrarLaberinto con los contadores y el coste
    MostrarLaberinto(output_file, coste_total, nodos_generados,
                     nodos_inspeccionados);
    return true;
  } else {
    std::cout << "No se encontró solución para el laberinto." << std::endl;
    return false;
  }
}

bool Laberinto::ResolverLaberintoDinamico(std::ofstream& output_file) {
  const int COSTE_HORIZONTAL_VERTICAL = 5;
  const int COSTE_DIAGONAL = 7;
  const int FACTOR_HEURISTICO = 3;
  const double PROB_CAMBIO_POR_CASILLA =
      0.30;  // probabilidad de cambio después de cada inspección
  const double PORCENTAJE_MAXIMO_MUROS = 0.25;  // 25%

  std::vector<std::pair<int, int>> direcciones = {
      {0, 1}, {1, 0},  {0, -1}, {-1, 0},  // horizontal y vertical
      {1, 1}, {1, -1}, {-1, 1}, {-1, -1}  // diagonales
  };

  int num_filas = laberinto_.size();
  int num_columnas = laberinto_[0].size();
  int total_casillas = num_filas * num_columnas;
  int max_muros_permitidos =
      static_cast<int>(PORCENTAJE_MAXIMO_MUROS * total_casillas);

  int g = 0;
  int h = FACTOR_HEURISTICO * (std::abs(inicio_.first - fin_.first) +
                               std::abs(inicio_.second - fin_.second));
  int f = g + h;
  Nodo nodo_inicio = {inicio_, g, h, f, {-1, -1}};

  std::priority_queue<Nodo, std::vector<Nodo>, std::greater<Nodo>> open_list;
  std::vector<std::vector<bool>> closed_list(
      num_filas, std::vector<bool>(num_columnas, false));
  std::vector<std::vector<std::pair<int, int>>> padres(
      num_filas, std::vector<std::pair<int, int>>(num_columnas, {-1, -1}));
  open_list.push(nodo_inicio);
  padres[inicio_.first][inicio_.second] = {-1, -1};
  closed_list[inicio_.first][inicio_.second] = true;

  // Matriz que indica las casillas por las que EL AGENTE ya ha pasado (no se
  // modificarán)
  std::vector<std::vector<bool>> visitado_por_agente(
      num_filas, std::vector<bool>(num_columnas, false));
  // posición actual del agente (se mueve sólo cuando "elige" el siguiente paso)
  std::pair<int, int> posicion_agente = inicio_;
  visitado_por_agente[inicio_.first][inicio_.second] = true;

  // Contadores solicitados
  int nodos_generados = 0;
  int nodos_inspeccionados = 0;
  int coste_total = INT_MAX;

  // Inicializar semilla para random()
  srandom(static_cast<unsigned int>(time(nullptr)));

  bool encontrado = false;
  int iteracion = 0;  // para saltar la primera iteración de cambios
  while (!open_list.empty()) {
    Nodo nodo_actual = open_list.top();
    open_list.pop();
    ++nodos_inspeccionados;
    ++iteracion;

    // Si la casilla del nodo_actual se ha convertido en muro, ignorar
    if (laberinto_[nodo_actual.posicion.first][nodo_actual.posicion.second] ==
        1) {
      continue;
    }

    // Si este nodo es el siguiente movimiento del agente (padre ==
    // posicion_agente), el agente lo "elige" y se mueve: en ese momento
    // marcamos como visitado y imprimimos.
    bool agente_se_mueve_a_este_nodo =
        (nodo_actual.posicion != posicion_agente &&
         nodo_actual.padre == posicion_agente);

    if (agente_se_mueve_a_este_nodo) {
      // actualizar posición del agente y marcar como visitada (no se
      // modificarán en el futuro)
      posicion_agente = nodo_actual.posicion;
      visitado_por_agente[posicion_agente.first][posicion_agente.second] = true;

      // impresión: solo cuando el agente elige y se mueve
      {
        std::vector<std::vector<int>> temp_matriz = laberinto_;
        // reconstruir camino recorrido por el agente hasta su nueva posición
        // usando 'padres'
        std::pair<int, int> p = posicion_agente;
        while (!(p.first == -1 && p.second == -1) && !(p == inicio_)) {
          if (temp_matriz[p.first][p.second] == 0)
            temp_matriz[p.first][p.second] = 2;
          p = padres[p.first][p.second];
        }
        temp_matriz[inicio_.first][inicio_.second] = 3;
        temp_matriz[fin_.first][fin_.second] = 4;

        output_file << "Iteración (movimiento) " << iteracion
                    << " - Agente se mueve a: (" << nodo_actual.posicion.first
                    << ", " << nodo_actual.posicion.second << ")"
                    << "  g=" << nodo_actual.g << std::endl;
        output_file << "Camino recorrido por el agente (marcado con 2):"
                    << std::endl;
        output_file << temp_matriz << std::endl;
      }
    }

    // Si alcanzó la meta
    if (nodo_actual.posicion == fin_) {
      encontrado = true;
      coste_total = nodo_actual.g;
      // marcar fin como visitado por agente (aunque no se modificará)
      visitado_por_agente[fin_.first][fin_.second] = true;
      break;
    }

    // Aplicar cambios dinámicos solo a partir de la segunda iteración,
    // y NUNCA sobre celdas visitadas por el agente (incluye inicio y posiciones
    // ya recorridas)
    if (iteracion > 1) {
      for (int i = 0; i < num_filas; ++i) {
        for (int j = 0; j < num_columnas; ++j) {
          if ((i == inicio_.first && j == inicio_.second) ||
              (i == fin_.first && j == fin_.second))
            continue;
          if (visitado_por_agente[i][j])
            continue;  // no cambiar celdas ya recorridas por el agente
          if (laberinto_[i][j] != 0 && laberinto_[i][j] != 1) continue;
          int umbral = static_cast<int>(PROB_CAMBIO_POR_CASILLA * 10000.0);
          int valor_random = random() % 10000;
          if (valor_random < umbral) {
            laberinto_[i][j] = (laberinto_[i][j] == 1) ? 0 : 1;
          }
        }
      }

      // Asegurar que el número de muros no supere el 25%: eliminar muros NO
      // visitados por el agente
      int muros_actuales = ContarMuros();
      if (muros_actuales > max_muros_permitidos) {
        int eliminar = muros_actuales - max_muros_permitidos;
        std::vector<std::pair<int, int>> lista_muros;
        lista_muros.reserve(muros_actuales);
        for (int i = 0; i < num_filas; ++i) {
          for (int j = 0; j < num_columnas; ++j) {
            if (laberinto_[i][j] == 1) {
              if ((i == inicio_.first && j == inicio_.second) ||
                  (i == fin_.first && j == fin_.second))
                continue;
              if (visitado_por_agente[i][j]) continue;
              lista_muros.emplace_back(i, j);
            }
          }
        }
        while (eliminar > 0 && !lista_muros.empty()) {
          int idx = static_cast<int>(random() % lista_muros.size());
          auto pos = lista_muros[idx];
          laberinto_[pos.first][pos.second] = 0;
          lista_muros[idx] = lista_muros.back();
          lista_muros.pop_back();
          --eliminar;
        }
      }
    }

    // Generar vecinos con el laberinto ya actualizado
    for (const auto& direccion : direcciones) {
      int nueva_fila = nodo_actual.posicion.first + direccion.first;
      int nueva_columna = nodo_actual.posicion.second + direccion.second;

      if (nueva_fila >= 0 && nueva_fila < num_filas && nueva_columna >= 0 &&
          nueva_columna < num_columnas &&
          laberinto_[nueva_fila][nueva_columna] != 1 &&
          !closed_list[nueva_fila][nueva_columna]) {
        bool es_movimiento_ortogonal =
            (direccion.first == 0 || direccion.second == 0);
        int coste_movimiento_para_g = es_movimiento_ortogonal
                                          ? COSTE_HORIZONTAL_VERTICAL
                                          : COSTE_DIAGONAL;
        int nuevo_g = nodo_actual.g + coste_movimiento_para_g;

        int nuevo_h =
            FACTOR_HEURISTICO * (std::abs(nueva_fila - fin_.first) +
                                 std::abs(nueva_columna - fin_.second));
        int nuevo_f = nuevo_g + nuevo_h;

        Nodo nuevo_nodo = {{nueva_fila, nueva_columna},
                           nuevo_g,
                           nuevo_h,
                           nuevo_f,
                           nodo_actual.posicion};
        open_list.push(nuevo_nodo);
        ++nodos_generados;
        padres[nueva_fila][nueva_columna] = nodo_actual.posicion;
        closed_list[nueva_fila][nueva_columna] = true;
      }
    }
  }

  if (encontrado) {
    std::pair<int, int> paso = fin_;
    while (paso != inicio_) {
      if (laberinto_[paso.first][paso.second] != 4) {
        laberinto_[paso.first][paso.second] = 2;  // marcar el camino final
      }
      paso = padres[paso.first][paso.second];
    }
    laberinto_[inicio_.first][inicio_.second] = 3;  // marcar la entrada
    laberinto_[fin_.first][fin_.second] = 4;        // marcar la salida
    output_file << "Laberinto dinámico resuelto:" << std::endl;
    MostrarLaberinto(output_file, coste_total, nodos_generados,
                     nodos_inspeccionados);
    return true;
  } else {
    std::cout << "No se encontró solución para el laberinto dinámico."
              << std::endl;
    return false;
  }
}

#endif  // LABERINTO_H_