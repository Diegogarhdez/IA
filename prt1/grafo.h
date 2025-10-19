// Universidad de La Laguna
// Escuela Superior de Ingeniería y Tecnología
// Grado en Ingeniería Informática
// Asignatura: Inteligencia Artificial
// Curso: 3º
// Práctica 1
// Autor: Diego García Hernández
// Correo: alu0101633732@ull.edu.es
// Fecha: 25/10/2025
// Archivo grafo.h
// Descripción: contiene la definición de la clase grafo

#ifndef GRAFO_H_
#define GRAFO_H_

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <vector>

class Grafo {
 public:
  Grafo(int n = 0) : vertices(n), matriz_(n, std::vector<double>(n, -1.0)) {}

  void InsertarArista(int i, int j, double coste) {
    if (i >= 1 && i <= vertices && j >= 1 && j <= vertices && i != j) {
      matriz_[i - 1][j - 1] = coste;
      matriz_[j - 1][i - 1] = coste;  // grafo no dirigido
    }
  }

  void EliminarArista(int i, int j) {
    if (i >= 1 && i <= vertices && j >= 1 && j <= vertices) {
      matriz_[i - 1][j - 1] = -1.0;
      matriz_[j - 1][i - 1] = -1.0;
    }
  }

  void BusquedaProfundidad(int nodo_inicio, int nodo_fin, std::ostream& os);
  void BusquedaAmplitud(int nodo_inicio, int nodo_fin, std::ostream& os);

  inline int GetNumVertices() const { return vertices; }
  inline void SetNumVertices(int n) {
    vertices = n;
    matriz_.assign(n, std::vector<double>(n, -1.0));
  }

  inline const std::vector<std::vector<double>>& GetMatriz() const {
    return matriz_;
  }

  // Cuenta aristas (no dirigidas, recorre solo i<j)
  int ContarAristas() const {
    int c = 0;
    for (int i = 0; i < vertices; ++i)
      for (int j = i + 1; j < vertices; ++j)
        if (matriz_[i][j] != -1.0) ++c;
    return c;
  }

 private:
  int vertices;
  std::vector<std::vector<double>> matriz_;

  // Suma los pesos de un camino (vector de nodos 1..n)
  double CalcularCosteCamino(const std::vector<int>& camino) const {
    double coste = 0.0;
    if (camino.size() < 2) return 0.0;
    for (size_t k = 0; k + 1 < camino.size(); ++k) {
      int u = camino[k];
      int v = camino[k + 1];
      double w = matriz_[u - 1][v - 1];
      if (w != -1.0) coste += w;
    }
    return coste;
  }
};

// ===================== IMPRESIÓN =====================
std::ostream& operator<<(std::ostream& os, const Grafo& grafo) {
  const auto& m = grafo.GetMatriz();
  for (int i = 0; i < grafo.GetNumVertices(); ++i) {
    for (int j = i + 1; j < grafo.GetNumVertices(); ++j) {
      if (m[i][j] != -1.0) {
        os << "Nodo 1: " << (i + 1) << ", Nodo 2: " << (j + 1)
           << ", Coste: " << std::fixed << std::setprecision(2) << m[i][j]
           << "\n";
      }
    }
  }
  return os;
}

// ===================== DFS =====================
void Grafo::BusquedaProfundidad(int nodo_inicio, int nodo_fin,
                                std::ostream& os) {
  std::vector<int> generados;  // pueden repetirse
  std::vector<int> inspeccionados;
  std::vector<int> path;
  int iteracion = 1;
  bool encontrado = false;

  os << "Número de nodos del grafo: " << vertices << "\n";
  os << "Número de aristas del grafo: " << ContarAristas() << "\n";
  os << "Vértice origen: " << nodo_inicio << "\n";
  os << "Vértice destino: " << nodo_fin << "\n";
  os << "---------------------------------------------------\n";

  std::function<void(int)> dfs = [&](int nodo_actual) {
    if (encontrado) return;

    path.push_back(nodo_actual);
    generados.push_back(nodo_actual);

    os << "Iteración " << iteracion++ << "\n";
    os << "Nodo actual: " << nodo_actual << "\n";

    os << "Nodos generados: ";
    if (generados.empty())
      os << "-";
    else
      for (size_t i = 0; i < generados.size(); ++i) {
        if (i > 0) os << ", ";
        os << generados[i];
      }
    os << "\n";

    os << "Nodos inspeccionados: ";
    if (inspeccionados.empty())
      os << "-";
    else
      for (size_t i = 0; i < inspeccionados.size(); ++i) {
        if (i > 0) os << ", ";
        os << inspeccionados[i];
      }
    os << "\n";
    os << "---------------------------------------------------\n";

    if (nodo_actual == nodo_fin) {
      std::vector<int> camino = path;
      os << "Camino: ";
      for (size_t i = 0; i < camino.size(); ++i) {
        if (i > 0) os << " - ";
        os << camino[i];
      }
      double coste_total = CalcularCosteCamino(camino);
      os << "\nCosto: " << std::fixed << std::setprecision(3) << coste_total
         << "\n";
      encontrado = true;
      path.pop_back();
      return;
    }

    for (int j = 1; j <= vertices && !encontrado; ++j) {
      if (matriz_[nodo_actual - 1][j - 1] != -1.0) {
        bool en_ruta = false;
        for (int v : path)
          if (v == j) {
            en_ruta = true;
            break;
          }
        if (!en_ruta) dfs(j);
      }
    }

    inspeccionados.push_back(nodo_actual);
    path.pop_back();
  };

  dfs(nodo_inicio);

  if (!encontrado) {
    os << "No se encontró un camino desde " << nodo_inicio << " hasta "
       << nodo_fin << ".\n";
  }
}

