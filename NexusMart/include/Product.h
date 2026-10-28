#ifndef NEXUSMART_PRODUCT_H
#define NEXUSMART_PRODUCT_H

#include <iosfwd>
#include <string>

class Product {
private:
    static int nextId_;

    int id_;
    std::string name_;
    std::string category_;
    double price_;
    int stock_;

    static void validate(const std::string& name,
                         const std::string& category,
                         double price,
                         int stock);

public:
    // Constructor overload: create a new product with an automatically assigned ID.
    Product(std::string name, std::string category, double price, int stock);

    // Constructor overload: restore a product from persistent storage.
    Product(int id, std::string name, std::string category, double price, int stock);

    int getId() const noexcept;
    const std::string& getName() const noexcept;
    const std::string& getCategory() const noexcept;
    double getPrice() const noexcept;
    int getStock() const noexcept;
    bool isAvailable(int quantity = 1) const noexcept;

    void setPrice(double price);
    void setStock(int stock);
    void reduceStock(int quantity);

    // A friend is useful here because the output operator needs private product data.
    friend std::ostream& operator<<(std::ostream& out, const Product& product);
};

#endif
