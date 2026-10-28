#ifndef NEXUSMART_STORE_H
#define NEXUSMART_STORE_H

#include "FileManager.h"
#include "Order.h"
#include "PaymentMethod.h"
#include "Product.h"
#include "User.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

struct StoreStatistics {
    std::size_t productCount = 0;
    int totalStockUnits = 0;
    std::size_t orderCount = 0;
    double totalOrderValue = 0.0;
};

class Store {
private:
    FileManager fileManager_;
    std::vector<Product> products_;
    std::vector<std::unique_ptr<User>> users_;
    std::vector<Order> orders_;
    std::string startupWarning_;

    void initialize();
    void persistAfterSuccess(std::string& message) const;

public:
    explicit Store(std::filesystem::path dataDirectory = "data");

    const std::string& getStartupWarning() const noexcept;
    const std::vector<Product>& getProducts() const noexcept;
    const std::vector<Order>& getOrders() const noexcept;

    Product* findProductById(int productId) noexcept;
    const Product* findProductById(int productId) const noexcept;
    User* authenticate(const std::string& username, const std::string& password) noexcept;
    bool registerCustomer(const std::string& username,
                          const std::string& password,
                          std::string& message);

    // Search overloads: keyword across name/category, or keyword plus a category filter.
    std::vector<Product> searchProducts(const std::string& query) const;
    std::vector<Product> searchProducts(const std::string& query,
                                        const std::string& category) const;

    bool addProduct(const std::string& name, const std::string& category,
                    double price, int stock, std::string& message);
    bool updateProductPrice(int productId, double price, std::string& message);
    bool updateProductStock(int productId, int stock, std::string& message);
    bool removeProduct(int productId, std::string& message);

    bool addToCart(Customer& customer, int productId, int quantity, std::string& message);
    bool updateCartQuantity(Customer& customer, int productId, int quantity, std::string& message);
    bool removeCartItem(Customer& customer, int productId, std::string& message);
    bool checkout(Customer& customer, PaymentMethod& paymentMethod, std::string& message);

    std::vector<const Order*> getOrdersForCustomer(int customerId) const;
    bool advanceOrderStatus(int orderId, std::string& message);
    std::string usernameForId(int userId) const;
    StoreStatistics getStatistics() const noexcept;
};

#endif
