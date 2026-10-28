#ifndef NEXUSMART_SHOPPINGCART_H
#define NEXUSMART_SHOPPINGCART_H

#include "Product.h"

#include <string>
#include <vector>

class CartItem {
private:
    int productId_;
    std::string productName_;
    std::string category_;
    double unitPrice_;
    int quantity_;

public:
    CartItem(const Product& product, int quantity);

    int getProductId() const noexcept;
    const std::string& getProductName() const noexcept;
    const std::string& getCategory() const noexcept;
    double getUnitPrice() const noexcept;
    int getQuantity() const noexcept;
    double getLineTotal() const noexcept;

    void setQuantity(int quantity);
    void addQuantity(int quantity);
};

class ShoppingCart {
private:
    std::vector<CartItem> items_;
    std::string couponCode_;
    double discountPercent_;

public:
    ShoppingCart();

    const std::vector<CartItem>& getItems() const noexcept;
    bool empty() const noexcept;
    std::size_t itemCount() const noexcept;
    int quantityFor(int productId) const noexcept;
    const CartItem* findItem(int productId) const noexcept;

    void addProduct(const Product& product, int quantity);
    bool setQuantity(int productId, int quantity);
    bool removeProduct(int productId);
    bool applyCoupon(const std::string& code, std::string& message);
    void clearCoupon() noexcept;
    void clear() noexcept;

    double subtotal() const noexcept;
    double discountAmount() const noexcept;
    double total() const noexcept;
    const std::string& getCouponCode() const noexcept;
    double getDiscountPercent() const noexcept;
};

#endif
