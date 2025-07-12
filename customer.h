#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
using namespace std;



class customer{
public:

    string name;
    string phoneNumber;
    double balance;
    string address = "";
    vector<string> transactions;
    string filePath = "";


    customer() = default;
    bool loadFromFile(const string file_name);
    void setFilePath(const string file_path);
    bool saveFile();

    void setData(string new_name, string new_phone, double new_balance, string new_address){
        name = new_name; phoneNumber = new_phone; balance = new_balance;
        address = (new_address.empty() ? "<>" : new_address);
    }

    void clear(){
        name = "";
        phoneNumber = "";
        balance = 0;
        transactions.clear();
    }

};


inline bool customer::saveFile(){
    string fullPath = filePath + "/" + name + ".txt";
    ofstream file(fullPath);

    if(!file)
        return false;

    file << phoneNumber << endl;
    file << balance << endl;
    file << address << endl;

    // <transaction with format of = <+/-><amount><data><comment(leave blank if none)>
    for(int i = 0; i < transactions.size(); i++){
        file << transactions[i] << endl;
    }

    file.close();
    return true;
}


inline void customer::setFilePath(const string file_path){
    filePath = file_path;

    namespace fs = std::filesystem;

    if (!fs::exists(file_path)) {
        try {
            fs::create_directories(file_path);
        } catch (const fs::filesystem_error& e) {
            cerr << "Failed to create directory: " << e.what() << endl;
        }
    }
}


inline bool customer::loadFromFile(const string customerName) {

    name = customerName;

    // Combine filePath and file_name
    string fullPath = filePath;
    if (!fullPath.empty() && fullPath.back() != '/' && fullPath.back() != '\\') {
        fullPath += '/';  // or use filesystem::path if using C++17
    }
    fullPath += customerName + ".txt";

    ifstream file(fullPath);
    if (!file.is_open()) {
        cout << "Error: Could not open file: " << fullPath << endl;
        return false;
    }

    string line;
    int lineNum = 0;

    while (getline(file, line)) {
        if (line.empty()) continue;

        switch (lineNum) {
        case 0:
            phoneNumber = line;
            break;
        case 1:
            try {
                balance = stod(line);
            } catch (...) {
                cout << "Invalid balance format in file." << endl;
                balance = 0.0;
            }
            break;
        case 2:
            address = line;
            break;
        default:
            transactions.push_back(line);
            break;
        }

        lineNum++;
    }

    file.close();
    return true;
}






/*
 *
 * Example text file:
 * name = file_name (this is going to be the file name), E.g:
 * "emanuel sanchez.txt"
 * ------------------------------------------------------------------
 * 2816858101
 * 3000
 * 6211 osprey dr
 * +200.00|07/07/2025|fixing a mistake of taking away 200
 * -200.00|07/07/2025|this here is a testing mistake
 * +3000.00|07/06/2025|
 * ------------------------------------------------------------------
 *
 *
 * FORMAT:
 * <file_name>.txt
 * ------------------------------------------------------------------
 * <phone number>
 * <balance>
 * <address>
 * <transaction with format of = <+/->|<amount>|<date>|<comment(leave blank if none)>
 * <transaction with format of = <+/->|<amount>|<date>|<comment(leave blank if none)>
 * <transaction with format of = <+/->|<amount>|<date>|<comment(leave blank if none)>
 * ...
 * <transaction with format of = <+/-><amount><date><comment(leave blank if none)>
 * ------------------------------------------------------------------
 *
 */






#endif // CUSTOMER_H
