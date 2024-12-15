#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

class SimplePasswordManager {
private:
    std::map<std::string, std::string> passwords;
    std::string masterPassword;
    const std::string STORAGE_FILE = "passwords.txt";
    
    // Simple XOR encryption (Note: This is not secure for production use)
    std::string simpleEncrypt(const std::string& input) const {
        std::string output = input;
        for (size_t i = 0; i < input.length(); ++i) {
            output[i] = input[i] ^ masterPassword[i % masterPassword.length()];
        }
        return output;
    }
    
    std::string simpleDecrypt(const std::string& input) const {
        return simpleEncrypt(input); // XOR encryption is reversible
    }
    
    void saveToFile() const {
        std::ofstream file(STORAGE_FILE, std::ios::binary);
        for (const auto& pair : passwords) {
            std::string encrypted = simpleEncrypt(pair.second);
            file << pair.first << ":" << encrypted << "\n";
        }
    }
    
    void loadFromFile() {
        std::ifstream file(STORAGE_FILE, std::ios::binary);
        std::string line;
        while (std::getline(file, line)) {
            size_t delimiter = line.find(':');
            if (delimiter != std::string::npos) {
                std::string service = line.substr(0, delimiter);
                std::string encryptedPassword = line.substr(delimiter + 1);
                passwords[service] = simpleDecrypt(encryptedPassword);
            }
        }
    }

public:
    SimplePasswordManager(const std::string& masterPass) : masterPassword(masterPass) {
        loadFromFile();
    }
    
    // Rest of the public methods remain the same as in your original code
    bool addPassword(const std::string& service, const std::string& password) {
        passwords[service] = password;
        saveToFile();
        return true;
    }
    
    std::string getPassword(const std::string& service) const {
        auto it = passwords.find(service);
        return (it != passwords.end()) ? it->second : "";
    }
    
    bool removePassword(const std::string& service) {
        auto result = passwords.erase(service);
        if (result > 0) {
            saveToFile();
            return true;
        }
        return false;
    }
    
    void listServices() const {
        for (const auto& pair : passwords) {
            std::cout << pair.first << "\n";
        }
    }
};

int main() {
    std::string masterPassword;
    std::cout << "Enter master password: ";
    std::getline(std::cin, masterPassword);
    
    SimplePasswordManager manager(masterPassword);
    
    while (true) {
        std::cout << "\n1. Add password\n"
                  << "2. Get password\n"
                  << "3. Remove password\n"
                  << "4. List services\n"
                  << "5. Exit\n"
                  << "Choose an option: ";
                  
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice == 5) break;
        
        std::string service, password;
        
        switch (choice) {
            case 1:
                std::cout << "Enter service name: ";
                std::getline(std::cin, service);
                std::cout << "Enter password: ";
                std::getline(std::cin, password);
                if (manager.addPassword(service, password)) {
                    std::cout << "Password added successfully!\n";
                }
                break;
                
            case 2:
                std::cout << "Enter service name: ";
                std::getline(std::cin, service);
                password = manager.getPassword(service);
                if (!password.empty()) {
                    std::cout << "Password: " << password << "\n";
                } else {
                    std::cout << "Service not found.\n";
                }
                break;
                
            case 3:
                std::cout << "Enter service name: ";
                std::getline(std::cin, service);
                if (manager.removePassword(service)) {
                    std::cout << "Password removed successfully!\n";
                } else {
                    std::cout << "Service not found.\n";
                }
                break;
                
            case 4:
                std::cout << "Services:\n";
                manager.listServices();
                break;
                
            default:
                std::cout << "Invalid option!\n";
        }
    }
    
    return 0;
}
