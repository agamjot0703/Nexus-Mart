# Manual Test Checklist

Run from the project root so the application finds `data/`.

## Startup and accounts

- [ ] Application launches with a product catalog.
- [ ] Admin login `admin / admin123` succeeds.
- [ ] Incorrect password is rejected.
- [ ] Registering a new customer succeeds.
- [ ] Registering a duplicate username in a different letter case is rejected.
- [ ] Username shorter than 3 characters is rejected.
- [ ] Password shorter than 4 characters is rejected.

## Catalog and inventory

- [ ] Browse displays the eight sample products.
- [ ] Search `keyboard` finds Mechanical Keyboard.
- [ ] Search using a category filter returns matching products only.
- [ ] Invalid product IDs are rejected.
- [ ] Admin can add a product and receives a new ID.
- [ ] Admin can update a product price.
- [ ] Admin can set stock to zero.
- [ ] Admin can remove a product.
- [ ] Negative prices and negative stock are rejected.

## Cart

- [ ] Adding a product puts it in the cart.
- [ ] Adding the same product again increases its quantity.
- [ ] Adding more than available stock is rejected.
- [ ] Updating cart quantity above stock is rejected.
- [ ] Removing a cart item works.
- [ ] `SAVE10` reduces subtotal by 10%.
- [ ] `SAVE5` reduces subtotal by 5%.
- [ ] An unknown coupon is rejected.
- [ ] Removing a coupon restores the original subtotal.
- [ ] Empty cart cannot be checked out.

## Checkout and order history

- [ ] Cash on Delivery creates an order.
- [ ] A valid-format demo UPI ID is accepted without performing a real transaction.
- [ ] An invalid UPI ID fails and leaves cart/stock unchanged.
- [ ] Checkout deducts stock.
- [ ] Checkout creates a unique order ID.
- [ ] Order history retains product name, quantity, and original unit price.
- [ ] Removing a product after an order does not erase the order snapshot.
- [ ] Order data remains after restarting the application.

## Order management and persistence

- [ ] Admin can view all orders.
- [ ] Status moves PLACED -> PROCESSING -> SHIPPED -> DELIVERED.
- [ ] A delivered order cannot advance further.
- [ ] Status remains changed after restart.
- [ ] Product price and stock changes remain after restart.
- [ ] The application reports persistence errors rather than silently ignoring failed saves.
