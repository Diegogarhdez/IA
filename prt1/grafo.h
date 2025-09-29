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
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <vector>

class Arista {
 public:
  Arista(int nodo1, int nodo2, double coste)
      : nodo1_(nodo1), nodo2_(nodo2), coste_(coste) {}
  Arista() : nodo1_(0), nodo2_(0), coste_(0.0) {}
  Arista(const Arista& other)
      : nodo1_(other.nodo1_), nodo2_(other.nodo2_), coste_(other.coste_) {}

  bool operator<(const Arista& other) const {
    if (nodo1_ != other.GetNodo1()) return nodo1_ < other.GetNodo1();
    if (nodo2_ != other.GetNodo2()) return nodo2_ < other.GetNodo2();
    return coste_ < other.GetCoste();
  }

  Arista operator=(const Arista& other) {
    if (this == &other) return *this;
    nodo1_ = other.nodo1_;
    nodo2_ = other.nodo2_;
    coste_ = other.coste_;
    return *this;
  }

  // Getters
  inline int GetNodo1() const { return nodo1_; }
  inline int GetNodo2() const { return nodo2_; }
  inline double GetCoste() const { return coste_; }

  // atributos privados
 private:
  int nodo1_;
  int nodo2_;
  double coste_;
};

class Grafo {
 public:
  Grafo() {}
  Grafo(const std::set<Arista>& grafo) : vertices(grafo.size()) {
    for (const auto& arista : grafo) {
      grafo_.insert(arista);
    }
  }

  void InsertarArista(const Arista& arista) { grafo_.insert(arista); }
  void EliminarArista(const Arista& arista) { grafo_.erase(arista); }
  void BusquedaProfundidad(int nodo_inicio, int nodo_fin, std::ostream& os);
  void BusquedaAmplitud(int nodo_inicio, int nodo_fin, std::ostream& os);

  inline std::set<Arista> GetGrafo() const { return grafo_; }
  inline int GetNumVertices() const { return vertices; }
  inline void SetNumVertices(int n) { vertices = n; }

  // atributos privados
 private:
  std::set<Arista> grafo_;
  int vertices = grafo_.size();
};

std::ostream& operator<<(std::ostream& os, const Grafo& grafo) {
  for (const auto& arista : grafo.GetGrafo()) {
    os << "Nodo 1: " << arista.GetNodo1() << ", Nodo 2: " << arista.GetNodo2()
       << ", Coste: " << arista.GetCoste() << "\n";
  }
  return os;
}

void Grafo::BusquedaProfundidad(int nodo_inicio, int nodo_fin,
                                std::ostream& os) {
  std::set<int> visitados;    // Nodos ya explorados
  std::vector<int> pila;      // Pila DFS
  std::map<int, int> padres;  // Para reconstruir el camino
  int iteracion = 1;          // Contador de iteraciones

  // Información inicial
  os << "Número de nodos del grafo: " << vertices << "\n";
  os << "Número de aristas del grafo: " << grafo_.size() << "\n";
  os << "Vértice origen: " << nodo_inicio << "\n";
  os << "Vértice destino: " << nodo_fin << "\n";

  pila.push_back(nodo_inicio);
  padres[nodo_inicio] = -1;

  while (!pila.empty()) {
    int nodo_actual = pila.back();
    pila.pop_back();

    // Agregar nodo a visitados (ahora sí se inspecciona)
    visitados.insert(nodo_actual);

    // --- Salida por iteración ---
    os << "Iteración " << iteracion++ << "\n";

    // Nodos generados = todos los que han sido descubiertos (pila + visitados)
    std::set<int> generados = visitados;
    for (auto v : pila) generados.insert(v);

    os << "Nodos generados: ";
    for (auto it = generados.begin(); it != generados.end(); ++it) {
      if (it != generados.begin()) os << ", ";
      os << *it;
    }
    os << "\n";

    os << "Nodos inspeccionados: ";
    if (visitados.empty()) {
      os << "-";
    } else {
      bool first = true;
      for (auto v : visitados) {
        if (!first) os << ", ";
        os << v;
        first = false;
      }
    }
    os << "\n";

    // --- Verificar si encontramos el destino ---
    if (nodo_actual == nodo_fin) {
      std::vector<int> camino;
      for (int nodo = nodo_fin; nodo != -1; nodo = padres[nodo]) {
        camino.push_back(nodo);
      }
      std::reverse(camino.begin(), camino.end());

      os << "Camino: ";
      for (size_t i = 0; i < camino.size(); i++) {
        if (i > 0) os << " - ";
        os << camino[i];
      }
      os << "\n";

      os << "Costo: " << (camino.size() - 1) << "\n";
      return;
    }

    // --- Expandir vecinos ---
    for (const auto& arista : grafo_) {
      int vecino = -1;
      if (arista.GetNodo1() == nodo_actual) {
        vecino = arista.GetNodo2();
      } else if (arista.GetNodo2() == nodo_actual) {
        vecino = arista.GetNodo1();
      }
      if (vecino != -1 && padres.find(vecino) == padres.end()) {
        pila.push_back(vecino);
        padres[vecino] = nodo_actual;
      }
    }
  }

  os << "No se encontró un camino desde " << nodo_inicio << " hasta "
     << nodo_fin << ".\n";
}

