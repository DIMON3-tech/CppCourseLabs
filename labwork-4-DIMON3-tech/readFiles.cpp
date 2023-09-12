#include <iostream>
#include <vector>
#include <filesystem>

void get_files(std::vector<std::string>& paths, const std::string& current_paths){

    for(const auto& file : std::filesystem::directory_iterator(current_paths)){
        if(std::filesystem::is_directory(file)){
            get_files(paths, file.path().string());
        }
        else {
            paths.push_back(file.path().string());
        }
    }
}

int main(){
    system("chcp 65001");
    std::vector<std::string> paths;
    get_files(paths, "D:\\git labwork1");
    for(int i = 0; i < paths.size();i++) std::cout << paths[i] << std::endl;
}
// работает