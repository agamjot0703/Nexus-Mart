#include "Store.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string currentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    const std::tm* local = std::localtime(&time);
    if (!local) return "Unknown date";
    std::ostringstream out;
    out << std::put_time(local, "%Y-%m-%d %H:%M");
    return out.str();
}
} // namespace

Store::Store(std::filesystem::path dataDirectory)
    : fileManager_(std::move(dataDirectory)) {
    initialize();
}

void Store::initialize() {
    products_ = fileManager_.loadProducts();
    users_ = fileManager_.loadUsers();
    orders_ = fileManager_.loadOrders();

    bool dataWasSeeded = false;
    if (products_.empty()) {
        products_.emplace_back(101, "Mechanical Keyboard", "Electronics", 2499.00, 12);
        products_.emplace_back(102, "Gaming Mouse", "Electronics", 1299.00, 20);
        products_.emplace_back(103, "USB-C Hub", "Accessories", 899.00, 8);
        products_.emplace_back(104, "Laptop Stand", "Accessories", 1499.00, 6);
        products_.emplace_back(105, "Wireless Headphones", "Audio", 3299.00, 10);
        products_.emplace_back(106, "Webcam 1080p", "Electronics", 2199.00, 7);
        products_.emplace_back(107, "Desk Mat", "Accessories", 499.00, 25);
        products_.emplace_back(108, "Portable SSD 1TB", "Storage", 6499.00, 5);
        dataWasSeeded = true;
    }

    const bool hasAdmin = std::any_of(users_.begin(), users_.end(), [](const auto& user) {
        return dynamic_cast<const Admin*>(user.get()) != nullptr;
    });
    if (!hasAdmin) {
        users_.push_back(std::make_unique<Admin>("admin", "admin123"));
        dataWasSeeded = true;
    }

    if (dataWasSeeded) {
        std::string error;
        if (!fileManager_.saveAll(products_, users_, orders_, error)) {
            startupWarning_ = "Initial data was created in memory, but could not be saved: " + error;
        }
    }
}

const std::string& Store::getStartupWarning() const noexcept { return startupWarning_; }
const std::vector<Product>& Store::getProducts() const noexcept { return products_; }
const std::vector<Order>& Store::getOrders() const noexcept { return orders_; }

Product* Store::findProductById(int productId) noexcept {
    const auto it = std::find_if(products_.begin(), products_.end(), [productId](const Product& product) {
        return product.getId() == productId;
    });
    return it == products_.end() ? nullptr : &(*it);
}

const Product* Store::findProductById(int productId) const noexcept {
    const auto it = std::find_if(products_.begin(), products_.end(), [productId](const Product& product) {
        return product.getId() == productId;
    });
    return it == products_.end() ? nullptr : &(*it);
}

User* Store::authenticate(const std::string& username, const std::string& password) noexcept {
    const auto it = std::find_if(users_.begin(), users_.end(), [&](const auto& user) {
        return user && user->getUsername() == username && user->verifyPassword(password);
    });
    return it == users_.end() ? nullptr : it->get();
}

bool Store::registerCustomer(const std::string& username,
                             const std::string& password,
                             std::string& message) {
    if (username.size() < 3 || username.size() > 24) {
        message = "Username must contain between 3 and 24 characters.";
        return false;
    }
    if (password.size() < 4 || password.size() > 64) {
        message = "Password must contain between 4 and 64 characters.";
        return false;
    }
    if (username.find_first_of(" \t\r\n") != std::string::npos) {
        message = "Username cannot contain whitespace.";
        return false;
    }
    const std::string normalized = lowercase(username);
    const bool usernameTaken = std::any_of(users_.begin(), users_.end(), [&](const auto& user) {
        return user && lowercase(user->getUsername()) == normalized;
    });
    if (usernameTaken) {
        message = "That username is already taken.";
        return false;
    }

    try {
        users_.push_back(std::make_unique<Customer>(username, password));
    } catch (const std::exception& ex) {
        message = ex.what();
        return false;
    }
    message = "Account created. You can now log in as " + username + ".";
    persistAfterSuccess(message);
    return true;
}

