#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map> 

#include "Library.h"

using namespace std;

const int ROLE_MEMBER = 0;
const int ROLE_LIBRARIAN = 1;
const int ROLE_ADMIN = 2;

map<string, int> globalUserRoles;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void saveBookToFile(string isbn, string title, string author, string subject, int year, int pages, string rack, int copies) {
    ofstream outFile("data.txt", ios::app);
    if (outFile.is_open()) {
        outFile << isbn << "|" << title << "|" << author << "|" << subject << "|" 
                << year << "|" << pages << "|" << rack << "|" << copies << "\n";
        outFile.close();
    }
}

void updateBookFile(const vector<Book>& books, const LibrarySystem& lib) {
    ofstream outFile("data.txt", ios::trunc);
    if (outFile.is_open()) {
        for (const auto& b : books) {
            
            int totalCopies = 0;
            for (const auto& copy : lib.getCopies()) {
                if (copy.getBookId() == b.getId()) totalCopies++;
            }
            outFile << b.getIsbn() << "|" << b.getTitle() << "|" << b.getAuthor() << "|" 
                    << b.getSubject() << "|" << b.getPublicationYear() << "|" 
                    << b.getPages() << "|" << b.getRackPosition() << "|" << totalCopies << "\n";
        }
        outFile.close();
    }
}

void saveUserToFile(string name, string dob, int gender, string address, string phone, string email, string password, int pref, int role) {
    ofstream outFile("users.txt", ios::app);
    if (outFile.is_open()) {
        outFile << name << "|" << dob << "|" << gender << "|" << address << "|" 
                << phone << "|" << email << "|" << password << "|" << pref << "|" << role << "\n";
        outFile.close();
        globalUserRoles[email] = role;
    }
}

void deleteUserFromFile(string emailToDelete) {
    ifstream inFile("users.txt");
    vector<string> lines;
    string line;

    while (getline(inFile, line)) {
        if (line.empty()) continue;
        vector<string> data = split(line, '|');
    
        if (data.size() >= 6) {
            if (data[5] != emailToDelete) {
                lines.push_back(line);
            }
        }
    }
    inFile.close();

    ofstream outFile("users.txt", ios::trunc);
    if (outFile.is_open()) {
        for (const auto& l : lines) {
            outFile << l << "\n";
        }
        outFile.close();
    }
}

void loadBooksFromFile(LibrarySystem& lib) {
    ifstream inFile("data.txt");
    if (!inFile.is_open()) return;
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        vector<string> data = split(line, '|');
        if (data.size() >= 8) {
            try {
                int year = stoi(data[4]);
                int pages = stoi(data[5]);
                int copies = stoi(data[7]);
                lib.addBook(data[0], data[1], data[2], data[3], year, "Vietnamese", pages, data[6], "Imported", copies);
            } catch (...) {}
        }
    }
    inFile.close();
}

void loadUsersFromFile(LibrarySystem& lib) {
    ifstream inFile("users.txt");
    if (!inFile.is_open()) return;
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        vector<string> data = split(line, '|');
        if (data.size() >= 9) {
            try {
                int genderInt = stoi(data[2]);
                int prefInt = stoi(data[7]);
                int role = stoi(data[8]);
                
                Gender gender = (genderInt == 1) ? Gender::Male : (genderInt == 2 ? Gender::Female : Gender::Other);
                NotificationPreference pref = (prefInt == 2) ? NotificationPreference::PostalMail : NotificationPreference::Email;
                
                lib.registerMember(data[0], data[1], gender, data[3], data[4], data[5], data[6], pref);
                globalUserRoles[data[5]] = role; 
            } catch (...) {}
        }
    }
    inFile.close();
}

void showBookList(const LibrarySystem& lib, const vector<int>& bookIds) {
    const auto& books = lib.getBooks();
    cout << "\n--- KET QUA TIM KIEM ---\n";
    for (int id : bookIds) {
        for (const auto& b : books) {
            if (b.getId() == id) {
                int available = lib.countAvailableCopies(id);
                cout << "[ID: " << b.getId() << "] [ISBN: " << b.getIsbn() << "] " 
                     << b.getTitle() << " - " << b.getAuthor() 
                     << " (Con lai: " << available << ")\n";
            }
        }
    }
}

void searchBooksFlow(LibrarySystem& lib) {
    string keyword;
    cout << "Nhap tu khoa: ";
    getline(cin, keyword);
    vector<int> ids = lib.searchBooks(keyword, "", "", 0);
    if (ids.empty()) cout << "Khong tim thay sach.\n";
    else showBookList(lib, ids);
}

