#ifndef NEXUSMART_USER_H
#define NEXUSMART_USER_H

#include "ShoppingCart.h"

#include <string>

class FileManager;

class User {
    friend class FileManager;
protected:
    static int nextId_;

    int id_;
    std::string username_;
    std::string password_;

    User(int id, std::string username, std::string password);

public:
    virtual ~User() = default;

    static int generateId();

    int getId() const noexcept;
    const std::string& getUsername() const noexcept;
    bool verifyPassword(const std::string& password) const noexcept;

    virtual std::string getRole() const = 0;
};

class Customer : public User {
private:
    ShoppingCart cart_;

public:
    Customer(std::string username, std::string password);
    Customer(int id, std::string username, std::string password);

    std::string getRole() const override;
    ShoppingCart& getCart() noexcept;
    const ShoppingCart& getCart() const noexcept;
};

class Admin : public User {
public:
    Admin(std::string username, std::string password);
    Admin(int id, std::string username, std::string password);

    std::string getRole() const override;
};

#endif
