#include "Library.h"

#include <algorithm>
#include <functional>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;


namespace {
    string simpleHash(const string& input) {
        std::hash<string> hasher;
        return std::to_string(hasher(input));
    }
}


MemberAccount::MemberAccount(
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
)
    : id(id),
      fullName(fullName),
      dateOfBirth(dob),
      gender(gender),
      address(address),
      phone(phone),
      email(email),
      passwordHash(simpleHash(rawPassword)),
      preference(pref),
      card(card) {
}

bool MemberAccount::checkPassword(const string& rawPassword) const {
    return passwordHash == simpleHash(rawPassword);
}

void MemberAccount::changePassword(const string& newRawPassword) {
    passwordHash = simpleHash(newRawPassword);
}

void MemberAccount::updateProfile(const string& newName, const string& newAddress, const string& newPhone) {
    fullName = newName;
    address = newAddress;
    phone = newPhone;
}


Book::Book(int id,
           const string& isbn,
           const string& title,
           const string& author,
           const string& subject,
           int publicationYear,
           const string& language,
           int pages,
           const string& rackPosition,
           const string& description)
    : id(id),
      isbn(isbn),
      title(title),
      author(author),
      subject(subject),
      publicationYear(publicationYear),
      language(language),
      pages(pages),
      rackPosition(rackPosition),
      description(description) {
}

void Book::updateInfo(const string& newTitle,
                      const string& newAuthor,
                      const string& newSubject,
                      int newPublicationYear,
                      const string& newLanguage,
                      int newPages,
                      const string& newRackPosition,
                      const string& newDescription) {
    title = newTitle;
    author = newAuthor;
    subject = newSubject;
    publicationYear = newPublicationYear;
    language = newLanguage;
    pages = newPages;
    rackPosition = newRackPosition;
    description = newDescription;
}


BookItem::BookItem(int id, int bookId, const string& barcode, bool available, const string& location)
    : id(id),
      bookId(bookId),
      barcode(barcode),
      available(available),
      location(location) {
}


Loan::Loan(int id,
           int memberId,
           const vector<int>& bookItemIds,
           int borrowDate,
           int dueDate)
    : id(id),
      memberId(memberId),
      bookItemIds(bookItemIds),
      borrowDate(borrowDate),
      dueDate(dueDate),
      returnDate(0),
      renewalCount(0),
      status(LoanStatus::Active),
      fine(0.0) {
}

void Loan::markReturned(int actualReturnDate, double finePerDay) {
    returnDate = actualReturnDate;
    if (actualReturnDate > dueDate) {
        int overdueDays = actualReturnDate - dueDate;
        fine = overdueDays * finePerDay;
        status = LoanStatus::Overdue;
    } else {
        status = LoanStatus::Returned;
        fine = 0.0;
    }
}

bool Loan::canRenew(int maxRenewals) const {
    return status == LoanStatus::Active && renewalCount < maxRenewals;
}

void Loan::renew(int extraDays) {
    if (status == LoanStatus::Active) {
        dueDate += extraDays;
        ++renewalCount;
    }
}

void Loan::markOverdue(double finePerDay, int today) {
    if (status == LoanStatus::Active && today > dueDate) {
        int overdueDays = today - dueDate;
        fine = overdueDays * finePerDay;
        status = LoanStatus::Overdue;
    }
}


Reservation::Reservation(int id, int memberId, int bookId)
    : id(id),
      memberId(memberId),
      bookId(bookId),
      active(true) {
}


LibrarySystem::LibrarySystem() {

}

MemberAccount* LibrarySystem::registerMember(
    const string& fullName,
    const string& dob,
    Gender gender,
    const string& address,
    const string& phone,
    const string& email,
    const string& password,
    NotificationPreference pref) {

    if (findMemberByEmail(email) != nullptr) {
        cout << "Email da ton tai trong he thong.\n";
        return nullptr;
    }

    if (password.size() < 6) {
        cout << "Mat khau phai co it nhat 6 ky tu.\n";
        return nullptr;
    }

    LibraryCard card;
    card.cardNumber = "CARD-" + std::to_string(nextMemberId);
    card.issuedDate = "Today";
    card.active = true;

    members.emplace_back(nextMemberId++,
        fullName, dob, gender, address, phone, email, password, pref, card);

    cout << "Dang ky thanh cong. So the thu vien: " << card.cardNumber << "\n";
    return &members.back();
}