void createUserFlow(LibrarySystem& lib, int roleToCreate) {
    string roleName = (roleToCreate == ROLE_ADMIN) ? "ADMIN" : 
                      (roleToCreate == ROLE_LIBRARIAN) ? "THU THU" : "THANH VIEN";
                      
    cout << "\n--- TAO TAI KHOAN MOI (" << roleName << ") ---\n";
    string name, dob, email, pass, addr, phone;
    int genderChoice;
    
    cout << "Ho ten: "; getline(cin, name);
    cout << "Ngay sinh (dd/mm/yyyy): "; getline(cin, dob);
    cout << "Gioi tinh (1. Nam, 2. Nu, 3. Khac): "; cin >> genderChoice; clearInput();
    cout << "Dia chi: "; getline(cin, addr);
    cout << "Dien thoai: "; getline(cin, phone);
    cout << "Email: "; getline(cin, email);
    cout << "Mat khau: "; getline(cin, pass);

    Gender g = Gender::Other;
    if (genderChoice == 1) g = Gender::Male;
    else if (genderChoice == 2) g = Gender::Female;

    MemberAccount* newMem = lib.registerMember(name, dob, g, addr, phone, email, pass, NotificationPreference::Email);
    
    if (newMem != nullptr) {
        saveUserToFile(name, dob, genderChoice, addr, phone, email, pass, 1, roleToCreate);
        cout << ">> Tao tai khoan " << roleName << " thanh cong!\n";
    } else {
        cout << ">> Tao that bai (Email da ton tai).\n";
    }
}

void displayCurrentUserInfo(MemberAccount* member) {
    cout << "\n----------------------------------------\n";
    cout << "          THONG TIN TAI KHOAN           \n";
    cout << "----------------------------------------\n";
    int role = globalUserRoles[member->getEmail()];
    string roleStr = (role == ROLE_ADMIN) ? "Quan Tri Vien (Admin)" : 
                     (role == ROLE_LIBRARIAN ? "Thu Thu (Librarian)" : "Thanh Vien (Member)");

    cout << "Ho va ten:    " << member->getName() << "\n";
    cout << "Email:        " << member->getEmail() << "\n";
    cout << "So dien thoai:" << member->getPhone() << "\n";
    cout << "Dia chi:      " << member->getAddress() << "\n";
    cout << "Vai tro:      " << roleStr << "\n";
    cout << "So the TV:    " << member->getCard().cardNumber << "\n";
    cout << "Ngay cap the: " << member->getCard().issuedDate << "\n";
    cout << "Trang thai:   " << (member->getCard().active ? "Dang hoat dong" : "Bi khoa") << "\n";
    cout << "----------------------------------------\n";
}

void runAdminMode(LibrarySystem& lib, MemberAccount* admin) {
    bool running = true;
    while (running) {
        cout << "\n=======================================\n";
        cout << "             QUAN TRI (ADMIN)      \n";
        cout << "=======================================\n";
        cout << "1. Quan ly sach \n";
        cout << "2. Quan ly tai khoan (Them Thu thu/Admin)\n";
        cout << "3. Xoa tai khoan\n";
        cout << "4. Thong tin tai khoan\n";
        cout << "0. DANG XUAT\n";
        cout << "Chon: ";
        
        int choice; cin >> choice; clearInput();
        if (choice == 0) running = false;
        else if (choice == 1) {
            string isbn, title, author, rack, subject; int year, copies, pages;
            cout << "\n--- THEM SACH MOI ---\n";
            cout << "ISBN: "; getline(cin, isbn);
            cout << "Tieu de: "; getline(cin, title);
            cout << "Tac gia: "; getline(cin, author);
            cout << "Chu de: "; getline(cin, subject);
            cout << "Nam XB: "; cin >> year;
            cout << "So trang: "; cin >> pages;
            cout << "So luong ban sao: "; cin >> copies;
            clearInput();
            cout << "Ke sach: "; getline(cin, rack);
            
            lib.addBook(isbn, title, author, subject, year, "Vietnamese", pages, rack, "Added by Admin", copies);
            saveBookToFile(isbn, title, author, subject, year, pages, rack, copies);
            cout << ">> Da them sach!\n";
        }
        else if (choice == 2) {
            cout << "\n--- THEM TAI KHOAN ---\n";
            cout << "1. Them Librarian\n";
            cout << "2. Them Admin khac\n";
            cout << "3. Them Thanh vien thuong\n";
            cout << "0. Quay lai\n";
            cout << "Chon loai tai khoan: ";
            int roleC; cin >> roleC; clearInput();
            
            if (roleC == 1) createUserFlow(lib, ROLE_LIBRARIAN);
            else if (roleC == 2) createUserFlow(lib, ROLE_ADMIN);
            else if (roleC == 3) createUserFlow(lib, ROLE_MEMBER);
        }

        else if (choice == 3) {
            cout << "\n--- XOA TAI KHOAN ---\n";
            string emailDel;
            cout << "Nhap Email tai khoan can xoa: "; getline(cin, emailDel);

            if (emailDel == admin->getEmail()) {
                cout << ">> LOI: Khong the tu xoa tai khoan dang su dung!\n";
            } else if (globalUserRoles.find(emailDel) == globalUserRoles.end()) {
                cout << ">> LOI: Email khong ton tai trong he thong.\n";
            } else {
                cout << "Xac nhan xoa user '" << emailDel << "'? (y/n): ";
                char confirm; cin >> confirm; clearInput();
                if (confirm == 'y' || confirm == 'Y') {
                    deleteUserFromFile(emailDel);
                    globalUserRoles.erase(emailDel);
                    cout << ">> Da xoa tai khoan thanh cong! (Khoi dong lai de cap nhat bo nho).\n";
                } else {
                    cout << ">> Da huy thao tac.\n";
                }
            }
        }
        else if (choice == 4) {
            displayCurrentUserInfo(admin);
        }
    }
}

