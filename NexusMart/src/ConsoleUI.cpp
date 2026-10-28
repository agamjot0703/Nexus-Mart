#include "ConsoleUI.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

bool ConsoleUI::colorsEnabled_ = true;
bool ConsoleUI::inputClosed_ = false;

void ConsoleUI::enableAnsiColors() {
#ifdef _WIN32
    HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (outputHandle == INVALID_HANDLE_VALUE || !GetConsoleMode(outputHandle, &mode)) {
        colorsEnabled_ = false;
        return;
    }
    if (!SetConsoleMode(outputHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        colorsEnabled_ = false;
        return;
    }
#endif
}

std::string ConsoleUI::color(const std::string& text, const std::string& code) {
    if (!colorsEnabled_) return text;
    return code + text + "\033[0m";
}

std::string ConsoleUI::readLine(const std::string& prompt) {
    std::cout << color(prompt, "\033[96m");
    std::string line;
    if (!std::getline(std::cin, line)) {
        inputClosed_ = true;
        return {};
    }
    const auto first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = line.find_last_not_of(" \t\r\n");
    return line.substr(first, last - first + 1);
}

int ConsoleUI::readInt(const std::string& prompt, int minimum, int maximum) {
    while (!inputClosed_) {
        const std::string raw = readLine(prompt);
        if (inputClosed_) return -1;
        std::istringstream input(raw);
        int value = 0;
        char extra = '\0';
        if ((input >> value) && !(input >> extra) && value >= minimum && value <= maximum) {
            return value;
        }
        std::cout << color("Please enter a whole number from " + std::to_string(minimum) +
                           " to " + std::to_string(maximum) + ".\n", "\033[91m");
    }
    return -1;
}

double ConsoleUI::readDouble(const std::string& prompt, double minimum) {
    while (!inputClosed_) {
        const std::string raw = readLine(prompt);
        if (inputClosed_) return -1.0;
        try {
            std::size_t used = 0;
            const double value = std::stod(raw, &used);
            if (used == raw.size() && std::isfinite(value) && value >= minimum) return value;
        } catch (const std::exception&) {
            // The validation message below handles malformed or out-of-range values.
        }
        std::cout << color("Please enter a valid number greater than or equal to " +
                           std::to_string(minimum) + ".\n", "\033[91m");
    }
    return -1.0;
}

std::string ConsoleUI::readRequiredText(const std::string& prompt, std::size_t maxLength) {
    while (!inputClosed_) {
        std::string value = readLine(prompt);
        if (inputClosed_) return {};
        if (!value.empty() && value.size() <= maxLength) return value;
        std::cout << color("Please enter between 1 and " + std::to_string(maxLength) +
                           " characters.\n", "\033[91m");
    }
    return {};
}

void ConsoleUI::pause() {
    if (inputClosed_) return;
    std::cout << color("\nPress ENTER to continue...", "\033[90m");
    std::string ignored;
    if (!std::getline(std::cin, ignored)) inputClosed_ = true;
}

void ConsoleUI::clearScreen() {
    if (colorsEnabled_) std::cout << "\033[2J\033[H";
}

void ConsoleUI::showBanner() {
    std::cout << color(
        "  _   _ _____  __  __  _   _  ____   __  __   _    ____  _____\n"
        " | \\ | | ____||  \\/  || | | |/ ___| |  \\/  | / \\  |  _ \\| ____|\n"
        " |  \\| |  _|  | |\\/| || | | | |  _  | |\\/| |/ _ \\ | |_) |  _|\n"
        " | |\\  | |___ | |  | || |_| | |_| | | |  | / ___ \\|  _ <| |___\n"
        " |_| \\_|_____||_|  |_| \\___/ \\____| |_|  |_/_/   \\_\\_| \\_\\_____|\n",
        "\033[96m");
    std::cout << color("                        YOUR TERMINAL MARKETPLACE\n", "\033[95m");
    std::cout << color("============================================================================\n", "\033[36m");
}

void ConsoleUI::printHeader(const std::string& title, const std::string& subtitle) {
    std::cout << color("+----------------------------------------------------------------------------+\n", "\033[36m");
    std::cout << color("|  " + title + "\n", "\033[96m");
    if (!subtitle.empty()) std::cout << color("|  " + subtitle + "\n", "\033[90m");
    std::cout << color("+----------------------------------------------------------------------------+\n", "\033[36m");
}

void ConsoleUI::showResult(bool success, const std::string& message) {
    const bool warning = message.find("WARNING:") != std::string::npos;
    const std::string selectedColor = warning ? "\033[93m" : (success ? "\033[92m" : "\033[91m");
    std::cout << color((success ? "[OK] " : "[!] ") + message + "\n", selectedColor);
}

std::string ConsoleUI::truncate(const std::string& text, std::size_t width) {
    if (text.size() <= width) return text;
    if (width <= 3) return text.substr(0, width);
    return text.substr(0, width - 3) + "...";
}

std::string ConsoleUI::money(double amount) {
    std::ostringstream out;
    out << "INR " << std::fixed << std::setprecision(2) << amount;
    return out.str();
}

void ConsoleUI::printProducts(const std::vector<Product>& products) {
    std::cout << "+------+---------------------------+--------------------+----------------+----------+\n";
    std::cout << "| ID   | PRODUCT                   | CATEGORY           | PRICE          | STOCK    |\n";
    std::cout << "+------+---------------------------+--------------------+----------------+----------+\n";
    if (products.empty()) {
        std::cout << "| No products matched your search.                                                |\n";
    } else {
        printCollection(products, [](const Product& product) {
            std::ostringstream row;
            row << "| " << std::left << std::setw(4) << product.getId() << " | "
                << std::setw(25) << truncate(product.getName(), 25) << " | "
                << std::setw(18) << truncate(product.getCategory(), 18) << " | "
                << std::right << std::setw(14) << std::fixed << std::setprecision(2)
                << product.getPrice() << " | "
                << std::setw(8) << product.getStock() << " |\n";
            std::cout << row.str() << std::left;
        });
    }
    std::cout << "+------+---------------------------+--------------------+----------------+----------+\n";
    std::cout << "Products shown: " << products.size() << "\n";
}

void ConsoleUI::browseProducts(Store& store, Customer& customer,
                               const std::vector<Product>& products,
                               const std::string& title) {
    while (!inputClosed_) {
        clearScreen();
        printHeader(title, "Select a product ID to see details; enter 0 to go back.");
        printProducts(products);
        const int productId = readInt("Product ID (0 = back): ", 0, 1000000000);
        if (inputClosed_ || productId == 0) return;

        const auto selected = std::find_if(products.begin(), products.end(), [productId](const Product& p) {
            return p.getId() == productId;
        });
        if (selected == products.end()) {
            showResult(false, "That ID is not in the displayed list.");
            pause();
            continue;
        }

        const Product* current = store.findProductById(productId);
        if (!current) {
            showResult(false, "This product is no longer available.");
            pause();
            continue;
        }

        clearScreen();
        printHeader("PRODUCT DETAILS");
        std::cout << "ID          : " << current->getId() << '\n'
                  << "Name        : " << current->getName() << '\n'
                  << "Category    : " << current->getCategory() << '\n'
                  << "Price       : " << money(current->getPrice()) << '\n'
                  << "Stock       : " << current->getStock() << '\n';
        if (current->getStock() == 0) {
            showResult(false, "This product is currently out of stock.");
            pause();
            continue;
        }
        const int add = readInt("Add this product to your cart? 1 = Yes, 0 = No: ", 0, 1);
        if (inputClosed_) return;
        if (add == 1) {
            const int quantity = readInt("Quantity (1-" + std::to_string(current->getStock()) + "): ",
                                         1, current->getStock());
            if (inputClosed_) return;
            std::string message;
            const bool success = store.addToCart(customer, productId, quantity, message);
            showResult(success, message);
            pause();
        }
    }
}

void ConsoleUI::searchProducts(Store& store, Customer& customer) {
    clearScreen();
    printHeader("SEARCH PRODUCTS", "Search is case-insensitive.");
    const std::string query = readRequiredText("Keyword (name or category): ", 80);
    if (inputClosed_) return;
    const int filtered = readInt("Apply a category filter? 1 = Yes, 0 = No: ", 0, 1);
    if (inputClosed_) return;

    std::vector<Product> results;
    if (filtered == 1) {
        const std::string category = readRequiredText("Category contains: ", 80);
        if (inputClosed_) return;
        results = store.searchProducts(query, category);
    } else {
        results = store.searchProducts(query);
    }
    browseProducts(store, customer, results, "SEARCH RESULTS");
}

void ConsoleUI::showCart(Store& store, Customer& customer) {
    ShoppingCart& cart = customer.getCart();
    while (!inputClosed_) {
        clearScreen();
        printHeader("YOUR SHOPPING CART", "Prices are snapshots from when an item was added.");
        std::cout << "+------+---------------------------+----------+----------+----------------+\n";
        std::cout << "| ID   | PRODUCT                   | QTY      | UNIT     | LINE TOTAL     |\n";
        std::cout << "+------+---------------------------+----------+----------+----------------+\n";
        printCollection(cart.getItems(), [](const CartItem& item) {
            std::cout << "| " << std::left << std::setw(4) << item.getProductId() << " | "
                      << std::setw(25) << truncate(item.getProductName(), 25) << " | "
                      << std::setw(8) << item.getQuantity() << " | "
                      << std::right << std::setw(8) << std::fixed << std::setprecision(2)
                      << item.getUnitPrice() << " | " << std::setw(14) << item.getLineTotal() << " |\n"
                      << std::left;
        });
        std::cout << "+------+---------------------------+----------+----------+----------------+\n";
        std::cout << "Subtotal      : " << money(cart.subtotal()) << '\n';
        std::cout << "Discount      : -" << money(cart.discountAmount());
        if (!cart.getCouponCode().empty()) std::cout << "  (" << cart.getCouponCode() << ")";
        std::cout << '\n' << color("FINAL TOTAL   : " + money(cart.total()) + "\n", "\033[92m");

        if (cart.empty()) {
            std::cout << "\nYour cart is empty. Add some products from the catalog.\n";
            pause();
            return;
        }

        std::cout << "\n1. Update item quantity\n"
                  << "2. Remove an item\n"
                  << "3. Apply coupon (SAVE10 / SAVE5)\n"
                  << "4. Remove coupon\n"
                  << "5. Checkout\n"
                  << "0. Back\n";
        const int choice = readInt("Choose an option: ", 0, 5);
        if (inputClosed_ || choice == 0) return;
        std::string message;
        bool success = false;

        if (choice == 1) {
            const int id = readInt("Product ID: ", 1, 1000000000);
            if (inputClosed_) return;
            const int quantity = readInt("New quantity: ", 1, 1000000);
            if (inputClosed_) return;
            success = store.updateCartQuantity(customer, id, quantity, message);
            showResult(success, message);
            pause();
        } else if (choice == 2) {
            const int id = readInt("Product ID to remove: ", 1, 1000000000);
            if (inputClosed_) return;
            success = store.removeCartItem(customer, id, message);
            showResult(success, message);
            pause();
        } else if (choice == 3) {
            const std::string code = readRequiredText("Coupon code: ", 40);
            if (inputClosed_) return;
            success = cart.applyCoupon(code, message);
            showResult(success, message);
            pause();
        } else if (choice == 4) {
            cart.clearCoupon();
            showResult(true, "Coupon removed.");
            pause();
        } else if (choice == 5) {
            std::cout << "\nPAYMENT METHODS\n1. Cash on Delivery\n2. Demo UPI\n0. Cancel\n";
            const int method = readInt("Choose payment method: ", 0, 2);
            if (inputClosed_ || method == 0) continue;
            if (method == 1) {
                CashOnDelivery payment;
                success = store.checkout(customer, payment, message);
            } else {
                const std::string upiId = readRequiredText("Demo UPI ID (example: demo@upi): ", 80);
                if (inputClosed_) return;
                UPIPayment payment(upiId);
                success = store.checkout(customer, payment, message);
            }
            showResult(success, message);
            pause();
            if (success && cart.empty()) return;
        }
    }
}

void ConsoleUI::showCustomerOrders(const Store& store, const Customer& customer) {
    clearScreen();
    printHeader("MY ORDERS", "Previous purchases keep their original product prices.");
    const auto orders = store.getOrdersForCustomer(customer.getId());
    if (orders.empty()) {
        std::cout << "You have not placed an order yet.\n";
        pause();
        return;
    }

    for (const Order* order : orders) {
        std::cout << "\n" << color("ORDER #" + std::to_string(order->getId()), "\033[96m")
                  << "  |  " << order->getDate() << "  |  "
                  << orderStatusToString(order->getStatus()) << '\n';
        std::cout << "Payment: " << order->getPaymentMethod() << '\n';
        for (const auto& item : order->getItems()) {
            std::cout << "  - " << item.getProductName() << " x " << item.getQuantity()
                      << " @ " << money(item.getUnitPrice()) << " = "
                      << money(item.getLineTotal()) << '\n';
        }
        std::cout << "  Subtotal: " << money(order->getSubtotal())
                  << " | Discount: -" << money(order->getDiscountAmount())
                  << " | Total: " << color(money(order->getTotal()), "\033[92m") << '\n';
    }
    pause();
}

void ConsoleUI::customerMenu(Store& store, Customer& customer) {
    while (!inputClosed_) {
        clearScreen();
        showBanner();
        printHeader("CUSTOMER DASHBOARD", "Signed in as " + customer.getUsername());
        std::cout << "  [1] Browse products\n"
                  << "  [2] Search products\n"
                  << "  [3] Shopping cart (" << customer.getCart().itemCount() << " product types)\n"
                  << "  [4] My orders\n"
                  << "  [5] Account details\n"
                  << "  [0] Logout\n\n";
        const int choice = readInt("Enter your choice: ", 0, 5);
        if (inputClosed_ || choice == 0) return;
        switch (choice) {
            case 1: browseProducts(store, customer, store.getProducts(), "PRODUCT CATALOG"); break;
            case 2: searchProducts(store, customer); break;
            case 3: showCart(store, customer); break;
            case 4: showCustomerOrders(store, customer); break;
            case 5:
                clearScreen();
                printHeader("ACCOUNT DETAILS");
                std::cout << "Username : " << customer.getUsername() << '\n'
                          << "User ID  : " << customer.getId() << '\n'
                          << "Role     : " << customer.getRole() << '\n';
                std::cout << "Note: the terminal demo does not mask password input.\n";
                pause();
                break;
            default: break;
        }
    }
}

void ConsoleUI::addProductFlow(Store& store) {
    clearScreen();
    printHeader("ADD PRODUCT", "New products receive an automatically assigned ID.");
    const std::string name = readRequiredText("Product name: ", 80);
    if (inputClosed_) return;
    const std::string category = readRequiredText("Category: ", 60);
    if (inputClosed_) return;
    const double price = readDouble("Price (INR): ", 0.0);
    if (inputClosed_) return;
    const int stock = readInt("Initial stock: ", 0, 1000000);
    if (inputClosed_) return;
    std::string message;
    const bool success = store.addProduct(name, category, price, stock, message);
    showResult(success, message);
    pause();
}

void ConsoleUI::updateProductFlow(Store& store) {
    clearScreen();
    printHeader("UPDATE INVENTORY");
    printProducts(store.getProducts());
    const int id = readInt("Product ID: ", 1, 1000000000);
    if (inputClosed_) return;
    if (!store.findProductById(id)) {
        showResult(false, "Product ID was not found.");
        pause();
        return;
    }
    std::cout << "1. Update price\n2. Set stock quantity\n0. Cancel\n";
    const int choice = readInt("Choose an option: ", 0, 2);
    if (inputClosed_ || choice == 0) return;
    std::string message;
    bool success = false;
    if (choice == 1) {
        const double price = readDouble("New price (INR): ", 0.0);
        if (inputClosed_) return;
        success = store.updateProductPrice(id, price, message);
    } else {
        const int stock = readInt("New stock quantity: ", 0, 1000000);
        if (inputClosed_) return;
        success = store.updateProductStock(id, stock, message);
    }
    showResult(success, message);
    pause();
}

void ConsoleUI::removeProductFlow(Store& store) {
    clearScreen();
    printHeader("REMOVE PRODUCT", "Completed order history is retained.");
    printProducts(store.getProducts());
    const int id = readInt("Product ID to remove (0 = cancel): ", 0, 1000000000);
    if (inputClosed_ || id == 0) return;
    std::cout << "Remove product #" << id << "?\n1. Confirm\n0. Cancel\n";
    const int confirm = readInt("Choice: ", 0, 1);
    if (inputClosed_ || confirm == 0) return;
    std::string message;
    const bool success = store.removeProduct(id, message);
    showResult(success, message);
    pause();
}

void ConsoleUI::showAllOrders(const Store& store) {
    clearScreen();
    printHeader("ORDER MANAGEMENT", "Use Advance Status to move an order one step forward.");
    const auto& orders = store.getOrders();
    if (orders.empty()) {
        std::cout << "No orders have been placed yet.\n";
        pause();
        return;
    }
    std::cout << "+--------+------------------+------------------+--------------+----------------+\n";
    std::cout << "| ORDER  | CUSTOMER         | DATE             | STATUS       | TOTAL          |\n";
    std::cout << "+--------+------------------+------------------+--------------+----------------+\n";
    printCollection(orders, [&store](const Order& order) {
        std::cout << "| " << std::left << std::setw(6) << order.getId() << " | "
                  << std::setw(16) << truncate(store.usernameForId(order.getCustomerId()), 16) << " | "
                  << std::setw(16) << truncate(order.getDate(), 16) << " | "
                  << std::setw(12) << orderStatusToString(order.getStatus()) << " | "
                  << std::right << std::setw(14) << std::fixed << std::setprecision(2)
                  << order.getTotal() << " |\n" << std::left;
    });
    std::cout << "+--------+------------------+------------------+--------------+----------------+\n";
    pause();
}

void ConsoleUI::updateOrderFlow(Store& store) {
    clearScreen();
    printHeader("ADVANCE ORDER STATUS", "Allowed sequence: PLACED -> PROCESSING -> SHIPPED -> DELIVERED");
    if (store.getOrders().empty()) {
        std::cout << "No orders are available.\n";
        pause();
        return;
    }
    for (const auto& order : store.getOrders()) {
        std::cout << "Order #" << order.getId() << " | "
                  << store.usernameForId(order.getCustomerId()) << " | "
                  << orderStatusToString(order.getStatus()) << " | " << money(order.getTotal()) << '\n';
    }
    const int id = readInt("Order ID to advance (0 = cancel): ", 0, 1000000000);
    if (inputClosed_ || id == 0) return;
    std::string message;
    const bool success = store.advanceOrderStatus(id, message);
    showResult(success, message);
    pause();
}

void ConsoleUI::showStatistics(const Store& store) {
    clearScreen();
    printHeader("STORE STATISTICS", "Simple operational snapshot.");
    const StoreStatistics stats = store.getStatistics();
    std::cout << "Distinct products     : " << stats.productCount << '\n'
              << "Units in stock       : " << stats.totalStockUnits << '\n'
              << "Orders placed        : " << stats.orderCount << '\n'
              << "Total order value    : " << money(stats.totalOrderValue) << '\n';
    std::cout << "\nTotal order value includes all orders, regardless of delivery status.\n";
    pause();
}

void ConsoleUI::adminMenu(Store& store, Admin& admin) {
    while (!inputClosed_) {
        clearScreen();
        showBanner();
        printHeader("ADMIN DASHBOARD", "Signed in as " + admin.getUsername());
        std::cout << "  [1] View inventory\n"
                  << "  [2] Add product\n"
                  << "  [3] Update product price/stock\n"
                  << "  [4] Remove product\n"
                  << "  [5] Search inventory\n"
                  << "  [6] View all orders\n"
                  << "  [7] Advance order status\n"
                  << "  [8] Store statistics\n"
                  << "  [0] Logout\n\n";
        const int choice = readInt("Enter your choice: ", 0, 8);
        if (inputClosed_ || choice == 0) return;
        switch (choice) {
            case 1:
                clearScreen(); printHeader("INVENTORY"); printProducts(store.getProducts()); pause(); break;
            case 2: addProductFlow(store); break;
            case 3: updateProductFlow(store); break;
            case 4: removeProductFlow(store); break;
            case 5: {
                clearScreen();
                printHeader("SEARCH INVENTORY");
                const std::string query = readRequiredText("Keyword (name or category): ", 80);
                if (inputClosed_) return;
                printProducts(store.searchProducts(query));
                pause();
                break;
            }
            case 6: showAllOrders(store); break;
            case 7: updateOrderFlow(store); break;
            case 8: showStatistics(store); break;
            default: break;
        }
    }
}

void ConsoleUI::run(Store& store) {
    enableAnsiColors();
    inputClosed_ = false;
    if (!store.getStartupWarning().empty()) {
        showResult(false, "WARNING: " + store.getStartupWarning());
        pause();
    }

    while (!inputClosed_) {
        clearScreen();
        showBanner();
        printHeader("WELCOME", "A C++17 terminal marketplace");
        std::cout << "  [1] Login\n"
                  << "  [2] Create customer account\n"
                  << "  [0] Exit\n\n"
                  << color("  Demo admin login: admin / admin123\n", "\033[93m")
                  << "\n";
        const int choice = readInt("Enter your choice: ", 0, 2);
        if (inputClosed_ || choice == 0) break;

        if (choice == 1) {
            clearScreen();
            printHeader("LOGIN");
            const std::string username = readRequiredText("Username: ", 80);
            if (inputClosed_) break;
            const std::string password = readRequiredText("Password: ", 80);
            if (inputClosed_) break;
            User* user = store.authenticate(username, password);
            if (!user) {
                showResult(false, "Incorrect username or password.");
                pause();
                continue;
            }
            showResult(true, "Welcome, " + user->getUsername() + ".");
            if (auto* admin = dynamic_cast<Admin*>(user)) adminMenu(store, *admin);
            else if (auto* customer = dynamic_cast<Customer*>(user)) customerMenu(store, *customer);
            else {
                showResult(false, "Unsupported account role.");
                pause();
            }
        } else if (choice == 2) {
            clearScreen();
            printHeader("CREATE ACCOUNT", "Customer usernames are case-insensitive for uniqueness.");
            const std::string username = readRequiredText("Choose a username (3-24 chars): ", 80);
            if (inputClosed_) break;
            const std::string password = readRequiredText("Choose a password (4-64 chars): ", 80);
            if (inputClosed_) break;
            std::string message;
            const bool success = store.registerCustomer(username, password, message);
            showResult(success, message);
            pause();
        }
    }
    if (!inputClosed_) {
        clearScreen();
        std::cout << color("Thanks for using Nexus Mart. Goodbye!\n", "\033[96m");
    }
}
