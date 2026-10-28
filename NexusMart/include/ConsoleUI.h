#ifndef NEXUSMART_CONSOLEUI_H
#define NEXUSMART_CONSOLEUI_H

#include "Store.h"

#include <string>
#include <vector>

class ConsoleUI {
private:
    static bool colorsEnabled_;
    static bool inputClosed_;

    static void enableAnsiColors();
    static std::string color(const std::string& text, const std::string& code);
    static std::string readLine(const std::string& prompt);
    static int readInt(const std::string& prompt, int minimum, int maximum);
    static double readDouble(const std::string& prompt, double minimum);
    static std::string readRequiredText(const std::string& prompt, std::size_t maxLength = 120);
    static void pause();
    static void clearScreen();
    static void showBanner();
    static void printHeader(const std::string& title, const std::string& subtitle = "");
    static void showResult(bool success, const std::string& message);
    static std::string truncate(const std::string& text, std::size_t width);
    static std::string money(double amount);

    static void printProducts(const std::vector<Product>& products);
    static void browseProducts(Store& store, Customer& customer,
                               const std::vector<Product>& products,
                               const std::string& title);
    static void searchProducts(Store& store, Customer& customer);
    static void customerMenu(Store& store, Customer& customer);
    static void adminMenu(Store& store, Admin& admin);
    static void showCart(Store& store, Customer& customer);
    static void showCustomerOrders(const Store& store, const Customer& customer);
    static void showAllOrders(const Store& store);
    static void addProductFlow(Store& store);
    static void updateProductFlow(Store& store);
    static void removeProductFlow(Store& store);
    static void updateOrderFlow(Store& store);
    static void showStatistics(const Store& store);

    // Small function template: the same iteration helper can render any collection.
    template <typename Container, typename Printer>
    static void printCollection(const Container& items, Printer printer) {
        for (const auto& item : items) {
            printer(item);
        }
    }

public:
    void run(Store& store);
};

#endif