void runLibrarianMode(LibrarySystem& lib, MemberAccount* librarian) {
    bool running = true;
    while (running) {
        cout << "\n=======================================\n";
        cout << "             THU THU (LIBRARIAN)   \n";
        cout << "=======================================\n";
        cout << "1. Quan ly sach (Xem/Them/Xoa)\n";
        cout << "2. Quan ly phieu muon\n";
        cout << "3. Thong tin tai khoan\n"; 
        cout << "0. DANG XUAT\n";
        cout << "Chon: ";
        int choice; cin >> choice; clearInput();
        if (choice == 0) running = false;
        else if (choice == 1) {
            bool bookRunning = true;
            while(bookRunning) {
                cout << "\n--- QUAN LY SACH ---\n";
                cout << "1. Xem toan bo danh sach sach\n";
                cout << "2. Them sach moi\n";
                cout << "3. Xoa sach\n";
                cout << "0. Quay lai\n";
                cout << "Chon: ";
                int bChoice; cin >> bChoice; clearInput();
                if(bChoice == 0) bookRunning = false;
                else if(bChoice == 1) {
                    const auto& allBooks = lib.getBooks();
                    if(allBooks.empty()) cout << "Thu vien chua co sach.\n";
                    else {
                        cout << "\nDANH SACH TOAN BO SACH:\n";
                        for(const auto& b : allBooks) {
                            int total = 0;
                            for(const auto& c : lib.getCopies()) if(c.getBookId() == b.getId()) total++;
                            cout << "ID: " << b.getId() << " | ISBN: " << b.getIsbn() << " | " << b.getTitle() 
                                 << " | Tac gia: " << b.getAuthor() << " | Tong so ban: " << total << "\n";
                        }
                    }
                }
                else if(bChoice == 2) {
                    string isbn, title, author, rack, subject; int year, copies, pages;
                    cout << "\n--- THEM SACH MOI ---\n";
                    cout << "ISBN: "; getline(cin, isbn);
                    cout << "Tieu de: "; getline(cin, title);
                    cout << "Tac gia: "; getline(cin, author);
                    cout << "Chu de: "; getline(cin, subject);
                    cout << "Nam XB: "; cin >> year;
                    cout << "So trang: "; cin >> pages;
                    cout << "So luong ban sao: "; cin >> copies;
                    clearInput();
                    cout << "Ke sach: "; getline(cin, rack);
                    
                    lib.addBook(isbn, title, author, subject, year, "Vietnamese", pages, rack, "Added by Librarian", copies);
                    saveBookToFile(isbn, title, author, subject, year, pages, rack, copies);
                    cout << ">> Da them sach thanh cong!\n";
                }
                else if(bChoice == 3) {
                    int bookId;
                    cout << "Nhap ID sach can xoa: "; cin >> bookId; clearInput();
                    if(lib.removeBook(bookId)) {
                        updateBookFile(lib.getBooks(), lib);
                        cout << ">> Xoa sach thanh cong va da cap nhat file du lieu.\n";
                    } else {
                        cout << ">> Khong the xoa (Sach khong ton tai hoac dang duoc muon).\n";
                    }
                }
            }
        } else if (choice == 2) {
            cout << "\n--- DANH SACH PHIEU MUON ---\n";
            for (const auto& loan : lib.getLoans()) {
                 cout << "Loan #" << loan.getId() << " | MemberID: " << loan.getMemberId() 
                      << " | Status: " << (loan.getStatus() == LoanStatus::Active ? "Active" : "Returned") << "\n";
            }
        } else if (choice == 3) {
            displayCurrentUserInfo(librarian);
        }
    }
}

