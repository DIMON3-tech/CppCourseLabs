#include <iostream>
#include <vector>
#include <variant>
#include <algorithm>
#include <fstream>
#include "string.h"
#include <string>
#include <cmath>

void FileToHemming(std::vector<bool>& help_vector, std::vector<bool>& vector_hemming, int user_bits) {
    std::vector<bool> support;
    support.push_back(false); // потом по 0 индексу запишу инфу о всех битах в этом векторе
    int help_index = 0;
    for (int i = 1; i < user_bits; ++i) {
        if (log2(i) == int(log2(i))) {
            support.push_back(false);
        } else {
            support.push_back(help_vector[help_index]);
            help_index++;
        }
    }

    int counter_mod_2 = 0; // counter mod2 for each parity bits
    int count_all_bits = 0; // for 0 parity bit in vector_ham

    for (int i = 1; i < help_vector.size(); i++) { // цикл проходит по всему help_vector
        if (log2(i) == int(log2(i))) { // если это parity bit начинаю его считать
            for (int j = i; j < user_bits; j += (2 * i)) { // делаю step = i (для ячейки 2 step 2)
                int index = j;
                for (int k = 0; k < i; ++k) {
                    counter_mod_2 += int(support[index]);
                    index++;
                }
            }
            support[i] = counter_mod_2 % 2;
            counter_mod_2 = 0;
        }
        count_all_bits += int(help_vector[i]);
    }
    support[0] = bool(count_all_bits % 2);
    for (int i = 0; i < support.size(); ++i) {
        vector_hemming.push_back(support[i]);
    }


    help_vector.resize(0);
}

std::string RecodeHemming(std::vector<bool>& decode_vector, std::vector<bool>& vector_hemming, int user_bits) {
    // нужно посчитать сколько исходя из user_bits у меня фейковых нулей и заигнорить их
    int count_parity_bits = log2(user_bits) + 2;
    int counter = 0;
    int count_initial_bits = user_bits - count_parity_bits;
    int chars = vector_hemming.size() / user_bits;
    int i = 1;
    for (int u = 0; u < chars; ++u) {
        for (int j = 0; j < count_initial_bits - 1; ++j) {
            if (log2(j) != int(log2(j))) {
                if (j != 0) {
                    decode_vector.push_back(vector_hemming[j]);
                }
            }

        }

        // delete first (count_initial_bits + parity_bits) elements from hemming_vector
        if (vector_hemming.size() != 0) {
            for (int j = 0; j < count_initial_bits + count_parity_bits; ++j) {
                vector_hemming.erase(vector_hemming.begin());
            }
        }

    }
    std::string s;
    std::string result;
    int number;
    int size = decode_vector.size() / 8;
    for (int y = 0; y < size; ++y) {
        for (int j = 0; j < 8; ++j) {
            if (decode_vector[j] == true) {
                s += '1';
            } else {
                s += '0';
            }
        }
        number = std::stoi(s, nullptr, 2);
        result += static_cast<char> (number);
        for (int j = 0; j < 8; ++j) {
            decode_vector.erase(decode_vector.begin());
        }
        s = "";

    }
    return result;

}

void CheckToCorrectHemming(std::vector<bool>& first_vector, std::vector<bool>& second_vector) {
    // size of each vector is equal
    for (int i = 0; i < first_vector.size(); i++) {
        int counter = 0;
        if (first_vector[i] xor second_vector[i]) {
            counter += i;
        }
    }
}

void read_bits(std::vector<bool>& vector_hemming, std::string& file_name, int user_bits) {
    std::ifstream file(file_name, std::ios_base::binary);
    if (!file.is_open()) {
        std::cout << "Error, file was not open" << std::endl;
    }

    int parity_bits = log2(user_bits) + 2;
    int max_chars = (user_bits - parity_bits) / 8;
    int initial_bits = user_bits - parity_bits;

    int chars = 0;
    char ch;
    std::vector<bool> help_vector;
    while (file >> ch) {
        if (chars <
            max_chars) { // если на текущий момент записано чаров меньше чем можно записать, значит записываем еще один
            for (int i = 7; i >= 0; i--) {
                bool bits = (ch >> i) & true;
                help_vector.push_back(bits);
            }

            chars++;
        } else {
            while (help_vector.size() < initial_bits) {
                help_vector.push_back(false);
            }

            FileToHemming(help_vector, vector_hemming, user_bits);

            for (int i = 7; i >= 0; i--) {
                bool bits = (ch >> i) & true;
                help_vector.push_back(bits);
            }
            chars = 1;
        }
    }
    while (help_vector.size() < initial_bits) {
        help_vector.push_back(false);
    }
    FileToHemming(help_vector, vector_hemming, user_bits);


}


int main() {
    std::string file = "C:\\Users\\dimab\\Downloads\\my_test_file_to_binary_read.txt";
    int user_bits = 0;
    std::cin >> user_bits;
    std::vector<bool> hemming_vector;
    read_bits(hemming_vector, file, user_bits);
//    for (int i = 0; i < hemming_vector.size(); ++i) {
//        std::cout << hemming_vector[i];
//    }
    std::vector<bool> decode_vector;
    std::string result;
    result = RecodeHemming(decode_vector, hemming_vector, user_bits);
    std::cout << result << std::endl;
//    for (int i = 0; i < decode_vector.size(); ++i) {
//        std::cout << decode_vector[i];
//    }

// 10000110100100000000 - b отдельно
}





