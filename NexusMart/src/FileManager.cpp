#include "FileManager.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <system_error>

FileManager::FileManager(std::filesystem::path directory) : directory_(std::move(directory)) {
    std::error_code ec;
    std::filesystem::create_directories(directory_, ec);
}

std::vector<Product> FileManager::loadProducts() const {
    std::vector<Product> products;
    std::ifstream in(directory_ / "products.txt");
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream row(line);
        int id = 0;
        std::string name, category;
        double price = 0.0;
        int stock = 0;
        if (!(row >> id >> std::quoted(name) >> std::quoted(category) >> price >> stock)) continue;
        try {
            products.emplace_back(id, name, category, price, stock);
        } catch (const std::exception&) {
            // Ignore malformed records; the rest of the inventory can still be loaded.
        }
    }
    return products;
}

std::vector<std::unique_ptr<User>> FileManager::loadUsers() const {
    std::vector<std::unique_ptr<User>> users;
    std::ifstream in(directory_ / "users.txt");
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream row(line);
        std::string role, username, password;
        int id = 0;
        if (!(row >> role >> id >> std::quoted(username) >> std::quoted(password))) continue;
        try {
            if (role == "ADMIN") {
                users.push_back(std::make_unique<Admin>(id, username, password));
            } else if (role == "CUSTOMER") {
                users.push_back(std::make_unique<Customer>(id, username, password));
            }
        } catch (const std::exception&) {
            // Skip invalid user records rather than preventing startup.
        }
    }
    return users;
}

std::vector<Order> FileManager::loadOrders() const {
    std::vector<Order> orders;
    std::ifstream in(directory_ / "orders.txt");
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream row(line);
        int id = 0, customerId = 0;
        std::string date, paymentMethod, statusText;
        double subtotal = 0.0, discount = 0.0;
        std::size_t itemCount = 0;

        if (!(row >> id >> customerId >> std::quoted(date) >> std::quoted(paymentMethod)
                  >> std::quoted(statusText) >> subtotal >> discount >> itemCount)) {
            continue;
        }
        if (itemCount == 0 || itemCount > 1000) continue;

        OrderStatus status;
        if (!orderStatusFromString(statusText, status)) continue;

        std::vector<OrderItem> items;
        bool valid = true;
        for (std::size_t i = 0; i < itemCount; ++i) {
            int productId = 0, quantity = 0;
            std::string name, category;
            double unitPrice = 0.0;
            if (!(row >> productId >> std::quoted(name) >> std::quoted(category)
                      >> unitPrice >> quantity)) {
                valid = false;
                break;
            }
            try {
                items.emplace_back(productId, name, category, unitPrice, quantity);
            } catch (const std::exception&) {
                valid = false;
                break;
            }
        }
        if (!valid) continue;
        try {
            orders.emplace_back(id, customerId, date, paymentMethod, status,
                                std::move(items), subtotal, discount);
        } catch (const std::exception&) {
            // Skip the invalid order while keeping other valid orders available.
        }
    }
    return orders;
}

bool FileManager::writeAtomically(const std::filesystem::path& target,
                                  const std::string& contents,
                                  std::string& error) const {
    std::error_code ec;
    std::filesystem::create_directories(target.parent_path(), ec);
    if (ec) {
        error = "Cannot create data directory: " + ec.message();
        return false;
    }

    auto temporary = target;
    temporary += ".tmp";
    auto backup = target;
    backup += ".bak";

    {
        std::ofstream out(temporary, std::ios::out | std::ios::trunc);
        if (!out) {
            error = "Cannot open temporary file for writing: " + temporary.string();
            return false;
        }
        out << contents;
        out.flush();
        if (!out) {
            error = "Failed while writing data to: " + temporary.string();
            out.close();
            std::filesystem::remove(temporary, ec);
            return false;
        }
    }

    const bool targetExists = std::filesystem::exists(target, ec);
    ec.clear();
    if (targetExists) {
        std::filesystem::remove(backup, ec);
        ec.clear();
        std::filesystem::rename(target, backup, ec);
        if (ec) {
            error = "Cannot create a backup of " + target.string() + ": " + ec.message();
            std::filesystem::remove(temporary, ec);
            return false;
        }
    }

    ec.clear();
    std::filesystem::rename(temporary, target, ec);
    if (ec) {
        const std::string renameError = ec.message();
        if (targetExists) {
            std::error_code restoreError;
            std::filesystem::rename(backup, target, restoreError);
        }
        std::filesystem::remove(temporary, ec);
        error = "Cannot replace " + target.string() + ": " + renameError;
        return false;
    }

    if (targetExists) {
        std::filesystem::remove(backup, ec);
    }
    return true;
}

bool FileManager::writeProducts(const std::vector<Product>& products, std::string& error) const {
    std::ostringstream out;
    out << "# id \"name\" \"category\" price stock\n";
    for (const auto& product : products) {
        out << product.getId() << ' ' << std::quoted(product.getName()) << ' '
            << std::quoted(product.getCategory()) << ' '
            << std::setprecision(17) << product.getPrice() << ' '
            << product.getStock() << '\n';
    }
    return writeAtomically(directory_ / "products.txt", out.str(), error);
}

bool FileManager::writeUsers(const std::vector<std::unique_ptr<User>>& users, std::string& error) const {
    std::ostringstream out;
    out << "# role id \"username\" \"password\" (demo project only)\n";
    for (const auto& user : users) {
        if (!user) continue;
        // The small learning project deliberately uses plaintext credentials; see README.
        // Keep this loop inside User's interface by writing the password through a dedicated
        // persistence accessor rather than exposing it to normal application code.
        out << user->getRole() << ' ' << user->getId() << ' '
            << std::quoted(user->getUsername()) << ' '
            << std::quoted(user->password_) << '\n';
    }
    return writeAtomically(directory_ / "users.txt", out.str(), error);
}

bool FileManager::writeOrders(const std::vector<Order>& orders, std::string& error) const {
    std::ostringstream out;
    out << "# id customerId \"date\" \"payment\" \"status\" subtotal discount itemCount [items...]\n";
    for (const auto& order : orders) {
        const auto& items = order.getItems();
        out << order.getId() << ' ' << order.getCustomerId() << ' '
            << std::quoted(order.getDate()) << ' '
            << std::quoted(order.getPaymentMethod()) << ' '
            << std::quoted(orderStatusToString(order.getStatus())) << ' '
            << std::setprecision(17) << order.getSubtotal() << ' '
            << order.getDiscountAmount() << ' ' << items.size();
        for (const auto& item : items) {
            out << ' ' << item.getProductId() << ' '
                << std::quoted(item.getProductName()) << ' '
                << std::quoted(item.getCategory()) << ' '
                << std::setprecision(17) << item.getUnitPrice() << ' '
                << item.getQuantity();
        }
        out << '\n';
    }
    return writeAtomically(directory_ / "orders.txt", out.str(), error);
}

bool FileManager::saveAll(const std::vector<Product>& products,
                          const std::vector<std::unique_ptr<User>>& users,
                          const std::vector<Order>& orders,
                          std::string& error) const {
    if (!writeProducts(products, error)) return false;
    if (!writeUsers(users, error)) return false;
    if (!writeOrders(orders, error)) return false;
    return true;
}

const std::filesystem::path& FileManager::getDirectory() const noexcept { return directory_; }
