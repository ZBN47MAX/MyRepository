#include "pch.h"
#include "GameManager.h"

int main(int argc, char* argv[]) {
    
    create_output_directories();

    GameManager manager;

    std::cout << std::string(50, '=') << std::endl;
    std::cout << ">>> PRISONER'S DILEMMA SIMULATOR" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    try {
        // get CLI and run
        parse_args(argc, argv, manager);
    }
    catch (const std::exception& e) {
        std::cout << "!!! Unexpected error: " << e.what() << std::endl;
        // output with errors
        manager.write_output_file();
        show_help();
        return 1;
    }

    
    std::cout << "\n";
    show_help();

    return 0;
}