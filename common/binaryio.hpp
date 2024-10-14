//
// Created by liang on 4/10/24.
//

#ifndef BINARYIO_HPP
#define BINARYIO_HPP

#include <string>
#include <vector>

class BinaryIO {
    public:
    // Lee un archivo binario y devuelve su contenido en un vector de bytes
    static std::vector<uint8_t> readBinaryFile(const std::string& filename);

    // Escribe el contenido en un archivo binario desde un vector de bytes
    static void writeBinaryFile(const std::string& filename, const std::vector<uint8_t>& data);
};

#endif // BINARYIO_HPP
