#include <iostream>
#include <filesystem>
#include <cmath>
#include <vector>
#include <vector>
#include <fstream>
#include "string.h"
#include <string>
#include <cmath>
#include <bitset>

namespace fs = std::filesystem;

struct File {
    std::string file_name;
    unsigned long offset;
    std::vector<bool> hemming_code;
};

struct Archive {
public:
    std::vector<bool> files;
    std::string archive_name;

};

std::vector<bool> FromHemmingToVector(std::vector<bool>& vector_hemming) {
    int user_bits = 20;
    std::vector<bool> decode_vector;
    std::bitset<8> bitset;
    // нужно посчитать сколько исходя из user_bits у меня фейковых нулей и заигнорить их
    int count_parity_bits = log2(user_bits) + 2;
    int count_initial_bits = user_bits - count_parity_bits;
    int chars = vector_hemming.size() / user_bits;

    for (int j = 0; j < count_initial_bits - 1; ++j) {
        if (log2(j) != int(log2(j))) {
            if (j != 0) {
                decode_vector.push_back(vector_hemming[j]);
            }
        }

    }
    return decode_vector;
}

std::bitset<8> FromHemmingToBitset(std::vector<bool>& vector_hemming) {
    int user_bits = 20;
    std::vector<bool> decode_vector;
    std::bitset<8> bitset;
    // нужно посчитать сколько исходя из user_bits у меня фейковых нулей и заигнорить их
    int count_parity_bits = log2(user_bits) + 2;
    int count_initial_bits = user_bits - count_parity_bits;
    int chars = vector_hemming.size() / user_bits;

    for (int j = 0; j < count_initial_bits - 1; ++j) {
        if (log2(j) != int(log2(j))) {
            if (j != 0) {
                decode_vector.push_back(vector_hemming[j]);
            }
        }

    }

    for (int k = decode_vector.size() - 1; k >= 0; k--) { // write bitset
        bitset[8 - k - 1] = decode_vector[k];
    }
    return bitset;

}

File DecodeFileHemming(std::vector<bool>& vector_hemming) {
    bool is_file;
    int uset_bit = 20;
    std::bitset<8> bitset;
    unsigned long offset_;
    unsigned long size_internal_info;
    std::string filename;
    std::vector<bool> internal_info;
    std::vector<bool> support;
    while (vector_hemming.size() != 0) {
        is_file = vector_hemming[0]; // get type of structure (file or archive)
        vector_hemming.erase(vector_hemming.begin(), vector_hemming.begin() + 1);
        if (is_file) {

            // get offset = length of name
            for (int i = 0; i < 20; ++i) {
                support.push_back(vector_hemming[i]);
                vector_hemming.erase(vector_hemming.begin(), vector_hemming.begin() + 1);
            }
            bitset = FromHemmingToBitset(support);
            support.resize(0);
            unsigned long f = bitset.to_ulong();
            char ch = static_cast<char> (f);
            offset_ = ch - '0';
            // get filename
            for (int i = 0; i < (offset_ * uset_bit); i++) {
                support.push_back(vector_hemming[i]);
                vector_hemming.erase(vector_hemming.begin(), vector_hemming.begin() + 1);
                if (support.size() == 8) {
                    bitset = FromHemmingToBitset(support);
                    unsigned long n = bitset.to_ulong();
                    char c = static_cast<char> (n);
                    filename += c;
                    support.resize(0);
                }
            }

            // get size of internal info
            for (int i = 0; i < 20; i++) {
                support.push_back(vector_hemming[i]);
                vector_hemming.erase(vector_hemming.begin(), vector_hemming.begin() + 1);
            }
            bitset = FromHemmingToBitset(support);
            support.resize(0);
            size_internal_info = bitset.to_ulong();


            // get internal info of file
            for (int i = 0; i < size_internal_info; i++) {
                support.push_back(vector_hemming[i]);
                vector_hemming.erase(vector_hemming.begin(), vector_hemming.begin() + 1);
            }
            internal_info = FromHemmingToVector(support);
            support.resize(0);
        }
    }
    File file;
    file.offset = offset_;
    file.file_name = filename;
    file.hemming_code = internal_info;
    return file;
}


