#include "User.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

int User::nextId_ = 1;

User::User(int id, std::string username, std::string password)
    : id_(id), username_(std::move(username)), password_(std::move(password)) {
    if (id_ <= 0) {
        throw std::invalid_argument("User ID must be positive.");
    }
    if (username_.empty()) {
        throw std::invalid_argument("Username cannot be empty.");
    }
    if (password_.empty()) {
        throw std::invalid_argument("Password cannot be empty.");
    }
    nextId_ = std::max(nextId_, id_ + 1);
}

int User::generateId() { return nextId_++; }
int User::getId() const noexcept { return id_; }
const std::string& User::getUsername() const noexcept { return username_; }
bool User::verifyPassword(const std::string& password) const noexcept { return password_ == password; }

Customer::Customer(std::string username, std::string password)
    : User(User::generateId(), std::move(username), std::move(password)) {}

Customer::Customer(int id, std::string username, std::string password)
    : User(id, std::move(username), std::move(password)) {}

std::string Customer::getRole() const { return "CUSTOMER"; }
ShoppingCart& Customer::getCart() noexcept { return cart_; }
const ShoppingCart& Customer::getCart() const noexcept { return cart_; }

Admin::Admin(std::string username, std::string password)
    : User(User::generateId(), std::move(username), std::move(password)) {}

Admin::Admin(int id, std::string username, std::string password)
    : User(id, std::move(username), std::move(password)) {}

std::string Admin::getRole() const { return "ADMIN"; }
