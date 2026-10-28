#include "ConsoleUI.h"
#include "Store.h"

#include <exception>
#include <iostream>

int main() {
    try {
        Store store("data");
        ConsoleUI ui;
        ui.run(store);
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