bool DecodeArchive(std::vector<bool>& vector_hemming, Archive& archive) {
    bool is_archive;
    int uset_bit = 20;
    int length_name;
    std::string archive_name;
    std::bitset<8> bitset;
    std::vector<bool> internal_info;
    std::vector<bool> support;
    is_archive = vector_hemming[0];
    vector_hemming.erase(vector_hemming.begin(), vector_hemming.begin() + 1);
    if (is_archive) {
        while (vector_hemming.size() != 0) {
            // get length_name = length of name
            for (int i = 0; i < 20; ++i) {
                support.push_back(vector_hemming[i]);
                vector_hemming.erase(vector_hemming.begin(), vector_hemming.begin() + 1);
            }
            bitset = FromHemmingToBitset(support);
            support.resize(0);
            unsigned long f = bitset.to_ulong();
            char ch = static_cast<char> (f);
            length_name = ch - '0';
            // get filename
            for (int i = 0; i < (length_name * uset_bit); i++) {
                support.push_back(vector_hemming[i]);
                vector_hemming.erase(vector_hemming.begin(), vector_hemming.begin() + 1);
                if (support.size() == 8) {
                    bitset = FromHemmingToBitset(support);
                    unsigned long n = bitset.to_ulong();
                    char c = static_cast<char> (n);
                    archive_name += c;
                    support.resize(0);
                }
            }

            // get internal info of archive (files)
            internal_info = FromHemmingToVector(vector_hemming);
            support.resize(0);

        }
        archive.files = internal_info;
        archive.archive_name = archive_name;
        return true;
    }
    return false; // it's not an archive
}