std::vector<Product> Store::searchProducts(const std::string& query) const {
    std::vector<Product> results;
    const std::string term = lowercase(query);
    for (const auto& product : products_) {
        const std::string name = lowercase(product.getName());
        const std::string category = lowercase(product.getCategory());
        if (term.empty() || name.find(term) != std::string::npos || category.find(term) != std::string::npos) {
            results.push_back(product);
        }
    }
    return results;
}

std::vector<Product> Store::searchProducts(const std::string& query,
                                           const std::string& category) const {
    std::vector<Product> results;
    const std::string term = lowercase(query);
    const std::string categoryFilter = lowercase(category);
    for (const auto& product : products_) {
        const std::string name = lowercase(product.getName());
        const std::string productCategory = lowercase(product.getCategory());
        const bool matchesTerm = term.empty() || name.find(term) != std::string::npos ||
                                 productCategory.find(term) != std::string::npos;
        const bool matchesCategory = categoryFilter.empty() ||
                                     productCategory.find(categoryFilter) != std::string::npos;
        if (matchesTerm && matchesCategory) results.push_back(product);
    }
    return results;
}

void Store::persistAfterSuccess(std::string& message) const {
    std::string error;
    if (!fileManager_.saveAll(products_, users_, orders_, error)) {
        message += "\nWARNING: the action succeeded in memory, but saving failed: " + error;
    }
}

bool Store::addProduct(const std::string& name, const std::string& category,
                       double price, int stock, std::string& message) {
    try {
        products_.emplace_back(name, category, price, stock);
    } catch (const std::exception& ex) {
        message = ex.what();
        return false;
    }
    message = "Product added with ID " + std::to_string(products_.back().getId()) + ".";
    persistAfterSuccess(message);
    return true;
}

bool Store::updateProductPrice(int productId, double price, std::string& message) {
    Product* product = findProductById(productId);
    if (!product) {
        message = "Product ID was not found.";
        return false;
    }
    try {
        product->setPrice(price);
    } catch (const std::exception& ex) {
        message = ex.what();
        return false;
    }
    message = "Price updated for product #" + std::to_string(productId) + ".";
    persistAfterSuccess(message);
    return true;
}

bool Store::updateProductStock(int productId, int stock, std::string& message) {
    Product* product = findProductById(productId);
    if (!product) {
        message = "Product ID was not found.";
        return false;
    }
    try {
        product->setStock(stock);
    } catch (const std::exception& ex) {
        message = ex.what();
        return false;
    }
    message = "Stock updated for product #" + std::to_string(productId) + ".";
    persistAfterSuccess(message);
    return true;
}

bool Store::removeProduct(int productId, std::string& message) {
    const auto it = std::find_if(products_.begin(), products_.end(), [productId](const Product& product) {
        return product.getId() == productId;
    });
    if (it == products_.end()) {
        message = "Product ID was not found.";
        return false;
    }
    const std::string removedName = it->getName();
    products_.erase(it);
    message = "Removed '" + removedName + "'. Existing order history remains intact.";
    persistAfterSuccess(message);
    return true;
}

bool Store::addToCart(Customer& customer, int productId, int quantity, std::string& message) {
    if (quantity <= 0) {
        message = "Quantity must be at least 1.";
        return false;
    }
    const Product* product = findProductById(productId);
    if (!product) {
        message = "Product ID was not found.";
        return false;
    }
    const int requestedTotal = customer.getCart().quantityFor(productId) + quantity;
    if (!product->isAvailable(requestedTotal)) {
        message = "Not enough stock. Available quantity: " + std::to_string(product->getStock()) + ".";
        return false;
    }
    try {
        customer.getCart().addProduct(*product, quantity);
    } catch (const std::exception& ex) {
        message = ex.what();
        return false;
    }
    message = "Added " + std::to_string(quantity) + " x " + product->getName() + " to the cart.";
    return true;
}

