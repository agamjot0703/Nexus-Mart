# Nexus Mart — Viva Preparation Guide

## 60-second project introduction

“Nexus Mart is a C++17 console-based e-commerce platform. It supports customer registration, product searching, a shopping cart, coupon discounts, simulated checkout, order history, and administrator inventory/order management. The code is separated into entity classes, a Store business-logic class, a FileManager for text-file persistence, payment interfaces, and a ConsoleUI. The design demonstrates encapsulation, abstraction, inheritance, runtime polymorphism, composition, operator overloading, templates, STL containers, and file handling.”

## Class-by-class explanation

- **Product:** owns an ID, name, category, price, and stock. It validates prices and stock so invalid values cannot be set through its public methods.
- **User:** abstract base class for shared user identity and credential verification. Its pure virtual `getRole()` makes the class abstract.
- **Customer:** derived from `User`; owns its `ShoppingCart`.
- **Admin:** derived from `User`; the UI exposes administrator operations only after an admin account is authenticated.
- **CartItem:** captures a product ID, label, category, price, and quantity for a cart line.
- **ShoppingCart:** owns cart lines and calculates subtotal, coupon discount, and final total.
- **OrderItem:** copies the purchased product details into an order snapshot.
- **Order:** owns order items, stores the checkout totals and payment label, and restricts status changes to the allowed forward sequence.
- **PaymentMethod:** abstract interface that defines `pay()` and `getName()`.
- **CashOnDelivery / UPIPayment:** concrete payment implementations. Checkout uses a `PaymentMethod&`, not a concrete payment class.
- **Store:** coordinates product/user/order operations and checks the business rules before committing a checkout.
- **FileManager:** reads and writes the data files; it keeps file-format details out of `Store`.
- **ConsoleUI:** renders menus/tables, parses input, and passes requests to `Store` rather than modifying the inventory itself.

## OOP questions you should be ready to answer

### 1. Where is encapsulation used?
In classes such as `Product`, `ShoppingCart`, and `Order`. Their fields are private, and public methods validate changes. For example, stock can be changed through `setStock()` or reduced through `reduceStock()` rather than by writing to a public integer.

### 2. What makes `User` abstract?
It declares the pure virtual function `virtual std::string getRole() const = 0;`. A class with a pure virtual function cannot be instantiated directly.

### 3. Explain inheritance in this project.
`Customer` and `Admin` are specialized types of `User`. They share the base account interface but provide different `getRole()` implementations. `CashOnDelivery` and `UPIPayment` similarly implement `PaymentMethod`.

### 4. Where is runtime polymorphism?
`Store::checkout()` accepts a `PaymentMethod&`. When it calls `paymentMethod.pay(...)`, C++ selects the overridden implementation for the actual object passed in, such as `CashOnDelivery` or `UPIPayment`.

### 5. What is dynamic binding?
The call to an overridden virtual function is resolved using the object's dynamic type at runtime. The checkout code does not need a separate payment branch inside the Store class.

### 6. Why do the abstract base classes have virtual destructors?
If a derived object is deleted through a pointer to its base class, a virtual destructor ensures the derived destructor is invoked correctly. This is especially important for polymorphic ownership.

### 7. Explain composition using the project.
A `Customer` contains a `ShoppingCart` member. The cart is part of that customer's shopping state. An `Order` also owns a collection of its `OrderItem` objects.

### 8. Where is operator overloading used?
`Product` overloads `operator<<` so a product can be written to an output stream in a readable format. The friend declaration lets that function access private product attributes.

### 9. Where is function overloading used?
There are two `Store::searchProducts` functions: one accepts a keyword, while the other accepts a keyword and category filter. Their parameter lists distinguish the overloads.

### 10. Why use `std::vector`?
The number of products, users, orders, and cart lines changes at runtime. A vector offers dynamic sizing and straightforward iteration. It is adequate for the size of this local project.

### 11. Why use `std::unique_ptr<User>`?
The user collection stores both `Customer` and `Admin` objects through a common base pointer. `unique_ptr` gives the Store clear ownership and automatic cleanup.

### 12. What happens during checkout?
The Store rejects an empty cart, revalidates every product and stock quantity, asks the selected payment implementation to approve the simulated payment, creates an order snapshot, deducts stock, clears the cart, and persists the changes. A failed payment leaves the cart and stock unchanged.

### 13. Why save order-item snapshots?
Inventory records can be updated or removed after a purchase. An order should still show the name and price from the original purchase, so each order item stores those values directly.

### 14. Why are prices stored as `double` here?
For an educational project, `double` makes the calculation readable. A production billing system should generally represent money as integer minor units (such as paise) or use a decimal-safe representation to avoid floating-point rounding concerns.

### 15. What is the purpose of `FileManager`?
It separates persistence details from business logic. `Store` can request that state be saved without knowing how each record is serialized into a text file.

### 16. Is this production-ready?
No. Passwords are plaintext, UPI payments are simulated, there is no real payment or shipping integration, and the app uses local files rather than a transactional database. Those limitations are explicitly documented.

## Concepts to point at in code

| Concept | File / symbol |
|---|---|
| Encapsulation | `Product.h`, private attributes and mutators |
| Abstract class / pure virtual function | `User.h`, `PaymentMethod.h` |
| Inheritance / overriding | `Customer`, `Admin`, `CashOnDelivery`, `UPIPayment` |
| Runtime polymorphism | `Store::checkout()` and `PaymentMethod&` |
| Virtual destructor | `User::~User`, `PaymentMethod::~PaymentMethod` |
| Constructor overloading | `Product`, `User`-derived classes, `Order` |
| Function overloading | `Store::searchProducts` |
| Operator overloading / friend | `operator<<` in `Product.cpp` |
| Static members | ID counters in `Product`, `User`, and `Order` |
| Composition | `Customer::cart_`, `Order::items_` |
| Template | `ConsoleUI::printCollection` |
| Exception handling | constructors and `Store` mutators |
| File handling | `FileManager.cpp` |
