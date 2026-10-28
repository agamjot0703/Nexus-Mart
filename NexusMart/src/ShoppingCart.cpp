#include "ShoppingCart.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

CartItem::CartItem(const Product& product, int quantity)
    : productId_(product.getId()),
      productName_(product.getName()),
      category_(product.getCategory()),
      unitPrice_(product.getPrice()),
      quantity_(quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("Cart quantity must be positive.");
    }
}

int CartItem::getProductId() const noexcept { return productId_; }
const std::string& CartItem::getProductName() const noexcept { return productName_; }
const std::string& CartItem::getCategory() const noexcept { return category_; }
double CartItem::getUnitPrice() const noexcept { return unitPrice_; }
int CartItem::getQuantity() const noexcept { return quantity_; }
double CartItem::getLineTotal() const noexcept { return unitPrice_ * quantity_; }

void CartItem::setQuantity(int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("Cart quantity must be positive.");
    }
    quantity_ = quantity;
}

void CartItem::addQuantity(int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity to add must be positive.");
    }
    quantity_ += quantity;
}

ShoppingCart::ShoppingCart() : discountPercent_(0.0) {}

const std::vector<CartItem>& ShoppingCart::getItems() const noexcept { return items_; }
bool ShoppingCart::empty() const noexcept { return items_.empty(); }
std::size_t ShoppingCart::itemCount() const noexcept { return items_.size(); }

int ShoppingCart::quantityFor(int productId) const noexcept {
    const CartItem* item = findItem(productId);
    return item ? item->getQuantity() : 0;
}

const CartItem* ShoppingCart::findItem(int productId) const noexcept {
    const auto it = std::find_if(items_.begin(), items_.end(), [productId](const CartItem& item) {
        return item.getProductId() == productId;
    });
    return it == items_.end() ? nullptr : &(*it);
}

void ShoppingCart::addProduct(const Product& product, int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be positive.");
    }

    const auto it = std::find_if(items_.begin(), items_.end(), [&product](const CartItem& item) {
        return item.getProductId() == product.getId();
    });

    if (it == items_.end()) {
        items_.emplace_back(product, quantity);
    } else {
        it->addQuantity(quantity);
    }
}

bool ShoppingCart::setQuantity(int productId, int quantity) {
    if (quantity <= 0) {
        return false;
    }
    const auto it = std::find_if(items_.begin(), items_.end(), [productId](const CartItem& item) {
        return item.getProductId() == productId;
    });
    if (it == items_.end()) {
        return false;
    }
    it->setQuantity(quantity);
    return true;
}

bool ShoppingCart::removeProduct(int productId) {
    const auto oldSize = items_.size();
    items_.erase(std::remove_if(items_.begin(), items_.end(), [productId](const CartItem& item) {
        return item.getProductId() == productId;
    }), items_.end());
    return items_.size() != oldSize;
}

bool ShoppingCart::applyCoupon(const std::string& code, std::string& message) {
    std::string normalized = code;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });

    if (normalized == "SAVE10") {
        couponCode_ = normalized;
        discountPercent_ = 10.0;
        message = "Coupon SAVE10 applied: 10% off the cart subtotal.";
        return true;
    }
    if (normalized == "SAVE5") {
        couponCode_ = normalized;
        discountPercent_ = 5.0;
        message = "Coupon SAVE5 applied: 5% off the cart subtotal.";
        return true;
    }

    message = "Invalid coupon. Try SAVE10 or SAVE5.";
    return false;
}

void ShoppingCart::clearCoupon() noexcept {
    couponCode_.clear();
    discountPercent_ = 0.0;
}

void ShoppingCart::clear() noexcept {
    items_.clear();
    clearCoupon();
}

double ShoppingCart::subtotal() const noexcept {
    double result = 0.0;
    for (const auto& item : items_) {
        result += item.getLineTotal();
    }
    return result;
}

double ShoppingCart::discountAmount() const noexcept {
    return subtotal() * discountPercent_ / 100.0;
}

double ShoppingCart::total() const noexcept {
    return subtotal() - discountAmount();
}

const std::string& ShoppingCart::getCouponCode() const noexcept { return couponCode_; }
double ShoppingCart::getDiscountPercent() const noexcept { return discountPercent_; }