void runMemberMode(LibrarySystem& lib, MemberAccount* member) {
    bool running = true;
    while (running) {
        cout << "\n=======================================\n";
        cout << "            THANH VIEN (MEMBER)   \n";
        cout << "=======================================\n";
        cout << "1. Tim kiem sach\n";
        cout << "2. Muon sach (Nhap ISBN)\n";
        cout << "3. Tra sach\n";
        cout << "4. Thong tin tai khoan\n";
        cout << "0. DANG XUAT\n";
        cout << "Chon: ";

        int choice; cin >> choice; clearInput();
        if (choice == 0) running = false;
        else if (choice == 1) searchBooksFlow(lib);
        else if (choice == 2) {
            cout << "\n--- MUON SACH ---\n";
            cout << "Nhap ISBN sach muon muon: ";
            string isbn;
            getline(cin, isbn);

            int targetBookId = -1;
            string bookTitle = "";
            for (const auto& b : lib.getBooks()) {
                if (b.getIsbn() == isbn) {
                    targetBookId = b.getId();
                    bookTitle = b.getTitle();
                    break;
                }
            }

            if (targetBookId == -1) {
                cout << ">> Khong tim thay sach voi ISBN: " << isbn << "\n";
            } else {
                int availableCopyId = -1;
                for (const auto& copy : lib.getCopies()) {
                    if (copy.getBookId() == targetBookId && copy.isAvailable()) {
                        availableCopyId = copy.getId();
                        break;
                    }
                }

                if (availableCopyId != -1) {
                    if(lib.borrowBooks(member->getId(), {availableCopyId}, 1)) {
                        cout << ">> Muon thanh cong cuon: " << bookTitle << "\n";
                    } else {
                        cout << ">> Loi he thong khi muon.\n";
                    }
                } else {
                    cout << ">> Sach '" << bookTitle << "' hien tai da het (tat ca ban sao dang duoc muon).\n";
                }
            }
            
        } else if (choice == 3) {
            cout << "Nhap LoanID de tra: "; int lid; cin >> lid; clearInput();
            lib.returnLoan(lid, 1);
        } else if (choice == 4) {
            displayCurrentUserInfo(member);
        }
    }
}

int main() {
    LibrarySystem lib;
    
    loadBooksFromFile(lib);
    loadUsersFromFile(lib);

    if (lib.findMemberByEmail("admin") == nullptr) {
        lib.registerMember("System Administrator", "01/01/1990", Gender::Other, "Server", "0000", "admin", "123456", NotificationPreference::Email);
        saveUserToFile("System Administrator", "01/01/1990", 3, "Server", "0000", "admin", "123456", 1, ROLE_ADMIN);
    }

    while (true) {
        cout << "\n=======================================\n";
        cout << "   HE THONG QUAN LY THU VIEN (GUEST)   \n";
        cout << "=======================================\n";
        cout << "1. Tra cuu sach\n";
        cout << "2. Dang ky thanh vien (Khach)\n";
        cout << "3. Dang nhap\n";
        cout << "0. Thoat\n";
        cout << "Chon: ";

        int choice; cin >> choice; clearInput();

        if (choice == 0) break;
        else if (choice == 1) searchBooksFlow(lib);
        else if (choice == 2) {
            createUserFlow(lib, ROLE_MEMBER);
        }
        else if (choice == 3) {
            cout << "\n--- DANG NHAP ---\n";
            string email, pass;
            cout << "Email: "; getline(cin, email);
            cout << "Mat khau: "; getline(cin, pass);

            MemberAccount* user = lib.login(email, pass);

            if (user == nullptr) {
                cout << ">> Dang nhap that bai!\n";
            } else {
                int role = globalUserRoles[email];
                
                if (role == ROLE_ADMIN) {
                    runAdminMode(lib, user);
                } else if (role == ROLE_LIBRARIAN) {
                    runLibrarianMode(lib, user);
                } else {
                    runMemberMode(lib, user);
                }
            }
        }
    }

    return 0;
}