// ===================== BFS =====================
void Grafo::BusquedaAmplitud(int nodo_inicio, int nodo_fin, std::ostream& os) {
  struct Estado {
    int nodo;
    int padre_idx;
  };
  std::vector<Estado> estados;
  std::vector<int> q;
  std::vector<int> generados;
  std::vector<int> inspeccionados;
  int iteracion = 1;

  os << "Número de nodos del grafo: " << vertices << "\n";
  os << "Número de aristas del grafo: " << ContarAristas() << "\n";
  os << "Vértice origen: " << nodo_inicio << "\n";
  os << "Vértice destino: " << nodo_fin << "\n";
  os << "---------------------------------------------------\n";

  if (nodo_inicio == nodo_fin) {
    os << "Iteración 1\nNodo actual: " << nodo_inicio << "\n";
    os << "Nodos generados: " << nodo_inicio << "\n";
    os << "Nodos inspeccionados: -\n";
    os << "Camino: " << nodo_inicio << "\n";
    os << "Costo: 0.00\n";
    return;
  }

  estados.push_back({nodo_inicio, -1});
  q.push_back(0);
  generados.push_back(nodo_inicio);

  while (!q.empty()) {
    int idx = q.front();
    q.erase(q.begin());
    Estado actual = estados[idx];
    int nodo_actual = actual.nodo;

    os << "Iteración " << iteracion++ << "\n";
    os << "Nodo actual: " << nodo_actual << "\n";

    os << "Nodos generados: ";
    if (generados.empty())
      os << "-";
    else
      for (size_t i = 0; i < generados.size(); ++i) {
        if (i > 0) os << ", ";
        os << generados[i];
      }
    os << "\n";

    os << "Nodos inspeccionados: ";
    if (inspeccionados.empty())
      os << "-";
    else
      for (size_t i = 0; i < inspeccionados.size(); ++i) {
        if (i > 0) os << ", ";
        os << inspeccionados[i];
      }
    os << "\n";

    if (nodo_actual == nodo_fin) {
      std::vector<int> camino;
      for (int cur = idx; cur != -1; cur = estados[cur].padre_idx) {
        camino.push_back(estados[cur].nodo);
      }
      std::reverse(camino.begin(), camino.end());
      os << "Camino: ";
      for (size_t i = 0; i < camino.size(); ++i) {
        if (i > 0) os << " - ";
        os << camino[i];
      }
      double coste_total = CalcularCosteCamino(camino);
      os << "\nCosto: " << std::fixed << std::setprecision(3) << coste_total
         << "\n";
      return;
    }

    // Recolectamos los vecinos con su costo
    std::vector<std::pair<int, double>> vecinos;  // (nodo, costo)
    for (int j = 1; j <= vertices; ++j) {
      double costo_arista = matriz_[nodo_actual - 1][j - 1];
      if (costo_arista == -1.0) continue;
      bool ciclo = false;
      for (int k = idx; k != -1; k = estados[k].padre_idx) {
        if (estados[k].nodo == j) {
          ciclo = true;
          break;
        }
      }
      if (!ciclo) {
        vecinos.push_back({j, costo_arista});
      }
    }
    srand(1);
    if (!vecinos.empty()) {
      // Encontramos el mejor y peor nodo según el costo
      auto mejor = vecinos[0];
      auto peor = vecinos[0];
      for (auto& v : vecinos) {
        if (v.second <= mejor.second) mejor = v;
        if (v.second >= peor.second) peor = v;
      }

      // Selección aleatoria entre el mejor y el peor
      auto seleccionado = (rand() % 2 == 0) ? mejor : peor;

      // Agregamos solo el nodo seleccionado
      estados.push_back({seleccionado.first, idx});
      q.push_back(estados.size() - 1);
      generados.push_back(seleccionado.first);
    }

    inspeccionados.push_back(nodo_actual);
    os << "---------------------------------------------------\n";
  }

  os << "No se encontró un camino desde " << nodo_inicio << " hasta "
     << nodo_fin << ".\n";
}

#endif  // GRAFO_H_
