#ifndef NEXUSMART_ORDER_H
#define NEXUSMART_ORDER_H

#include "ShoppingCart.h"

#include <string>
#include <vector>

enum class OrderStatus {
    Placed,
    Processing,
    Shipped,
    Delivered
};

std::string orderStatusToString(OrderStatus status);
bool orderStatusFromString(const std::string& text, OrderStatus& status);

class OrderItem {
private:
    int productId_;
    std::string productName_;
    std::string category_;
    double unitPrice_;
    int quantity_;

public:
    explicit OrderItem(const CartItem& item);
    OrderItem(int productId, std::string name, std::string category, double unitPrice, int quantity);

    int getProductId() const noexcept;
    const std::string& getProductName() const noexcept;
    const std::string& getCategory() const noexcept;
    double getUnitPrice() const noexcept;
    int getQuantity() const noexcept;
    double getLineTotal() const noexcept;
};

class Order {
private:
    static int nextId_;

    int id_;
    int customerId_;
    std::string date_;
    std::string paymentMethod_;
    OrderStatus status_;
    std::vector<OrderItem> items_;
    double subtotal_;
    double discountAmount_;
    double total_;

    static void validateAmounts(double subtotal, double discountAmount);

public:
    // New order constructor.
    Order(int customerId,
          std::string date,
          std::string paymentMethod,
          std::vector<OrderItem> items,
          double subtotal,
          double discountAmount);

    // Constructor used to restore a saved order.
    Order(int id,
          int customerId,
          std::string date,
          std::string paymentMethod,
          OrderStatus status,
          std::vector<OrderItem> items,
          double subtotal,
          double discountAmount);

    int getId() const noexcept;
    int getCustomerId() const noexcept;
    const std::string& getDate() const noexcept;
    const std::string& getPaymentMethod() const noexcept;
    OrderStatus getStatus() const noexcept;
    const std::vector<OrderItem>& getItems() const noexcept;
    double getSubtotal() const noexcept;
    double getDiscountAmount() const noexcept;
    double getTotal() const noexcept;

    bool advanceStatus(std::string& message);
};

#endif