void Grafo::BusquedaAmplitud(int nodo_inicio, int nodo_fin, std::ostream& os) {
  std::set<int> visitados;       // Nodos descubiertos
  std::queue<int> cola;          // Cola BFS
  std::map<int, int> padres;     // Para reconstruir el camino
  std::set<int> inspeccionados;  // Nodos ya procesados
  int iteracion = 1;             // Contador de iteraciones

  // Información inicial
  os << "Número de nodos del grafo: " << vertices << "\n";
  os << "Número de aristas del grafo: " << grafo_.size() << "\n";
  os << "Vértice origen: " << nodo_inicio << "\n";
  os << "Vértice destino: " << nodo_fin << "\n";

  cola.push(nodo_inicio);
  visitados.insert(nodo_inicio);
  padres[nodo_inicio] = -1;

  while (!cola.empty()) {
    int nodo_actual = cola.front();
    cola.pop();

    // Marcar nodo como inspeccionado
    inspeccionados.insert(nodo_actual);

    // --- Salida por iteración ---
    os << "Iteración " << iteracion++ << "\n";

    // Nodos generados = todos los que se han descubierto (visitados)
    os << "Nodos generados: ";
    {
      bool first = true;
      for (auto v : visitados) {
        if (!first) os << ", ";
        os << v;
        first = false;
      }
    }
    os << "\n";

    // Nodos inspeccionados
    os << "Nodos inspeccionados: ";
    if (inspeccionados.empty()) {
      os << "-";
    } else {
      bool first = true;
      for (auto v : inspeccionados) {
        if (!first) os << ", ";
        os << v;
        first = false;
      }
    }
    os << "\n";

    // --- Verificar si encontramos el destino ---
    if (nodo_actual == nodo_fin) {
      std::vector<int> camino;
      for (int nodo = nodo_fin; nodo != -1; nodo = padres[nodo]) {
        camino.push_back(nodo);
      }
      std::reverse(camino.begin(), camino.end());

      os << "Camino: ";
      for (size_t i = 0; i < camino.size(); i++) {
        if (i > 0) os << " - ";
        os << camino[i];
      }
      os << "\n";

      os << "Costo: " << (camino.size() - 1) << "\n";
      return;
    }

    // --- Expandir vecinos ---
    for (const auto& arista : grafo_) {
      int vecino = -1;
      if (arista.GetNodo1() == nodo_actual) {
        vecino = arista.GetNodo2();
      } else if (arista.GetNodo2() == nodo_actual) {
        vecino = arista.GetNodo1();
      }
      if (vecino != -1 && visitados.find(vecino) == visitados.end()) {
        cola.push(vecino);
        visitados.insert(vecino);
        padres[vecino] = nodo_actual;
      }
    }
  }

  os << "No se encontró un camino desde " << nodo_inicio << " hasta "
     << nodo_fin << ".\n";
}

#endif  // GRAFO_H_