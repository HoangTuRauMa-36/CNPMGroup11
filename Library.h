
#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;


enum class Gender {
    Male,
    Female,
    Other
};

enum class NotificationPreference {
    Email,
    PostalMail
};

enum class LoanStatus {
    Active,
    Returned,
    Overdue
};

struct LibraryCard {
    string cardNumber;
    string issuedDate;
    bool active{ true };
};


class MemberAccount {
private:
    int id{};
    string fullName;
    string dateOfBirth;
    Gender gender{};
    string address;
    string phone;
    string email;
    string passwordHash;  
    NotificationPreference preference{};
    LibraryCard card;
public:
    MemberAccount() = default;

    MemberAccount(
        int id,
        const string& fullName,
        const string& dob,
        Gender gender,
        const string& address,
        const string& phone,
        const string& email,
        const string& rawPassword,
        NotificationPreference pref,
        const LibraryCard& card
    );

    int getId() const { return id; }
    const string& getName() const { return fullName; }
    const string& getEmail() const { return email; }
    const string& getAddress() const { return address; }
    const string& getPhone() const { return phone; }
    const LibraryCard& getCard() const { return card; }
    NotificationPreference getPreference() const { return preference; }

    bool checkPassword(const string& rawPassword) const;
    void changePassword(const string& newRawPassword);
    void updateProfile(const string& newName, const string& newAddress, const string& newPhone);
};

class Book {
private:
    int id{};
    string isbn;
    string title;
    string author;
    string subject;
    int publicationYear{};
    string language;
    int pages{};
    string rackPosition;
    string description;
public:
    Book() = default;

    Book(int id,
         const string& isbn,
         const string& title,
         const string& author,
         const string& subject,
         int publicationYear,
         const string& language,
         int pages,
         const string& rackPosition,
         const string& description);

    int getId() const { return id; }
    const string& getIsbn() const { return isbn; }
    const string& getTitle() const { return title; }
    const string& getAuthor() const { return author; }
    const string& getSubject() const { return subject; }
    int getPublicationYear() const { return publicationYear; }
    const string& getLanguage() const { return language; }
    int getPages() const { return pages; }
    const string& getRackPosition() const { return rackPosition; }
    const string& getDescription() const { return description; }

    void updateInfo(const string& newTitle,
                    const string& newAuthor,
                    const string& newSubject,
                    int newPublicationYear,
                    const string& newLanguage,
                    int newPages,
                    const string& newRackPosition,
                    const string& newDescription);
};

class BookItem {
private:
    int id{};
    int bookId{};
    string barcode;
    bool available{ true };
    string location;
public:
    BookItem() = default;

    BookItem(int id, int bookId, const string& barcode, bool available, const string& location);

    int getId() const { return id; }
    int getBookId() const { return bookId; }
    const string& getBarcode() const { return barcode; }
    const string& getLocation() const { return location; }
    bool isAvailable() const { return available; }
    void setAvailable(bool value) { available = value; }
};

class Loan {
private:
    int id{};
    int memberId{};
    vector<int> bookItemIds;
    int borrowDate{}; 
    int dueDate{};
    int returnDate{}; 
    int renewalCount{};
    LoanStatus status{ LoanStatus::Active };
    double fine{};
public:
    Loan() = default;

    Loan(int id,
         int memberId,
         const vector<int>& bookItemIds,
         int borrowDate,
         int dueDate);

    int getId() const { return id; }
    int getMemberId() const { return memberId; }
    const vector<int>& getBookItemIds() const { return bookItemIds; }
    int getBorrowDate() const { return borrowDate; }
    int getDueDate() const { return dueDate; }
    int getReturnDate() const { return returnDate; }
    int getRenewalCount() const { return renewalCount; }
    LoanStatus getStatus() const { return status; }
    double getFine() const { return fine; }

    void markReturned(int actualReturnDate, double finePerDay);
    bool canRenew(int maxRenewals) const;
    void renew(int extraDays);
    void markOverdue(double finePerDay, int today);
};

class Reservation {
private:
    int id{};
    int memberId{};
    int bookId{};
    bool active{ true };
public:
    Reservation() = default;

    Reservation(int id, int memberId, int bookId);

    int getId() const { return id; }
    int getMemberId() const { return memberId; }
    int getBookId() const { return bookId; }
    bool isActive() const { return active; }
    void cancel() { active = false; }
};


class LibrarySystem {
private:
    vector<MemberAccount> members;
    vector<Book> books;
    vector<BookItem> copies;
    vector<Loan> loans;
    vector<Reservation> reservations;

    int nextMemberId{ 1 };
    int nextBookId{ 1 };
    int nextCopyId{ 1 };
    int nextLoanId{ 1 };
    int nextReservationId{ 1 };

    int maxBorrowedBooks{ 5 };
    int maxRenewals{ 2 };
    double finePerDay{ 1.0 };

public:
    LibrarySystem();

    MemberAccount* registerMember(
        const string& fullName,
        const string& dob,
        Gender gender,
        const string& address,
        const string& phone,
        const string& email,
        const string& password,
        NotificationPreference pref);

    MemberAccount* findMemberByEmail(const string& email);
    const MemberAccount* findMemberByEmail(const string& email) const;
    MemberAccount* login(const string& email, const string& password);
    void forgotPassword(const string& email, const string& newPassword);

    Book* addBook(const string& isbn,
                  const string& title,
                  const string& author,
                  const string& subject,
                  int publicationYear,
                  const string& language,
                  int pages,
                  const string& rackPosition,
                  const string& description,
                  int numCopies);

    bool editBook(int bookId,
                  const string& title,
                  const string& author,
                  const string& subject,
                  int publicationYear,
                  const string& language,
                  int pages,
                  const string& rackPosition,
                  const string& description);

    bool removeBook(int bookId);

    const vector<Book>& getBooks() const { return books; }
    const vector<BookItem>& getCopies() const { return copies; }
    const vector<Loan>& getLoans() const { return loans; }

    vector<int> searchBooks(const string& keyword,
                            const string& author,
                            const string& subject,
                            int year) const;

    int countAvailableCopies(int bookId) const;

    Loan* borrowBooks(int memberId, const vector<int>& bookItemIds, int today);
    bool returnLoan(int loanId, int actualReturnDate);
    bool renewLoan(int loanId, int extraDays);

    void updateOverdueAndSendReminders(int today) const;


    Book* findBookById(int bookId);
    const Book* findBookById(int bookId) const;
    BookItem* findCopyById(int copyId);
    const BookItem* findCopyById(int copyId) const;
};
