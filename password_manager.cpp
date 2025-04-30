#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

const string FILENAME = "passwords.dat";
const int ENCRYPTION_KEY = 5;  // Caesar cipher shift

// Utility functions
string encrypt(const string& text) {
    string result = text;
    for (char& ch : result) {
        ch += ENCRYPTION_KEY;
    }
    return result;
}

string decrypt(const string& text) {
    string result = text;
    for (char& ch : result) {
        ch -= ENCRYPTION_KEY;
    }
    return result;
}

// Class to hold each credential
class Credential {
public:
    string website;
    string username;
    string password;

    Credential(string site, string user, string pass) {
        website = site;
        username = user;
        password = pass;
    }

    string toStringEncrypted() {
        return encrypt(website) + "|" + encrypt(username) + "|" + encrypt(password);
    }

    static Credential fromEncryptedString(const string& data) {
        size_t pos1 = data.find('|');
        size_t pos2 = data.find('|', pos1 + 1);

        string enc_site = data.substr(0, pos1);
        string enc_user = data.substr(pos1 + 1, pos2 - pos1 - 1);
        string enc_pass = data.substr(pos2 + 1);

        return Credential(decrypt(enc_site), decrypt(enc_user), decrypt(enc_pass));
    }

    void display() {
        cout << left << setw(20) << website << setw(20) << username << setw(20) << password << endl;
    }
};

// Class to manage all credentials
class PasswordManager {
private:
    vector<Credential> credentials;

    void loadFromFile() {
        credentials.clear();
        ifstream file(FILENAME);
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                credentials.push_back(Credential::fromEncryptedString(line));
            }
        }
        file.close();
    }

    void saveToFile() {
        ofstream file(FILENAME);
        for (const auto& cred : credentials) {
            file << cred.toStringEncrypted() << endl;
        }
        file.close();
    }

public:
    PasswordManager() {
        loadFromFile();
    }

    void addCredential() {
        string site, user, pass;
        cout << "Enter website: ";
        cin >> site;
        cout << "Enter username: ";
        cin >> user;
        cout << "Enter password: ";
        cin >> pass;
        credentials.push_back(Credential(site, user, pass));
        saveToFile();
        cout << "Credential added.\n";
    }

    void viewCredentials() {
        if (credentials.empty()) {
            cout << "No credentials saved.\n";
            return;
        }
        cout << left << setw(20) << "Website" << setw(20) << "Username" << setw(20) << "Password\n";
        cout << string(60, '-') << endl;
        for (const auto& cred : credentials) {
            cred.display();
        }
    }

    void searchCredential() {
        string keyword;
        cout << "Enter website or username to search: ";
        cin >> keyword;

        bool found = false;
        for (const auto& cred : credentials) {
            if (cred.website.find(keyword) != string::npos || cred.username.find(keyword) != string::npos) {
                if (!found) {
                    cout << left << setw(20) << "Website" << setw(20) << "Username" << setw(20) << "Password\n";
                    cout << string(60, '-') << endl;
                }
                cred.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No match found.\n";
        }
    }

    void deleteCredential() {
        string site;
        cout << "Enter website to delete: ";
        cin >> site;
        bool deleted = false;

        for (auto it = credentials.begin(); it != credentials.end(); ++it) {
            if (it->website == site) {
                credentials.erase(it);
                saveToFile();
                cout << "Credential deleted.\n";
                deleted = true;
                break;
            }
        }

        if (!deleted) {
            cout << "Website not found.\n";
        }
    }
};

// Menu
void showMenu() {
    cout << "\n=== Password Manager ===\n";
    cout << "1. Add Credential\n";
    cout << "2. View All Credentials\n";
    cout << "3. Search Credential\n";
    cout << "4. Delete Credential\n";
    cout << "5. Exit\n";
    cout << "Choose an option: ";
}

// Main
int main() {
    PasswordManager manager;
    int choice;

    do {
        showMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                manager.addCredential();
                break;
            case 2:
                manager.viewCredentials();
                break;
            case 3:
                manager.searchCredential();
                break;
            case 4:
                manager.deleteCredential();
                break;
            case 5:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid option.\n";
        }
    } while (choice != 5);

    return 0;
}
