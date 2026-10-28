#ifndef NEXUSMART_FILEMANAGER_H
#define NEXUSMART_FILEMANAGER_H

#include "Order.h"
#include "Product.h"
#include "User.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class FileManager {
private:
    std::filesystem::path directory_;

    bool writeProducts(const std::vector<Product>& products, std::string& error) const;
    bool writeUsers(const std::vector<std::unique_ptr<User>>& users, std::string& error) const;
    bool writeOrders(const std::vector<Order>& orders, std::string& error) const;
    bool writeAtomically(const std::filesystem::path& target,
                         const std::string& contents,
                         std::string& error) const;

public:
    explicit FileManager(std::filesystem::path directory = "data");

    std::vector<Product> loadProducts() const;
    std::vector<std::unique_ptr<User>> loadUsers() const;
    std::vector<Order> loadOrders() const;

    bool saveAll(const std::vector<Product>& products,
                 const std::vector<std::unique_ptr<User>>& users,
                 const std::vector<Order>& orders,
                 std::string& error) const;

    const std::filesystem::path& getDirectory() const noexcept;
};

#endif
