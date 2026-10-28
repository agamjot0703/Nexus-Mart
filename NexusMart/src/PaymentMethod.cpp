#include "PaymentMethod.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>

bool CashOnDelivery::pay(double amount, std::string& message) {
    if (!std::isfinite(amount) || amount < 0.0) {
        message = "Invalid payment amount.";
        return false;
    }
    message = "Cash on Delivery selected. Payment is due upon delivery (simulation only).";
    return true;
}

std::string CashOnDelivery::getName() const { return "Cash on Delivery"; }

UPIPayment::UPIPayment(std::string upiId) : upiId_(std::move(upiId)) {}

bool UPIPayment::pay(double amount, std::string& message) {
    if (!std::isfinite(amount) || amount < 0.0) {
        message = "Invalid payment amount.";
        return false;
    }
    const auto at = upiId_.find('@');
    if (at == std::string::npos || at == 0 || at + 1 >= upiId_.size() ||
        upiId_.find(' ') != std::string::npos) {
        message = "Invalid demo UPI ID. Enter a value such as demo@upi.";
        return false;
    }
    std::ostringstream amountText;
    amountText << std::fixed << std::setprecision(2) << amount;
    message = "Demo UPI payment accepted for INR " + amountText.str() +
              ". No real transaction was performed.";
    return true;
}

std::string UPIPayment::getName() const { return "Demo UPI"; }
