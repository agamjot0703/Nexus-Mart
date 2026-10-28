# Nexus Mart — C++17 Console E-Commerce Project

Nexus Mart is a terminal-based mini e-commerce platform written in standard C++17. It has no GUI framework, database, network calls, or third-party libraries. It is designed to be substantial enough for an OOP course project while remaining understandable in one evening.

## Features

### Customer
- Register and log in.
- Browse and search the product catalog (case-insensitive search and optional category filter).
- Add products to a cart, update quantities, and remove items.
- Apply `SAVE10` (10%) or `SAVE5` (5%) coupon codes.
- Check out using Cash on Delivery or a simulated UPI payment.
- View order history, including the product prices captured at purchase time.

### Administrator
- View, search, add, update, and remove inventory items.
- View all orders and move each order through `PLACED -> PROCESSING -> SHIPPED -> DELIVERED`.
- View basic store statistics.

### Persistence
- Products: `data/products.txt`
- User accounts: `data/users.txt`
- Orders: `data/orders.txt`

The program loads these files when it starts and writes updates as actions are completed. Files use quoted strings, so names and categories can contain spaces. Saves use temporary files and backups to reduce the chance of leaving a partially written individual file.

## Run it

### Windows + MinGW (VS Code)
1. Open the `NexusMart` folder in VS Code.
2. Ensure MinGW-w64 is installed and `g++` is available in your terminal `PATH`.
3. Open a terminal **at the project root**.
4. Run:

   ```bat
   build.bat
   run.bat
   ```

   Or compile manually:

   ```bat
   g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\main.cpp src\Product.cpp src\User.cpp src\ShoppingCart.cpp src\Order.cpp src\PaymentMethod.cpp src\FileManager.cpp src\Store.cpp src\ConsoleUI.cpp -o nexus_mart.exe
   ```

5. Run `nexus_mart.exe` from the project root so the `data` folder is found.

### Linux / macOS with g++
From the project root:

```bash
./run.sh
```

### CMake

```bash
cmake -S . -B build
cmake --build build
```

Run the compiled executable with the current working directory set to the project root, because the application uses the relative path `data/` for persistence.

## Demo login

- Username: `admin`
- Password: `admin123`

Customer accounts can be registered from the welcome menu. Product IDs `101` through `108` are seeded as demo inventory.

## Coupon codes

- `SAVE10`: 10% off the cart subtotal.
- `SAVE5`: 5% off the cart subtotal.

## Architecture

| Class | Responsibility |
|---|---|
| `Product` | Product data, validation, stock management, and output formatting |
| `User` | Abstract base for shared account data and role interface |
| `Customer` | Customer role and owned shopping cart |
| `Admin` | Administrator role |
| `CartItem` | Product snapshot and quantity in a cart |
| `ShoppingCart` | Cart operations, totals, and coupon calculation |
| `OrderItem` | A historical snapshot of a purchased item |
| `Order` | Order contents, totals, status, and status transitions |
| `PaymentMethod` | Abstract payment interface |
| `CashOnDelivery` | Simulated cash-on-delivery selection |
| `UPIPayment` | Validates a demo UPI ID; does not contact a payment service |
| `Store` | Application business logic and inventory/order coordination |
| `FileManager` | Loading and saving text-based persistence files |
| `ConsoleUI` | Menus, colored messages, tables, and input validation |

## OOP concepts demonstrated

- **Encapsulation:** important data members are private/protected and changed through methods.
- **Abstraction:** `User` and `PaymentMethod` are abstract base classes.
- **Inheritance:** `Customer` and `Admin` inherit from `User`; payment types inherit from `PaymentMethod`.
- **Runtime polymorphism:** the UI passes a `PaymentMethod&` to checkout; the selected payment implementation supplies `pay()` and `getName()`.
- **Virtual functions / overriding / dynamic binding:** `getRole()` and payment operations use virtual interfaces and overridden implementations.
- **Virtual destructors:** polymorphic base classes have virtual destructors.
- **Constructor overloading:** `Product` has a constructor for new IDs and one for restoring a saved ID; `Order` and account types also have restoration constructors.
- **Function overloading:** `Store::searchProducts()` supports keyword search and keyword-plus-category filtering.
- **Operator overloading and friend function:** `operator<<` provides a readable representation of `Product`.
- **Static members:** product, user, and order ID counters track IDs across object creation within the running process and advance after loading saved records.
- **Composition:** `Customer` owns a `ShoppingCart`; an `Order` owns a vector of `OrderItem` objects.
- **STL:** `std::vector`, `std::string`, algorithms, streams, and smart pointers.
- **Template:** `ConsoleUI::printCollection` is a small generic collection iteration helper.
- **Exception handling:** constructors and mutators validate data; invalid operations are reported rather than accepted.
- **File handling:** the `FileManager` persists products, users, and orders.
- **Separation of concerns:** UI, business rules, entities, payment behavior, and persistence are in separate modules.

## Important design decisions

- A cart line keeps the unit price captured when the item was added. That price is used for the order; this avoids a surprising price change between adding an item and checking out.
- Before checkout, the store rechecks current inventory. A missing product or insufficient stock blocks checkout.
- Payment happens only after stock validation. A failed demo UPI validation leaves the cart and inventory unchanged.
- Orders save product names, categories, prices, and quantities as snapshots. Deleting a product therefore does not erase old order details.
- Order status can only move forward one step at a time.
- The shopping cart is session-only and is not persisted between application runs.

## Security and production limitations

This is a learning/demo project, **not a production e-commerce system**. User passwords are stored as plaintext in `data/users.txt` so the file persistence and class structure remain easy to inspect. Do not use real or reused passwords. The UPI option only validates the format of a demo identifier and performs no real transaction. There is no real authorization backend, payment processor, shipping integration, concurrency control, or database. A production system would need password hashing, stronger authorization, durable transactional storage, audit logging, and real payment-provider integration.

## Suggested demonstration flow

1. Log in as admin and show inventory, add a product, then update its stock.
2. Log out and register a customer.
3. Log in as the customer, search for a product, and add it to the cart.
4. Apply `SAVE10`, then place an order using Cash on Delivery.
5. Open order history to show the saved order snapshot.
6. Log back in as admin and advance the order from `PLACED` to `PROCESSING`.
7. Exit and restart the app to demonstrate file persistence.

See [VIVA_GUIDE.md](VIVA_GUIDE.md) and [TEST_CHECKLIST.md](TEST_CHECKLIST.md) for viva preparation and manual testing.
