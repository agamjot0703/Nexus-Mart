#include "Order.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

int Order::nextId_ = 5001;

std::string orderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::Placed: return "PLACED";
        case OrderStatus::Processing: return "PROCESSING";
        case OrderStatus::Shipped: return "SHIPPED";
        case OrderStatus::Delivered: return "DELIVERED";
    }
    return "UNKNOWN";
}

bool orderStatusFromString(const std::string& text, OrderStatus& status) {
    if (text == "PLACED") status = OrderStatus::Placed;
    else if (text == "PROCESSING") status = OrderStatus::Processing;
    else if (text == "SHIPPED") status = OrderStatus::Shipped;
    else if (text == "DELIVERED") status = OrderStatus::Delivered;
    else return false;
    return true;
}

OrderItem::OrderItem(const CartItem& item)
    : productId_(item.getProductId()),
      productName_(item.getProductName()),
      category_(item.getCategory()),
      unitPrice_(item.getUnitPrice()),
      quantity_(item.getQuantity()) {}

OrderItem::OrderItem(int productId, std::string name, std::string category,
                     double unitPrice, int quantity)
    : productId_(productId),
      productName_(std::move(name)),
      category_(std::move(category)),
      unitPrice_(unitPrice),
      quantity_(quantity) {
    if (productId_ <= 0 || productName_.empty() || category_.empty() ||
        !std::isfinite(unitPrice_) || unitPrice_ < 0.0 || quantity_ <= 0) {
        throw std::invalid_argument("Invalid order item data.");
    }
}

int OrderItem::getProductId() const noexcept { return productId_; }
const std::string& OrderItem::getProductName() const noexcept { return productName_; }
const std::string& OrderItem::getCategory() const noexcept { return category_; }
double OrderItem::getUnitPrice() const noexcept { return unitPrice_; }
int OrderItem::getQuantity() const noexcept { return quantity_; }
double OrderItem::getLineTotal() const noexcept { return unitPrice_ * quantity_; }

void Order::validateAmounts(double subtotal, double discountAmount) {
    if (!std::isfinite(subtotal) || !std::isfinite(discountAmount) ||
        subtotal < 0.0 || discountAmount < 0.0 || discountAmount > subtotal) {
        throw std::invalid_argument("Invalid order subtotal or discount.");
    }
}

Order::Order(int customerId,
             std::string date,
             std::string paymentMethod,
             std::vector<OrderItem> items,
             double subtotal,
             double discountAmount)
    : id_(nextId_++),
      customerId_(customerId),
      date_(std::move(date)),
      paymentMethod_(std::move(paymentMethod)),
      status_(OrderStatus::Placed),
      items_(std::move(items)),
      subtotal_(subtotal),
      discountAmount_(discountAmount),
      total_(subtotal - discountAmount) {
    if (customerId_ <= 0 || date_.empty() || paymentMethod_.empty() || items_.empty()) {
        throw std::invalid_argument("Invalid order data.");
    }
    validateAmounts(subtotal_, discountAmount_);
}

Order::Order(int id,
             int customerId,
             std::string date,
             std::string paymentMethod,
             OrderStatus status,
             std::vector<OrderItem> items,
             double subtotal,
             double discountAmount)
    : id_(id),
      customerId_(customerId),
      date_(std::move(date)),
      paymentMethod_(std::move(paymentMethod)),
      status_(status),
      items_(std::move(items)),
      subtotal_(subtotal),
      discountAmount_(discountAmount),
      total_(subtotal - discountAmount) {
    if (id_ <= 0 || customerId_ <= 0 || date_.empty() || paymentMethod_.empty() || items_.empty()) {
        throw std::invalid_argument("Invalid saved order data.");
    }
    validateAmounts(subtotal_, discountAmount_);
    nextId_ = std::max(nextId_, id_ + 1);
}

int Order::getId() const noexcept { return id_; }
int Order::getCustomerId() const noexcept { return customerId_; }
const std::string& Order::getDate() const noexcept { return date_; }
const std::string& Order::getPaymentMethod() const noexcept { return paymentMethod_; }
OrderStatus Order::getStatus() const noexcept { return status_; }
const std::vector<OrderItem>& Order::getItems() const noexcept { return items_; }
double Order::getSubtotal() const noexcept { return subtotal_; }
double Order::getDiscountAmount() const noexcept { return discountAmount_; }
double Order::getTotal() const noexcept { return total_; }

bool Order::advanceStatus(std::string& message) {
    switch (status_) {
        case OrderStatus::Placed:
            status_ = OrderStatus::Processing;
            message = "Order status changed to PROCESSING.";
            return true;
        case OrderStatus::Processing:
            status_ = OrderStatus::Shipped;
            message = "Order status changed to SHIPPED.";
            return true;
        case OrderStatus::Shipped:
            status_ = OrderStatus::Delivered;
            message = "Order status changed to DELIVERED.";
            return true;
        case OrderStatus::Delivered:
            message = "This order is already delivered.";
            return false;
    }
    message = "Unknown order status.";
    return false;
}
