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

#include <iostream>
#include <map>
#include <set>

class Grafo {
 public:
	Grafo() { }
	Grafo(std::set<Arista> grafo) : vertices(grafo.size()) { grafo_ = grafo; }
	
 private:
	std::set<Arista> grafo_;
	int vertices = grafo_.size();

};


class Arista {
 public:
	
  Arista(int nodo1, int nodo2, double coste) : nodo1_(nodo1), nodo2_(nodo2), coste_(coste) { }
	
	
	//Getters
	inline int GetNodo1() { return nodo1_;}
	inline int GetNodo2() { return nodo2_;}
	inline int GetCoste() { return coste_;}

	//atributos privados
 private:
  int nodo1_;
	int nodo2_;
	double coste_;
};


#endif