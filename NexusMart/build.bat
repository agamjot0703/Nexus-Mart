@echo off
setlocal
where g++ >nul 2>nul
if errorlevel 1 (
  echo ERROR: g++ was not found. Install MinGW-w64 and add its bin folder to PATH.
  pause
  exit /b 1
)

g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\main.cpp src\Product.cpp src\User.cpp src\ShoppingCart.cpp src\Order.cpp src\PaymentMethod.cpp src\FileManager.cpp src\Store.cpp src\ConsoleUI.cpp -o nexus_mart.exe
if errorlevel 1 (
  echo Build failed. Read the compiler errors above.
  pause
  exit /b 1
)
echo Build successful: nexus_mart.exe
endlocal
