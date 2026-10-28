#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Product.cpp src/User.cpp src/ShoppingCart.cpp src/Order.cpp src/PaymentMethod.cpp src/FileManager.cpp src/Store.cpp src/ConsoleUI.cpp -o nexus_mart
exec ./nexus_mart
