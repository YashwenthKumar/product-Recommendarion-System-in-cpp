#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <limits>
#include <fstream>
#include <algorithm>

using namespace std;

// Graph structure to hold users and products
class Graph {
    unordered_map<string, vector<pair<string, double>>> adjList;

public:
    void addNode(const string& node) {
        if (adjList.find(node) == adjList.end()) {
            adjList[node] = vector<pair<string, double>>();
        }
    }

    void addEdge(const string& node1, const string& node2, double weight = 1.0) {
        adjList[node1].push_back(make_pair(node2, weight));
        adjList[node2].push_back(make_pair(node1, weight));
    }

    const vector<pair<string, double>>& getNeighbors(const string& node) const {
        return adjList.at(node);
    }

    bool containsNode(const string& node) const {
        return adjList.find(node) != adjList.end();
    }

    void displayGraph() const {
        for (const auto& node : adjList) {
            cout << node.first << ": ";
            for (const auto& edge : node.second) {
                cout << "(" << edge.first << ", " << edge.second << ") ";
            }
            cout << endl;
        }
    }
};

class Product {
public:
    string productName;
    string category;
    double price;
    string description;

    Product(const string& name, const string& category, double price, const string& description)
        : productName(name), category(category), price(price), description(description) {}
};

vector<string> recommendProducts(const Graph& g, const string& user, int maxRecommendations) {
    if (!g.containsNode(user)) {
        cout << "User not found!" << endl;
        return {};
    }

    set<string> visited;
    priority_queue<pair<double, string>> pq;
    unordered_map<string, double> productScores;

    queue<string> bfsQueue;
    bfsQueue.push(user);
    visited.insert(user);

    while (!bfsQueue.empty()) {
        string currentUser = bfsQueue.front();
        bfsQueue.pop();

        for (const auto& neighbor : g.getNeighbors(currentUser)) {
            string neighborNode = neighbor.first;
            double weight = neighbor.second;

            if (g.containsNode(neighborNode)) {
                productScores[neighborNode] += weight;
            } else if (visited.find(neighborNode) == visited.end()) {
                bfsQueue.push(neighborNode);
                visited.insert(neighborNode);
            }
        }
    }

    for (const auto& score : productScores) {
        pq.push({score.second, score.first});
    }

    vector<string> recommendations;
    while (!pq.empty() && recommendations.size() < maxRecommendations) {
        recommendations.push_back(pq.top().second);
        pq.pop();
    }

    return recommendations;
}

int getIntInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (!cin.fail() && value > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << "Invalid input. Please enter a positive integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

double getDoubleInput(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (!cin.fail() && value >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        } else {
            cout << "Invalid input. Please enter a non-negative number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string getStringInput(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        cin >> ws;
        getline(cin, value);
        if (!value.empty()) {
            return value;
        } else {
            cout << "Input cannot be empty. Please try again.\n";
        }
    }
}

Product createProduct() {
    cout << "Adding a New Product" << endl;
    string name = getStringInput("Enter the name of the product: ");
    string category = getStringInput("Enter the category of the product: ");
    double price = getDoubleInput("Enter the price of the product: ");
    string description = getStringInput("Enter a description for the product: ");

    return Product(name, category, price, description);
}

void removeProduct(vector<Product>& products, const string& productName) {
    auto it = remove_if(products.begin(), products.end(), [&](const Product& p) {
        return p.productName == productName;
    });

    if (it != products.end()) {
        products.erase(it, products.end());
        cout << "Product '" << productName << "' removed successfully!" << endl;
    } else {
        cout << "Product not found!" << endl;
    }
}

void updateProduct(vector<Product>& products, const string& productName) {
    for (auto& product : products) {
        if (product.productName == productName) {
            cout << "Updating Product: " << productName << endl;
            product.category = getStringInput("Enter the new category: ");
            product.price = getDoubleInput("Enter the new price: ");
            product.description = getStringInput("Enter the new description: ");
            cout << "Product updated successfully!" << endl;
            return;
        }
    }
    cout << "Product not found!" << endl;
}

void displayAllProducts(const vector<Product>& products) {
    cout << "Available Products" << endl;
    for (const auto& product : products) {
        cout << "Product Name: " << product.productName << ", Category: " << product.category
             << ", Price: $" << product.price << endl;
    }
}

void displayProductDetails(const vector<Product>& products, const string& productName) {
    for (const auto& product : products) {
        if (product.productName == productName) {
            cout << "Product Name: " << product.productName << endl;
            cout << "Category: " << product.category << endl;
            cout << "Price: $" << product.price << endl;
            cout << "Description: " << product.description << endl;
            return;
        }
    }
    cout << "Product not found!" << endl;
}

int main() {
    Graph g;
    vector<Product> productCatalog;

    cout << "Welcome to the Product Recommendation System" << endl;

    int numUsers = getIntInput("Enter the number of users: ");
    for (int i = 1; i <= numUsers; ++i) {
        string userName = getStringInput("Enter the name of User " + to_string(i) + ": ");
        g.addNode(userName);
    }

    int numProducts = getIntInput("Enter the number of products: ");
    for (int i = 1; i <= numProducts; ++i) {
        Product newProduct = createProduct();
        g.addNode(newProduct.productName);
        productCatalog.push_back(newProduct);
    }

    int numInteractions = getIntInput("Enter the number of user-product interactions: ");
    for (int i = 0; i < numInteractions; ++i) {
        cout << "Adding Interaction " << (i + 1) << endl;
        string userName = getStringInput("Enter the user name: ");
        string productName = getStringInput("Enter the product name: ");
        double weight = getDoubleInput("Enter the interaction weight (e.g., 1 to 5): ");

        if (g.containsNode(productName)) {
            g.addEdge(userName, productName, weight);
            cout << "Added interaction from " << userName << " to " << productName << " with weight " << weight << endl;
        } else {
            cout << "Product not found!" << endl;
        }
    }

    while (true) {
        cout << "\nMain Menu" << endl;
        cout << "1. Get Product Recommendations" << endl;
        cout << "2. Display Product Details" << endl;
        cout << "3. Display All Products" << endl;
        cout << "4. Remove a Product" << endl;
        cout << "5. Update Product Details" << endl;
        cout << "6. Display Graph (for debugging)" << endl;
        cout << "7. Exit" << endl;

        int choice = getIntInput("Choose an option (1-7): ");
        switch (choice) {
            case 1: {
                string targetUser = getStringInput("Enter the target user for recommendations: ");
                int maxRecommendations = getIntInput("Enter the maximum number of recommendations: ");
                vector<string> recommendations = recommendProducts(g, targetUser, maxRecommendations);

                cout << "Recommendations for " << targetUser << ": ";
                if (recommendations.empty()) {
                    cout << "No recommendations found." << endl;
                } else {
                    for (const auto& product : recommendations) {
                        cout << product << " ";
                    }
                    cout << endl;
                }
                break;
            }
            case 2: {
                string productName = getStringInput("Enter the product name to view details: ");
                displayProductDetails(productCatalog, productName);
                break;
            }
            case 3: {
                displayAllProducts(productCatalog);
                break;
            }
            case 4: {
                string productName = getStringInput("Enter the product name to remove: ");
                removeProduct(productCatalog, productName);
                break;
            }
            case 5: {
                string productName = getStringInput("Enter the product name to update: ");
                updateProduct(productCatalog, productName);
                break;
            }
            case 6: {
                cout << "Graph Structure" << endl;
                g.displayGraph();
                break;
            }
            case 7: {
                cout << "Exiting the program. Thank you!" << endl;
                return 0;
            }
            default: {
                cout << "Invalid choice. Please select a valid option." << endl;
            }
        }
    }

    return 0;
}
