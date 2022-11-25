#pragma once
class IMNetwork {
public:
        virtual int sayHello() const {
                printf("From IMNetwork");
                return 20;
        }
        virtual ~IMNetwork() {}
};