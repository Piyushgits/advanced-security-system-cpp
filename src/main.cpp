#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <limits>
#include <cctype>
using namespace std;

struct User {
    string username;
    string passwordHash;
    int age;
    int failedAttempts;
    int isLocked;
    string securityQuestion;
    string securityAnswerHash;
};

const string USER_FILE = "users.txt";
const string LOG_FILE = "activity_log.txt";
const int MAX_FAILED_ATTEMPTS = 3;

string trim(const string &s) {
    size_t start = 0, end = s.size();
    while (start < s.size() && isspace((unsigned char)s[start])) start++;
    while (end > start && isspace((unsigned char)s[end - 1])) end--;
    return s.substr(start, end - start);
}

string getCurrentTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << setfill('0')
       << 1900 + ltm->tm_year << '-'
       << setw(2) << 1 + ltm->tm_mon << '-'
       << setw(2) << ltm->tm_mday << ' '
       << setw(2) << ltm->tm_hour << ':'
       << setw(2) << ltm->tm_min << ':'
       << setw(2) << ltm->tm_sec;
    return ss.str();
}

void logActivity(const string &msg) {
    ofstream fout(LOG_FILE, ios::app);
    if (fout.is_open()) {
        fout << '[' << getCurrentTime() << "] " << msg << "\n";
        fout.close();
    }
}

string simpleHash(const string &s) {
    unsigned long long h = 1469598103934665603ULL;
    for (char c : s) {
        h ^= (unsigned long long)(unsigned char)c;
        h *= 1099511628211ULL;
        h += 31;
        h ^= (h >> 13);
    }
    stringstream ss;
    ss << hex << h;
    return ss.str();
}

bool isStrongPassword(const string &pass) {
    if (pass.size() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char c : pass) {
        if (isupper((unsigned char)c)) hasUpper = true;
        else if (islower((unsigned char)c)) hasLower = true;
        else if (isdigit((unsigned char)c)) hasDigit = true;
        else hasSpecial = true;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

bool usernameExists(const string &username) {
    ifstream fin(USER_FILE);
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string token;
        getline(ss, token, '|');
        if (token == username) return true;
    }
    return false;
}

vector<User> loadUsers() {
    vector<User> users;
    ifstream fin(USER_FILE);
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        User u;
        string ageStr, failedStr, lockedStr;
        getline(ss, u.username, '|');
        getline(ss, u.passwordHash, '|');
        getline(ss, ageStr, '|');
        getline(ss, failedStr, '|');
        getline(ss, lockedStr, '|');
        getline(ss, u.securityQuestion, '|');
        getline(ss, u.securityAnswerHash, '|');
        u.age = ageStr.empty() ? 0 : stoi(ageStr);
        u.failedAttempts = failedStr.empty() ? 0 : stoi(failedStr);
        u.isLocked = lockedStr.empty() ? 0 : stoi(lockedStr);
        users.push_back(u);
    }
    fin.close();
    return users;
}

void saveAllUsers(const vector<User> &users) {
    ofstream fout(USER_FILE);
    for (const User &u : users) {
        fout << u.username << '|'
             << u.passwordHash << '|'
             << u.age << '|'
             << u.failedAttempts << '|'
             << u.isLocked << '|'
             << u.securityQuestion << '|'
             << u.securityAnswerHash << "\n";
    }
    fout.close();
}

