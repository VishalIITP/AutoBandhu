#include <iostream>
#include <string>
#include <vector>
#include <numeric>// std::accumulate
#include <queue>
#include <array>
#include <stdexcept>// std::invalid_argument
#include <algorithm>// std::max

// Forward declaration of Stand
class Stand;

class Auto {
private:
    int id;
    bool available;
    std::vector<Stand*> route;
    std::vector<double> segmentFares;
    double minFare;

public:
    Auto(int autoId, double minimumFare = 25.0)
      : id(autoId), available(true), minFare(minimumFare)
    {}

    int getId() const { return id; }
    bool isAvailable() const { return available; }
    void setAvailable(bool v) { available = v; }

    void setRoute(const std::vector<Stand*>& r,
                  const std::vector<double>& fares)
    {
        if (r.size() < 2 || fares.size() != r.size() - 1)
            throw std::invalid_argument("Route & fare vector sizes mismatch");
        route = r;
        segmentFares = fares;
    }

    double totalFare() const {
        double sum = std::accumulate(segmentFares.begin(),
                                     segmentFares.end(), 0.0);
        return std::max(sum, minFare);
    }

    Stand* targetStand() const;

    void clearRoute() {
        route.clear();
        segmentFares.clear();
    }

    void displayStatus() const;
};

class Stand {
private:
    std::string name;
    std::queue<std::array<std::string,4>> passengerQueue;
    std::queue<Auto*> autoQueue;

public:
    Stand(const std::string& standName) : name(standName) {}

    const std::string& getName() const { return name; }

    void addAuto(Auto* a) {
        a->clearRoute();
        a->setAvailable(true);
        autoQueue.push(a);
    }

    void bookGroup(const std::array<std::string,4>& riders) {
        passengerQueue.push(riders);
    }

    void assignNext(const std::vector<Stand*>& route,
                    const std::vector<double>& fares)
    {
        if (passengerQueue.empty()) {
            std::cout << "[" << name << "] No waiting passenger groups.\n";
            return;
        }
        if (autoQueue.empty()) {
            std::cout << "[" << name << "] No autos available.\n";
            return;
        }

        auto group = passengerQueue.front(); passengerQueue.pop();
        Auto* a = autoQueue.front(); autoQueue.pop();

        a->setAvailable(false);
        a->setRoute(route, fares);

        std::cout << "[" << name << "] Dispatching Auto "
                  << a->getId() << " with passengers: ";
        for (auto& p : group) std::cout << p << " ";
        std::cout << "| Fare: " << a->totalFare() << "\n";
    }

    void receiveReturnedAuto(Auto* a) {
        std::cout << "[" << name << "] Auto " << a->getId()
                  << " arrived; now available.\n";
        addAuto(a);
    }
};

// Implementing Auto methods that need Stand definition
Stand* Auto::targetStand() const {
    return route.empty() ? nullptr : route.back();
}

void Auto::displayStatus() const {
    std::cout << "[Auto " << id << "] "
              << (available ? "Available" : "Busy")
              << ", Trip Fare: " << totalFare()
              << ", Target: "
              << (route.empty() ? "N/A"
                  : route.back()->getName())
              << "\n";
}

int main() {
    // Create stands
    Stand s1("Stand-A"), s2("Stand-B"), s3("Stand-C");

    // Create autos
    Auto a101(101), a102(102), a103(103);

    // Park autos at Stand-A
    s1.addAuto(&a101);
    s1.addAuto(&a102);
    s1.addAuto(&a103);

    // Book two groups
    s1.bookGroup({"Alice","Bob","Carol","David"});
    s1.bookGroup({"Eve","Frank","Grace","Heidi"});

    // Assign them: route A→B→C, fares 30,20
    std::vector<Stand*> route = {&s1,&s2,&s3};
    std::vector<double> fares = {30.0,20.0};
    s1.assignNext(route, fares);
    s1.assignNext(route, fares);

    // Simulate arrival of a101 at Stand-C
    s3.receiveReturnedAuto(&a101);

    // New booking at Stand-C
    s3.bookGroup({"Ivan","Judy","Karl","Leo"});
    s3.assignNext({&s3,&s1}, {25.0});  // e.g. C→A, 25 flat

    return 0;
}
