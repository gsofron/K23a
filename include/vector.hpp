#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <string>

template <typename T>
class MathVector {
private:
    std::vector<T> items;

public:
    MathVector(int dimension) : items(dimension) {}
    MathVector(int dimension, const T* values) : items(values, values + dimension) {}

    size_t dimension() const { return items.size(); }
    bool operator==(const MathVector<T>& other) const { return items == other.items; }

    float euclidean_distance(const MathVector<T>& other) const;

    friend std::ostream& operator<<(std::ostream& os, const MathVector<T>& vec) {
        os << "( ";
        for (size_t i = 0; i < vec.items.size(); i++) {
            os << +vec.items[i];
            if (i < vec.items.size() - 1) os << ", ";
        }
        os << " )";
        return os;
    }

    static std::vector<MathVector<T>*>* init_from_file(const std::string& file_name, int& num_read_vectors, int max_vectors);
    static std::vector<int>* query_solutions(const std::string& file_name, int query_index);


};

template <typename T>
float MathVector<T>::euclidean_distance(const MathVector<T>& other) const {
    if (items.size() != other.dimension()) {
        throw std::invalid_argument("Vectors must have the same dimension.");
    }

    float sum = 0.0;
    for (size_t i = 0; i < items.size(); i++) {
        float diff = static_cast<float>(items[i]) - static_cast<float>(other.items[i]);
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

template <typename T>
void destroy_vector(std::vector<MathVector<T>*>* vec) {
    if (vec) {
        for (auto* v : *vec) {
            delete v;
        }
        delete vec;
    }
}

template <typename T>
std::vector<MathVector<T>*>* MathVector<T>::init_from_file(
    const std::string& file_name, int& num_read_vectors, int max_vectors) {

    std::ifstream file(file_name, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error opening file: " + file_name);
    }

    auto* vectors = new std::vector<MathVector<T>*>();
    num_read_vectors = 0;

    while (!file.eof() && num_read_vectors < max_vectors) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) {
            break;
        }

        MathVector<T>* vector = new MathVector<T>(dimension);

        if (!file.read(reinterpret_cast<char*>(&vector->items[0]), dimension * sizeof(T))) {
            delete vector;  
            throw std::runtime_error("Error reading vector data from file");
        }

        vectors->push_back(vector);
        num_read_vectors++;
    }

    file.close();
    return vectors;
}

template <typename T>
std::vector<int>* MathVector<T>::query_solutions(const std::string& file_name, int query_index) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error opening file: " + file_name);
    }

    int dimension;
    if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) {
        throw std::runtime_error("Error reading vector dimension from file.");
    }

    long start_byte = (query_index * ((dimension + 1) * sizeof(int))) + sizeof(int);
    file.seekg(start_byte, std::ios::beg);
    if (!file) {
        throw std::runtime_error("Error seeking to the required byte position.");
    }

    auto* solution_indices = new std::vector<int>(dimension);
    if (!file.read(reinterpret_cast<char*>(&(*solution_indices)[0]), dimension * sizeof(int))) {
        delete solution_indices; 
        throw std::runtime_error("Error reading solution indices from file.");
    }

    file.close();
    return solution_indices;
}

#endif