int findUserIndex(const vector<User> &users, const string &username) {
    for (int i = 0; i < (int)users.size(); i++) {
        if (users[i].username == username) return i;
    }
    return -1;
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void printLine() {
    cout << "============================================================\n";
}

void registerUser() {
    string username, password, confirmPassword, question, answer;
    int age;

    printLine();
    cout << "                    REGISTER NEW USER\n";
    printLine();

    cout << "Enter username: ";
    cin >> username;

    if (usernameExists(username)) {
        cout << "Username already exists. Try another.\n";
        logActivity("Register failed: duplicate username -> " + username);
        return;
    }

    cout << "Enter age: ";
    cin >> age;
    if (age <= 0 || age > 120) {
        cout << "Invalid age.\n";
        logActivity("Register failed: invalid age for -> " + username);
        return;
    }

    cout << "Enter password: ";
    cin >> password;
    cout << "Confirm password: ";
    cin >> confirmPassword;

    if (password != confirmPassword) {
        cout << "Passwords do not match.\n";
        logActivity("Register failed: password mismatch -> " + username);
        return;
    }

    if (!isStrongPassword(password)) {
        cout << "Weak password. Use at least 8 chars with Uppercase, Lowercase, Digit and Special char.\n";
        logActivity("Register failed: weak password -> " + username);
        return;
    }

    clearInput();
    cout << "Enter security question (example: Your favorite color?): ";
    getline(cin, question);
    cout << "Enter security answer: ";
    getline(cin, answer);

    User u;
    u.username = trim(username);
    u.passwordHash = simpleHash(password);
    u.age = age;
    u.failedAttempts = 0;
    u.isLocked = 0;
    u.securityQuestion = trim(question);
    u.securityAnswerHash = simpleHash(trim(answer));

    ofstream fout(USER_FILE, ios::app);
    if (!fout.is_open()) {
        cout << "Error opening user file.\n";
        return;
    }

    fout << u.username << '|'
         << u.passwordHash << '|'
         << u.age << '|'
         << u.failedAttempts << '|'
         << u.isLocked << '|'
         << u.securityQuestion << '|'
         << u.securityAnswerHash << "\n";
    fout.close();

    cout << "Registration successful.\n";
    logActivity("User registered successfully -> " + username);
}

void loginUser() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    vector<User> users = loadUsers();
    int idx = findUserIndex(users, username);

    if (idx == -1) {
        cout << "User not found.\n";
        logActivity("Login failed: user not found -> " + username);
        return;
    }

    if (users[idx].isLocked) {
        cout << "Account is locked. Please reset password or unlock account.\n";
        logActivity("Login blocked: locked account -> " + username);
        return;
    }

    if (users[idx].passwordHash == simpleHash(password)) {
        users[idx].failedAttempts = 0;
        saveAllUsers(users);
        cout << "Login successful. Welcome, " << username << "!\n";
        logActivity("Login success -> " + username);
    } else {
        users[idx].failedAttempts++;
        if (users[idx].failedAttempts >= MAX_FAILED_ATTEMPTS) {
            users[idx].isLocked = 1;
            cout << "Too many failed attempts. Account locked.\n";
            logActivity("Account locked after failed attempts -> " + username);
        } else {
            cout << "Wrong password. Attempts left: "
                 << (MAX_FAILED_ATTEMPTS - users[idx].failedAttempts) << "\n";
            logActivity("Login failed: wrong password -> " + username);
        }
        saveAllUsers(users);
    }
}

void changePassword() {
    string username, oldPassword, newPassword, confirmPassword;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter old password: ";
    cin >> oldPassword;

    vector<User> users = loadUsers();
    int idx = findUserIndex(users, username);

    if (idx == -1) {
        cout << "User not found.\n";
        return;
    }

    if (users[idx].isLocked) {
        cout << "Account is locked. Use Forgot Password first.\n";
        return;
    }

    if (users[idx].passwordHash != simpleHash(oldPassword)) {
        cout << "Old password is incorrect.\n";
        logActivity("Password change failed: wrong old password -> " + username);
        return;
    }

    cout << "Enter new password: ";
    cin >> newPassword;
    cout << "Confirm new password: ";
    cin >> confirmPassword;

    if (newPassword != confirmPassword) {
        cout << "Passwords do not match.\n";
        return;
    }

    if (!isStrongPassword(newPassword)) {
        cout << "Weak password. Use at least 8 chars with Uppercase, Lowercase, Digit and Special char.\n";
        return;
    }

    users[idx].passwordHash = simpleHash(newPassword);
    users[idx].failedAttempts = 0;
    saveAllUsers(users);
    cout << "Password changed successfully.\n";
    logActivity("Password changed -> " + username);
}