MemberAccount* LibrarySystem::findMemberByEmail(const string& email) {
    for (auto& m : members) {
        if (m.getEmail() == email) return &m;
    }
    return nullptr;
}

const MemberAccount* LibrarySystem::findMemberByEmail(const string& email) const {
    for (const auto& m : members) {
        if (m.getEmail() == email) return &m;
    }
    return nullptr;
}

MemberAccount* LibrarySystem::login(const string& email, const string& password) {
    MemberAccount* m = findMemberByEmail(email);
    if (!m) {

        return nullptr;
    }
    if (!m->checkPassword(password)) {
        // cout << "Sai mat khau.\n";
        return nullptr;
    }
    // Giao diện main sẽ in thông báo chào mừng
    return m;
}

void LibrarySystem::forgotPassword(const string& email, const string& newPassword) {
    MemberAccount* m = findMemberByEmail(email);
    if (!m) {
        cout << "This email is not registered in the system.\n";
        return;
    }
    cout << "Gui ma xac thuc / lien ket reset password toi " << email << "...\n";
    m->changePassword(newPassword);
    cout << "Mat khau da duoc cap nhat.\n";
}

Book* LibrarySystem::addBook(const string& isbn,
                             const string& title,
                             const string& author,
                             const string& subject,
                             int publicationYear,
                             const string& language,
                             int pages,
                             const string& rackPosition,
                             const string& description,
                             int numCopies) {
    int bookId = nextBookId++;
    books.emplace_back(bookId, isbn, title, author, subject,
                       publicationYear, language, pages, rackPosition, description);

    for (int i = 0; i < numCopies; ++i) {
        string barcode = "BC-" + std::to_string(bookId) + "-" + std::to_string(i + 1);
        copies.emplace_back(nextCopyId++, bookId, barcode, true, rackPosition);
    }

    return &books.back();
}

bool LibrarySystem::editBook(int bookId,
                             const string& title,
                             const string& author,
                             const string& subject,
                             int publicationYear,
                             const string& language,
                             int pages,
                             const string& rackPosition,
                             const string& description) {
    Book* b = findBookById(bookId);
    if (!b) return false;
    b->updateInfo(title, author, subject, publicationYear, language, pages, rackPosition, description);
    for (auto& c : copies) {
        if (c.getBookId() == bookId) {
            
            c = BookItem(c.getId(), c.getBookId(), c.getBarcode(), c.isAvailable(), rackPosition);
        }
    }
    return true;
}

bool LibrarySystem::removeBook(int bookId) {
 
    for (const auto& loan : loans) {
        for (int copyId : loan.getBookItemIds()) {
            const BookItem* copy = findCopyById(copyId);
            if (copy && copy->getBookId() == bookId && loan.getStatus() == LoanStatus::Active) {
                cout << "Khong the xoa sach dang duoc muon.\n";
                return false;
            }
        }
    }

    books.erase(std::remove_if(books.begin(), books.end(),
        [bookId](const Book& b) { return b.getId() == bookId; }),
        books.end());

    copies.erase(std::remove_if(copies.begin(), copies.end(),
        [bookId](const BookItem& c) { return c.getBookId() == bookId; }),
        copies.end());

    return true;
}

vector<int> LibrarySystem::searchBooks(const string& keyword,
                                       const string& author,
                                       const string& subject,
                                       int year) const {
    vector<int> resultIds;
    for (const auto& b : books) {
        bool match = true;
        if (!keyword.empty()) {
            string lowerKey = keyword;
            std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
            string combined = b.getTitle() + " " + b.getDescription();
            std::transform(combined.begin(), combined.end(), combined.begin(), ::tolower);
            if (combined.find(lowerKey) == string::npos) match = false;
        }
        if (match && !author.empty() && b.getAuthor().find(author) == string::npos) {
            match = false;
        }
        if (match && !subject.empty() && b.getSubject().find(subject) == string::npos) {
            match = false;
        }
        if (match && year != 0 && b.getPublicationYear() != year) {
            match = false;
        }
        if (match) {
            resultIds.push_back(b.getId());
        }
    }
    return resultIds;
}

