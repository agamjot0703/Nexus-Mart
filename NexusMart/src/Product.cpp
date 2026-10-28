#include "Product.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <utility>

int Product::nextId_ = 100;

void Product::validate(const std::string& name,
                       const std::string& category,
                       double price,
                       int stock) {
    if (name.empty()) {
        throw std::invalid_argument("Product name cannot be empty.");
    }
    if (category.empty()) {
        throw std::invalid_argument("Product category cannot be empty.");
    }
    if (!std::isfinite(price) || price < 0.0) {
        throw std::invalid_argument("Product price must be a finite, non-negative number.");
    }
    if (stock < 0) {
        throw std::invalid_argument("Product stock cannot be negative.");
    }
}

Product::Product(std::string name, std::string category, double price, int stock)
    : id_(nextId_++),
      name_(std::move(name)),
      category_(std::move(category)),
      price_(price),
      stock_(stock) {
    validate(name_, category_, price_, stock_);
}

Product::Product(int id, std::string name, std::string category, double price, int stock)
    : id_(id),
      name_(std::move(name)),
      category_(std::move(category)),
      price_(price),
      stock_(stock) {
    if (id_ <= 0) {
        throw std::invalid_argument("Product ID must be positive.");
    }
    validate(name_, category_, price_, stock_);
    nextId_ = std::max(nextId_, id_ + 1);
}

int Product::getId() const noexcept { return id_; }
const std::string& Product::getName() const noexcept { return name_; }
const std::string& Product::getCategory() const noexcept { return category_; }
double Product::getPrice() const noexcept { return price_; }
int Product::getStock() const noexcept { return stock_; }

bool Product::isAvailable(int quantity) const noexcept {
    return quantity > 0 && stock_ >= quantity;
}

void Product::setPrice(double price) {
    if (!std::isfinite(price) || price < 0.0) {
        throw std::invalid_argument("Product price must be a finite, non-negative number.");
    }
    price_ = price;
}

void Product::setStock(int stock) {
    if (stock < 0) {
        throw std::invalid_argument("Product stock cannot be negative.");
    }
    stock_ = stock;
}

void Product::reduceStock(int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be positive.");
    }
    if (quantity > stock_) {
        throw std::invalid_argument("Insufficient stock.");
    }
    stock_ -= quantity;
}

std::ostream& operator<<(std::ostream& out, const Product& product) {
    out << "Product #" << product.id_ << " | " << product.name_
        << " | " << product.category_ << " | INR "
        << std::fixed << std::setprecision(2) << product.price_
        << " | Stock: " << product.stock_;
    return out;
}