bool Store::updateCartQuantity(Customer& customer, int productId, int quantity, std::string& message) {
    if (quantity <= 0) {
        message = "Quantity must be at least 1. Use Remove Item to delete a product.";
        return false;
    }
    const Product* product = findProductById(productId);
    if (!product) {
        message = "That product is no longer in the inventory.";
        return false;
    }
    if (!product->isAvailable(quantity)) {
        message = "Not enough stock. Available quantity: " + std::to_string(product->getStock()) + ".";
        return false;
    }
    if (!customer.getCart().setQuantity(productId, quantity)) {
        message = "That product is not in your cart.";
        return false;
    }
    message = "Cart quantity updated.";
    return true;
}

bool Store::removeCartItem(Customer& customer, int productId, std::string& message) {
    if (!customer.getCart().removeProduct(productId)) {
        message = "That product is not in your cart.";
        return false;
    }
    message = "Item removed from the cart.";
    return true;
}

bool Store::checkout(Customer& customer, PaymentMethod& paymentMethod, std::string& message) {
    ShoppingCart& cart = customer.getCart();
    if (cart.empty()) {
        message = "Your cart is empty.";
        return false;
    }

    for (const auto& item : cart.getItems()) {
        const Product* product = findProductById(item.getProductId());
        if (!product) {
            message = "Checkout stopped: '" + item.getProductName() + "' is no longer available.";
            return false;
        }
        if (!product->isAvailable(item.getQuantity())) {
            message = "Checkout stopped: insufficient stock for '" + item.getProductName() + "'.";
            return false;
        }
    }

    std::string paymentMessage;
    if (!paymentMethod.pay(cart.total(), paymentMessage)) {
        message = "Payment was not accepted. " + paymentMessage + " Your cart is unchanged.";
        return false;
    }

    std::vector<OrderItem> orderItems;
    orderItems.reserve(cart.getItems().size());
    for (const auto& item : cart.getItems()) orderItems.emplace_back(item);

    try {
        Order order(customer.getId(), currentTimestamp(), paymentMethod.getName(),
                    std::move(orderItems), cart.subtotal(), cart.discountAmount());
        const int orderId = order.getId();
        const double orderTotal = order.getTotal();

        // All items have been validated above, so stock changes can now be committed.
        for (const auto& item : cart.getItems()) {
            Product* product = findProductById(item.getProductId());
            product->reduceStock(item.getQuantity());
        }
        orders_.push_back(std::move(order));
        cart.clear();

        std::ostringstream summary;
        summary << "Order #" << orderId << " placed successfully. Total: INR "
                << std::fixed << std::setprecision(2) << orderTotal << ".\n" << paymentMessage;
        message = summary.str();
        persistAfterSuccess(message);
        return true;
    } catch (const std::exception& ex) {
        message = std::string("Checkout failed safely: ") + ex.what();
        return false;
    }
}

std::vector<const Order*> Store::getOrdersForCustomer(int customerId) const {
    std::vector<const Order*> results;
    for (const auto& order : orders_) {
        if (order.getCustomerId() == customerId) results.push_back(&order);
    }
    return results;
}

bool Store::advanceOrderStatus(int orderId, std::string& message) {
    const auto it = std::find_if(orders_.begin(), orders_.end(), [orderId](const Order& order) {
        return order.getId() == orderId;
    });
    if (it == orders_.end()) {
        message = "Order ID was not found.";
        return false;
    }
    if (!it->advanceStatus(message)) return false;
    message = "Order #" + std::to_string(orderId) + ": " + message;
    persistAfterSuccess(message);
    return true;
}

std::string Store::usernameForId(int userId) const {
    const auto it = std::find_if(users_.begin(), users_.end(), [userId](const auto& user) {
        return user && user->getId() == userId;
    });
    return it == users_.end() ? "Unknown user" : (*it)->getUsername();
}

StoreStatistics Store::getStatistics() const noexcept {
    StoreStatistics stats;
    stats.productCount = products_.size();
    stats.orderCount = orders_.size();
    for (const auto& product : products_) stats.totalStockUnits += product.getStock();
    for (const auto& order : orders_) stats.totalOrderValue += order.getTotal();
    return stats;
}