int LibrarySystem::countAvailableCopies(int bookId) const {
    int count = 0;
    for (const auto& c : copies) {
        if (c.getBookId() == bookId && c.isAvailable()) {
            ++count;
        }
    }
    return count;
}

Loan* LibrarySystem::borrowBooks(int memberId, const vector<int>& bookItemIds, int today) {

    int currentBorrowed = 0;
    for (const auto& loan : loans) {
        if (loan.getMemberId() == memberId && loan.getStatus() == LoanStatus::Active) {
            currentBorrowed += static_cast<int>(loan.getBookItemIds().size());
        }
    }
    if (currentBorrowed + static_cast<int>(bookItemIds.size()) > maxBorrowedBooks) {
        cout << "Vuot qua gioi han muon sach (" << maxBorrowedBooks << ").\n";
        return nullptr;
    }

    for (int copyId : bookItemIds) {
        BookItem* copy = findCopyById(copyId);
        if (!copy || !copy->isAvailable()) {
            cout << "Ban sao sach co ID " << copyId << " khong san sang de muon.\n";
            return nullptr;
        }
    }

    int loanId = nextLoanId++;
    int due = today + 14;
    loans.emplace_back(loanId, memberId, bookItemIds, today, due);

    for (int copyId : bookItemIds) {
        BookItem* copy = findCopyById(copyId);
        if (copy) copy->setAvailable(false);
    }

    cout << "Tao phieu muon #" << loanId << " thanh cong.\n";
    return &loans.back();
}

bool LibrarySystem::returnLoan(int loanId, int actualReturnDate) {
    for (auto& loan : loans) {
        if (loan.getId() == loanId && loan.getStatus() == LoanStatus::Active) {
            loan.markReturned(actualReturnDate, finePerDay);
            for (int copyId : loan.getBookItemIds()) {
                BookItem* copy = findCopyById(copyId);
                if (copy) copy->setAvailable(true);
            }
            cout << "Cap nhat tra sach cho phieu muon #" << loanId
                 << ". Tien phat: " << loan.getFine() << "\n";
            return true;
        }
    }
    cout << "Khong tim thay phieu muon hop le.\n";
    return false;
}

bool LibrarySystem::renewLoan(int loanId, int extraDays) {
    for (auto& loan : loans) {
        if (loan.getId() == loanId) {
            if (!loan.canRenew(maxRenewals)) {
                cout << "Khong the gia han phieu muon #" << loanId << " (vuot qua so lan cho phep hoac khong con hieu luc).\n";
                return false;
            }
            loan.renew(extraDays);
            cout << "Da gia han phieu muon #" << loanId << " den ngay " << loan.getDueDate() << "\n";
            return true;
        }
    }
    cout << "Khong tim thay phieu muon #" << loanId << "\n";
    return false;
}

void LibrarySystem::updateOverdueAndSendReminders(int today) const {
    cout << "=== Notifications & Reminders ===\n";
    for (const auto& loan : loans) {
        if (loan.getStatus() == LoanStatus::Active) {
            int daysToDue = loan.getDueDate() - today;
            if (daysToDue == 2) {
                cout << "Nhac nho: Phieu muon #" << loan.getId()
                     << " sap den han (con 2 ngay).\n";
            } else if (daysToDue < 0) {
                int overdueDays = -daysToDue;
                cout << "Qua han: Phieu muon #" << loan.getId()
                     << " da qua han " << overdueDays << " ngay.\n";
            }
        }
    }
}

Book* LibrarySystem::findBookById(int bookId) {
    for (auto& b : books) {
        if (b.getId() == bookId) return &b;
    }
    return nullptr;
}

const Book* LibrarySystem::findBookById(int bookId) const {
    for (const auto& b : books) {
        if (b.getId() == bookId) return &b;
    }
    return nullptr;
}

BookItem* LibrarySystem::findCopyById(int copyId) {
    for (auto& c : copies) {
        if (c.getId() == copyId) return &c;
    }
    return nullptr;
}

const BookItem* LibrarySystem::findCopyById(int copyId) const {
    for (const auto& c : copies) {
        if (c.getId() == copyId) return &c;
    }
    return nullptr;
}