void ToCodeHemming(std::vector<bool>& help_vector, std::vector<bool>& vector_hemming, int user_bits) {
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

void DecodeHemming(std::vector<bool>& decode_vector, std::vector<bool> vector_hemming, int number_of_file) {
    int user_bits = 20;
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

    std::bitset<8> bitset;
    char element;
    std::string result;
    int size = decode_vector.size() / 8; // how many chars we can read and write to file
    for (int j = 0; j < size; ++j) {
        for (int k = decode_vector.size() - 1; k > 0; k--) { // write bitset
            bitset[8 - k - 1] = decode_vector[k];
        }

        unsigned long n = bitset.to_ulong();
        element = static_cast<char> (n);
        result += element;
    }

//    std::string str = "D:\\Labworks_Proga\\lab4\\labwork-4-DIMON3-tech\\file_from_archive";
//    std::string filename = static_cast<std::string>(str) + std::to_string(number_of_file) + ".txt";
//    std::ofstream fout(filename, std::ios::binary);
//
//    fout.write((char *) &result, sizeof(result));
//    fout.close();

}

void CheckToCorrectHemming(std::vector<bool>& first_vector, std::vector<bool>& second_vector) {
    // size of each vector is equal
    // first vector - правильный вектор, second vector - вектор для проверки
    int parity_bits = 0;
    int counter = 0;
    for (int i = 0; i < first_vector.size(); i++) {
        if (first_vector[i] xor second_vector[i]) {
            counter += i;
            parity_bits++;
        }
    }
    if (parity_bits > 1) {
        std::cout << "There are more than one error in file, can not fix" << std::endl;
        return;
    }
    if (second_vector[counter] == true) {
        second_vector[counter] = false;
    } else {
        second_vector[counter] = true;
    }
    std::cout << "Error was fixed" << std::endl;


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

            ToCodeHemming(help_vector, vector_hemming, user_bits);

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
    ToCodeHemming(help_vector, vector_hemming, user_bits);


}

// парсер работает

struct Variables {
    std::vector<std::string> files_to_extract; // -x --extract extract files from archive
    std::vector<std::string> files_to_delete; // -d --delete
    std::vector<std::string> files_to_append; // -a --append add files in archive
    bool create_new_archive = false; // -c --create create new archive
    bool print_all_files_in_archive = false;
    bool merge_two_archives = false;
    std::string archive_name; // -f --file=[Archive] name of new archive
    std::vector<std::string> archives_to_merge;
} variables;

void ConsoleRead(int argc, char** argv) {
    std::string filename; // help string for write some filename
    std::string name_first_archive; // string for write filenames archives to merge
    std::string name_second_archive; // string for write filenames archives to merge

    for (int i = 1; i < argc; i++) {
        bool short_command = false;
        bool long_command = false;
        if (*argv != nullptr) {
            // for command -x --extract
            if (argv[i][0] == '-') {
                if (i + 1 < argc) {
                    if (argv[i][1] == 'x') {
                        short_command = true;
                    } else if (argv[i][2] != char(0)) {
                        if (argv[i][2] == 'e') {
                            long_command = true;
                        }
                    }
                }
                if (long_command or short_command) {
                    int index = i + 1;
                    while (fs::exists(argv[index])) {
                        variables.files_to_extract.push_back((std::string) argv[index]);
                        if (index + 1 < argc) {
                            index++;
                        } else {
                            break;
                        }
                    }
                    long_command = false;
                    short_command = false;
                    continue;
                }
            }
            if (argv[i][0] == '-') {
                // for command -c --create

                if (argv[i][1] != char(0)) {
                    if (argv[i][1] == 'c') {
                        short_command = true;
                    } else if (argv[i][2] != char(0) && argv[i][3] != char(0)) {
                        if (argv[i][2] == 'c' && argv[i][3] == 'r') {
                            long_command = true;
                        }
                    }
                }
                if (long_command or short_command) {
                    variables.create_new_archive = true;
                    long_command = false;
                    short_command = false;
                    continue;
                }
            }
            if (argv[i][0] == '-') {
                // for command -f --file
                if (argv[i][1] == 'f') {
                    if (argv[i][1] != char(0)) {
                        variables.archive_name = argv[i + 1];
                    }
                    // for short command
                    if (i + 2 < argc) {
                        // there are names of files and I
                        // have to add their to vector
                        if (argv[i + 2][0] != '-') {
                            int index = i + 2;
                            while (index < argc and fs::exists(argv[index])) {
                                variables.files_to_append.push_back((std::string) argv[index]);
                                if (index + 1 < argc) {
                                    index++;
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
                if (argv[i][2] != char(0)) {
                    // for long command -- file
                    if (argv[i][2] == 'f') {
                        int index = 7; // because filename looks like [archive_name] and I skip []
                        std::string string;
                        while (argv[i][index] != '-' and argv[i][index] !=
                                                         char(0)) { // read files names while there is no other command or meet - (start other command)
                            string += argv[i][index];
                            index++;
                        }
                        variables.archive_name = string;
                        if (argv[i + 1][0] != '-') {
                            int new_index = i + 1;
                            while (fs::exists(argv[new_index])) {
                                variables.files_to_append.push_back((std::string) argv[new_index]);
                                if (new_index + 1 < argc) {
                                    new_index++;
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (argv[i][0] == '-') {
                // for command -l --list
                if (argv[i][1] == 'l') {
                    short_command = true;
                }
                if (argv[i][2] == 'l') {
                    long_command = true;
                }


                if (short_command or long_command) {
                    variables.print_all_files_in_archive = true;
                    short_command = false;
                    long_command = false;
                    continue;
                }
            }
            if (argv[i][0] == '-') {
                // for command -a --append
                if (argv[i][1] == 'a') {
                    short_command = true;
                }
                if (argv[i][2] == 'a') {
                    long_command = true;
                }


                if (long_command or short_command) {
                    int index = i + 1;
                    while (fs::exists(argv[index])) {
                        variables.files_to_append.push_back((std::string) argv[index]);
                        if (index + 1 < argc) {
                            index++;
                        } else {
                            break;
                        }
                    }
                    long_command = false;
                    short_command = false;
                    continue;
                }
            }

            if (argv[i][0] == '-') {
                // for command -d --delete
                if (argv[i][1] == 'd') {
                    short_command = true;
                } else if (argv[i][2] != char(0)) {
                    if (argv[i][2] == 'd') {
                        long_command = true;
                    }
                }

                if (long_command or short_command) {
                    int index = i + 1;
                    while (fs::exists(argv[index])) {
                        variables.files_to_delete.push_back((std::string) argv[index]);
                        if (index + 1 < argc) {
                            index++;
                        } else {
                            break;
                        }
                    }
                    long_command = false;
                    short_command = false;
                    continue;
                }
            }
            if (argv[i][0] == '-') {
                // for command -A --concatenate
                if (argv[i][1] == 'A') {
                    short_command = true;
                }
                if ((argv[i][2] == 'c') && (argv[i][3] = 'o')) {
                    long_command = true;
                }


                if (short_command or long_command) {
                    variables.merge_two_archives = true;
                    variables.archives_to_merge.push_back(argv[i + 1]);
                    variables.archives_to_merge.push_back(argv[i + 2]);
                    short_command = false;
                    long_command = false;
                    continue;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    ConsoleRead(argc, argv);
    int user_bits = 20;
    std::vector<bool> all_info;
    if (variables.create_new_archive == true) {
        // for each file write type, length of name, size of internal info
        for (int i = 0; i < variables.files_to_append.size(); ++i) {
            char type = '1';
            int length_of_name = variables.files_to_append[i].length();
            std::vector<bool> all_info_in_hemming_code;
            std::vector<bool> internal_info;

            // code in hemming internal info
            read_bits(internal_info, variables.files_to_append[i], 20);
            int size_of_internal = internal_info.size();

            // write type in hemming
            for (int j = 7; j >= 0; j--) {
                bool bits = (type >> j) & true;
                all_info.push_back(bits);
            }

            // write an int of name length
            std::bitset<8> length_name(length_of_name);
            for (int j = 0; j < length_name.size(); ++j) {
                all_info.push_back(length_name[j]);
            }

            // write name in hemming
            std::vector<bool> names_in_hemming;
            for (int k = 0; k < variables.files_to_append[i].length(); ++k) {
                for (int j = 7; j >= 0; j--) {
                    bool bits = (variables.files_to_append[i][k] >> j) & true;
                    names_in_hemming.push_back(bits);
                }
                ToCodeHemming(names_in_hemming, all_info, 20);
            }

            // write size_of_internal info
            std::bitset<8> size_internal_info(size_of_internal);
            for (int j = 0; j < size_internal_info.size(); ++j) {
                all_info.push_back(size_internal_info[j]);
            }

            // write an internal info
            for (int j = 0; j < internal_info.size(); ++j) {
                all_info.push_back(internal_info[j]);
            }


        }
        // write an archive

        char type_archive = '1';
        int archive_name_length = variables.archive_name.length();
        int size_archive_internal_info = all_info.size();
        std::vector<bool> archive_header;

        // write type in hemming
        for (int j = 7; j >= 0; j--) {
            bool bits = (type_archive >> j) & true;
            archive_header.push_back(bits);
        }

        // write an int of archive name length
        std::bitset<8> length_name_archive(archive_name_length);
        for (int j = 0; j < length_name_archive.size(); ++j) {
            archive_header.push_back(length_name_archive[j]);
        }

        // write size_of_internal info
        std::bitset<8> size_internal_info(size_archive_internal_info);
        for (int j = 0; j < size_internal_info.size(); ++j) {
            archive_header.push_back(size_internal_info[j]);
        }

        // write an internal info
        for (int j = 0; j < all_info.size(); ++j) {
            archive_header.push_back(all_info[j]);
        }


        std::string full_name_of_archive = static_cast<std::string> (variables.archive_name) + ".haf";
        std::ofstream fout(full_name_of_archive, std::ios::binary);

        char c;
        for (int j = 0; j < archive_header.size(); ++j) {
            if (archive_header[j]) {
                c = '1';
            } else {
                c = '0';
            }
            fout.write((char*) &c, sizeof(char));
        }
        all_info.resize(0);

    }

    if (variables.files_to_delete.size() != 0) {
        std::ifstream fin(variables.archive_name, std::ios::binary);
        char ch;
        std::vector<bool> archive_data;
        while (fin >> ch) {
            for (int j = 7; j >= 0; j--) {
                bool bits = (ch >> j) & true;
                archive_data.push_back(bits);
            }
        }
        Archive archive; // to write information
        DecodeArchive(archive_data, archive);
    }
    if (variables.merge_two_archives == true) {
        Archive first_archive; // archive whick will contain second archive
        std::ifstream fin(variables.archive_name, std::ios::binary);
        char ch;
        std::vector<bool> archive_data;
        while (fin >> ch) {
            for (int j = 7; j >= 0; j--) {
                bool bits = (ch >> j) & true;
                archive_data.push_back(bits);
            }
        }
        DecodeArchive(archive_data, first_archive);

        Archive second_archive;
        std::ifstream ifstream(variables.archive_name, std::ios::binary);
        std::vector<bool> second_archive_data;
        while (ifstream >> ch) {
            for (int j = 7; j >= 0; j--) {
                bool bits = (ch >> j) & true;
                second_archive_data.push_back(bits);
            }
        }
        DecodeArchive(archive_data, second_archive);
        for (int i = 0; i < first_archive.files.size(); ++i) {
            first_archive.files.push_back(second_archive.files[i]);
        }

    }

    if (variables.files_to_append.size() != 0) {
        std::ifstream fin(variables.archive_name, std::ios::binary);
        char ch;
        std::vector<bool> archive_data;
        while (fin >> ch) {
            for (int j = 7; j >= 0; j--) {
                bool bits = (ch >> j) & true;
                archive_data.push_back(bits);
            }
        }
        Archive archive; // to write information
        DecodeArchive(archive_data, archive);

        // code to hemming new files
        for (int i = 0; i < variables.files_to_append.size(); ++i) {
            char type = '1';
            int length_of_name = variables.files_to_append[i].length();
            std::vector<bool> all_info_in_hemming_code;
            std::vector<bool> internal_info;

            // code in hemming internal info
            read_bits(internal_info, variables.files_to_append[i], 20);
            int size_of_internal = internal_info.size();

            // write type in hemming
            for (int j = 7; j >= 0; j--) {
                bool bits = (type >> j) & true;
                all_info.push_back(bits);
            }

            // write an int of name length
            std::bitset<8> length_name(length_of_name);
            for (int j = 0; j < length_name.size(); ++j) {
                all_info.push_back(length_name[j]);
            }

            // write name in hemming
            std::vector<bool> names_in_hemming;
            for (int k = 0; k < variables.files_to_append[i].length(); ++k) {
                for (int j = 7; j >= 0; j--) {
                    bool bits = (variables.files_to_append[i][k] >> j) & true;
                    names_in_hemming.push_back(bits);
                }
                ToCodeHemming(names_in_hemming, all_info, 20);
            }

            // write size_of_internal info
            std::bitset<8> size_internal_info(size_of_internal);
            for (int j = 0; j < size_internal_info.size(); ++j) {
                all_info.push_back(size_internal_info[j]);
            }

            // write an internal info
            for (int j = 0; j < internal_info.size(); ++j) {
                all_info.push_back(internal_info[j]);
            }
        }

        // append old files for new files (and getting size of all archive data and hemming it)
        int archive_data_size = archive.files.size() + all_info.size();
        std::bitset<8> size_of_new_internal(archive_data_size);
        std::vector<bool> help_vector;
        for (int i = 0; i < size_of_new_internal.size(); ++i) {
            help_vector.push_back(size_of_new_internal[i]);
        }
        std::vector<bool> hemming;
        ToCodeHemming(help_vector, hemming, 20);

        int old_length_name = 0;
        int old_size_internal = 0;
        std::bitset<8> old_bitset;
        std::vector<bool> support;

        // find in old data archive size of internal info and rewrite it
        for (int i = 1; i < 21; ++i) {
            if (support.size() == 0) {
                old_bitset = FromHemmingToBitset(support);
                support.resize(0);
                unsigned long n = old_bitset.to_ulong();
                char old_for_cast = static_cast<char> (old_length_name);
                old_length_name = (int) old_for_cast - '0';

            }
            support.push_back(archive.files[i]);
        }
        int index_of_start_next_int = 21 + old_length_name;

        // rewrite old size of internal
        for (int i = index_of_start_next_int; i < index_of_start_next_int + 20; ++i) {
            archive.files[i] = size_of_new_internal[i];
        }

        // push new internal info
        for (int i = 0; i < all_info.size(); ++i) {
            archive.files.push_back(all_info[i]);
        }

        all_info.resize(0);

        // pushing new archive without old files
        std::string full_name_of_archive = static_cast<std::string> (variables.archive_name) + ".haf";
        std::ofstream fout(full_name_of_archive, std::ios::binary);

        char c;
        for (int j = 0; j < archive.files.size(); ++j) {
            if (archive.files[j]) {
                if (archive.files[j]) {
                    c = '1';
                } else {
                    c = '0';
                }
                fout.write((char*) &c, sizeof(char));
            }

        }

    }
}
