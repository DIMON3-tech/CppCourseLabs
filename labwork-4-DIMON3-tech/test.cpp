#include <iostream>
#include <cstring>
#include <vector>
#include <bits/stdc++.h>
#include <fstream>
#include <bitset>
void RecodeFileFromHemming(std::vector<bool> hamming_vector) { // раскодирование
    std::vector<bool> help_vector;
    for(int i = 0; i < hamming_vector.size(); i++) {
        if(log2(i) == int(log2(i))) { // если это parity bits
            help_vector.push_back(hamming_vector[i]);
        }
    }
}

void FileToHemming(std::vector<bool>& help_vector, std::vector<bool>& vector_ham, int count_user_bits) {
    // help_vector содержит биты только тех чаров, которые мы можем закодировать в одном блоке и доп нули, если они есть
    // vector_ham - пустой, в него будем складывать закодированные хэммингом биты (в том числе кодирует доп нули)

    int help_vector_index = 0;
    vector_ham.push_back(false); // потом по 0 индексу запишу инфу о всех битах в этом векторе
    for(int i = 1; i < count_user_bits; i++) { // с единицы, так как 0 не является степенью 2 и перед циклом добавлено доп ячейка которая и будет 0 ячейкй вектора
        if(log2(i) == int(log2(i))) {
            vector_ham.push_back(false);
        } else {
            vector_ham.push_back(help_vector[help_vector_index]);
            help_vector_index++;
        }
    }

    int counter = 0; // counter % for each parity bits
    int count_all_bits = 0; // for 0 index in vector_ham

    // считаем ТОЛЬКО parity bits
    for(int i = 1; i < help_vector.size(); i++) { // цикл проходит по всему help_vector
        if(log2(i) == int(log2(i))){ // если это parity bit начинаю его считать
            for(int j = i + 1; j < vector_ham.size(); j+=i) { // делаю step = i (для ячейки 2 step 2)
                for(int t = j; t < j + i; t++){ // считываю i битов подряд (для ячейки 2 считал 2 бита подряд, потом через 2 еще 2 подряд считал и тд)
                    counter += int(help_vector[t]);
                }
            }
            vector_ham[i] = counter % 2;
            counter = 0;
        }
        count_all_bits += int(help_vector[i]);
    }
    vector_ham[0] = count_all_bits % 2;
    for(int i = 0; i < vector_ham.size(); i++) {
        std::cout << vector_ham[i];
    }
    std::cout << std::endl;
    vector_ham.resize(0);
}
void CharsToBits(std::vector<bool>& vector_ham, std::vector<bool>&, std::string& file, int count_user_bits) {
    std::ifstream f(file, std::ios_base::binary);
    if (!f) {
        std::cout << "Error, file was not open" << std::endl;
    }

    int count_of_parity_bits = log2(count_user_bits) + 2; // сколько parity bits потребуется для кодирования (вектор из parity bits + из изначальных битов)
    int count_chars = 0;
    int count_chars_to_ham = ((count_user_bits - count_of_parity_bits) / 8); // count_chars_to_ham - это сколько чаров мы сможем поместить в блок
    int count_initial_bits = count_user_bits - count_of_parity_bits;

    char ch;
    std::vector<bool> help_vector; // вектор в который я положу count_chars_to_ham раз чары переведенные в 2СС
    while (f >> ch) {
        if(count_chars < count_chars_to_ham) { // если на текущий момент записано чаров меньше чем можно записать, значит записываем еще один
            for (int i = 7; i >= 0; i--) {
                bool bits = (ch >> i) & true;
                help_vector.push_back(bits);
            }
            count_chars++ ;// увеличиваем колиество уже записанных чаров
        } else{
            while(help_vector.size() < count_initial_bits) { // если в векторе есть еще пустые места (и чар больше не помещается), запихиваем туда нули
                help_vector.push_back(false);
            }

            FileToHemming(help_vector, vector_ham, count_user_bits);


            help_vector.resize(0);
            // записали блок битов, где на первый count_chars * 8 лежат закодированные чары (0 и 1, который потом будем кодировать хэммингом)
            for (int i = 7; i >= 0; i--) { // на случай, когда не можем записать в текущий блок еще один чар и создаем новый (туда первым пойдет текуцщий чар)
                bool bits = (ch >> i) & true;
                help_vector.push_back(bits);
            }
            count_chars = 1; // готовы записывать новый блок (уже один чар в этом блоке есть)
        }
//        for(int i = 0; i < help_vector.size(); i++) {
//            std::cout << help_vector[i];
//        } std::cout << std::endl;
    }
    // одтельно проделываем операцию отправки в функцию create_hemming для последнего считанного элемента
    while(help_vector.size() < count_initial_bits) { // если в векторе есть еще пустые места (и чар больше не помещается), запихиваем туда нули
        help_vector.push_back(false);
    }
    FileToHemming(help_vector, vector_ham, count_initial_bits);

}
void CheckToCorrectHemming(std::vector<bool>& vector_to_check) {
    // size of each vector is equal (равны)
    for(int i = 0; i < vector_to_check.size(); i++) {
        int counter = 0;
        if(vector_to_check[i] xor vector_to_check[i]) { // если биты различаются, значит ошибка

            counter += i;
        }
    }
}

struct HeaderOfArchive {
    bool archive_or_file; // file=false, archive=true
    std::string name; // name of file or archive (it depends on archive_or_file value)
    int block_size = 0;

};
int main() {
    int i = 8;
    if(log2(i) == int(log2(i))){
        std::cout << "slkdfj";
    }


}