void forgotPassword() {
    string username, answer, newPassword, confirmPassword;
    cout << "Enter username: ";
    cin >> username;

    vector<User> users = loadUsers();
    int idx = findUserIndex(users, username);

    if (idx == -1) {
        cout << "User not found.\n";
        return;
    }

    clearInput();
    cout << "Security question: " << users[idx].securityQuestion << "\n";
    cout << "Enter answer: ";
    getline(cin, answer);

    if (simpleHash(trim(answer)) != users[idx].securityAnswerHash) {
        cout << "Wrong security answer.\n";
        logActivity("Forgot password failed: wrong answer -> " + username);
        return;
    }

    cout << "Enter new password: ";
    cin >> newPassword;
    cout << "Confirm new password: ";
    cin >> confirmPassword;

    if (newPassword != confirmPassword) {
        cout << "Passwords do not match.\n";
        return;
    }

    if (!isStrongPassword(newPassword)) {
        cout << "Weak password. Use at least 8 chars with Uppercase, Lowercase, Digit and Special char.\n";
        return;
    }

    users[idx].passwordHash = simpleHash(newPassword);
    users[idx].failedAttempts = 0;
    users[idx].isLocked = 0;
    saveAllUsers(users);

    cout << "Password reset successful. Account unlocked.\n";
    logActivity("Forgot password success -> " + username);
}

void searchUser() {
    string username;
    cout << "Enter username to search: ";
    cin >> username;

    vector<User> users = loadUsers();
    int idx = findUserIndex(users, username);

    if (idx == -1) {
        cout << "User not found.\n";
        return;
    }

    printLine();
    cout << "User found\n";
    printLine();
    cout << "Username        : " << users[idx].username << "\n";
    cout << "Age             : " << users[idx].age << "\n";
    cout << "Failed Attempts : " << users[idx].failedAttempts << "\n";
    cout << "Locked Status   : " << (users[idx].isLocked ? "Locked" : "Active") << "\n";
}

void displayAllUsers() {
    vector<User> users = loadUsers();
    if (users.empty()) {
        cout << "No users found.\n";
        return;
    }

    printLine();
    cout << left << setw(20) << "USERNAME"
         << setw(8) << "AGE"
         << setw(16) << "FAILED_ATTEMPTS"
         << setw(12) << "STATUS" << "\n";
    printLine();

    for (const User &u : users) {
        cout << left << setw(20) << u.username
             << setw(8) << u.age
             << setw(16) << u.failedAttempts
             << setw(12) << (u.isLocked ? "Locked" : "Active") << "\n";
    }
}

void deleteUser() {
    string username;
    cout << "Enter username to delete: ";
    cin >> username;

    vector<User> users = loadUsers();
    int idx = findUserIndex(users, username);
    if (idx == -1) {
        cout << "User not found.\n";
        return;
    }

    users.erase(users.begin() + idx);
    saveAllUsers(users);
    cout << "User deleted successfully.\n";
    logActivity("User deleted -> " + username);
}

void unlockUser() {
    string username;
    cout << "Enter username to unlock: ";
    cin >> username;

    vector<User> users = loadUsers();
    int idx = findUserIndex(users, username);
    if (idx == -1) {
        cout << "User not found.\n";
        return;
    }

    users[idx].isLocked = 0;
    users[idx].failedAttempts = 0;
    saveAllUsers(users);
    cout << "User unlocked successfully.\n";
    logActivity("User unlocked -> " + username);
}

void viewLogs() {
    ifstream fin(LOG_FILE);
    if (!fin.is_open()) {
        cout << "No logs available.\n";
        return;
    }

    printLine();
    cout << "                     ACTIVITY LOGS\n";
    printLine();

    string line;
    while (getline(fin, line)) {
        cout << line << "\n";
    }
    fin.close();
}

void showMenu() {
    printLine();
    cout << "              ADVANCED SECURITY SYSTEM PROJECT\n";
    printLine();
    cout << "1. Register User\n";
    cout << "2. Login User\n";
    cout << "3. Change Password\n";
    cout << "4. Forgot Password\n";
    cout << "5. Search User\n";
    cout << "6. Display All Users\n";
    cout << "7. Delete User\n";
    cout << "8. Unlock User\n";
    cout << "9. View Logs\n";
    cout << "10. Exit\n";
    printLine();
}

int main() {
    int choice;

    do {
        showMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            clearInput();
            cout << "Invalid input. Enter a number only.\n";
            continue;
        }

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3:
                changePassword();
                break;
            case 4:
                forgotPassword();
                break;
            case 5:
                searchUser();
                break;
            case 6:
                displayAllUsers();
                break;
            case 7:
                deleteUser();
                break;
            case 8:
                unlockUser();
                break;
            case 9:
                viewLogs();
                break;
            case 10:
                cout << "Exiting program. Goodbye.\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }

        if (choice != 10) {
            clearInput();
            pauseScreen();
        }

    } while (choice != 10);

    return 0;
}
