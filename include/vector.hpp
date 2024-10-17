#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <string>
#include <memory>

template <typename T>
class MathVector {
private:
    std::vector<T> items;  

public:
    MathVector(int dimension) : items(dimension) {}

    MathVector(int dimension, const T* values) : items(values, values + dimension) {}

    int dimension() const { return items.size(); }

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

    static std::unique_ptr<std::vector<std::unique_ptr<MathVector<T>>>> init_from_file(const std::string& file_name, int& num_read_vectors, int max_vectors);

    static std::unique_ptr<std::vector<int>> query_solutions(const std::string& file_name, int query_index);
};

template <typename T>
float MathVector<T>::euclidean_distance(const MathVector<T>& other) const {
    if (items.size() != other.dimension()) {
        throw std::invalid_argument("Vectors must have the same dimension.");
    }

    float sum = 0.0;
    for (int i = 0; i < items.size(); i++) {
        float diff = static_cast<float>(items[i]) - static_cast<float>(other[i]);
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

template <typename T>
std::unique_ptr<std::vector<std::unique_ptr<MathVector<T>>>> MathVector<T>::init_from_file(
    const std::string& file_name, int& num_read_vectors, int max_vectors) {

    std::ifstream file(file_name, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error opening file: " + file_name);
    }

    auto vectors = std::make_unique<std::vector<std::unique_ptr<MathVector<T>>>>();
    num_read_vectors = 0;

    while (!file.eof() && num_read_vectors < max_vectors) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) {
            break;  
        }

        auto vector = std::make_unique<MathVector<T>>(dimension);

        if (!file.read(reinterpret_cast<char*>(&vector->items[0]), dimension * sizeof(T))) {
            throw std::runtime_error("Error reading vector data from file");
        }

        vectors->push_back(std::move(vector));
        num_read_vectors++;
    }

    file.close();
    return vectors; 
}

template <typename T>
std::unique_ptr<std::vector<int>> MathVector<T>::query_solutions(const std::string& file_name, int query_index) {
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

    auto solution_indices = std::make_unique<std::vector<int>>(dimension);
    if (!file.read(reinterpret_cast<char*>(&(*solution_indices)[0]), dimension * sizeof(int))) {
        throw std::runtime_error("Error reading solution indices from file.");
    }

    file.close();

    return solution_indices; 
}

#endif 
