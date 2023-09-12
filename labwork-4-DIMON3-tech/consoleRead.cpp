#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;


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
                }
            }
            if (argv[i][0] == '-') {
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
                }
            }
            if (argv[i][0] == '-') {
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
                    if (argv[i][2] == 'f') {
                        int index = 8; // because filename looks like [archive_name] and I skip []
                        std::string string;
                        while (argv[i][index] != '-' and argv[i][index] !=
                                                         char(0)) { // reaf files names while there is no other command or meet - (start other command)
                            string += argv[i][index];
                            index++;
                        }
                        variables.archive_name = string;
                        if (argv[i + 2][0] != '-') {
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
                }
            }
            if (argv[i][0] == '-') {
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
                }
            }
            if (argv[i][0] == '-') {
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
                }
            }
            if (argv[i][0] == '-') {
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
                }
            }
        }
    }
}
//int main(int argc, char* argv[]){
//    ConsoleRead(argc, argv);
//    std::cout << variables.merge_two_archives << std::endl;
//    std::cout << variables.archive_name << std::endl;
//    for(int i = 0; i < variables.archives_to_merge.size(); i++){
//        std::cout << variables.archives_to_merge[i];
//    }
//
//
//
//}
