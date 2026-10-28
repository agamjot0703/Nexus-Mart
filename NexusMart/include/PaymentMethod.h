#ifndef NEXUSMART_PAYMENTMETHOD_H
#define NEXUSMART_PAYMENTMETHOD_H

#include <string>

class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;
    virtual bool pay(double amount, std::string& message) = 0;
    virtual std::string getName() const = 0;
};

class CashOnDelivery : public PaymentMethod {
public:
    bool pay(double amount, std::string& message) override;
    std::string getName() const override;
};

class UPIPayment : public PaymentMethod {
private:
    std::string upiId_;

public:
    explicit UPIPayment(std::string upiId);
    bool pay(double amount, std::string& message) override;
    std::string getName() const override;
};

#endif
