# Advanced Security System (C++)

A console-based authentication system built using C++ with file handling, password hashing, and activity logging.

##  Features

- User Registration
- Secure Login System
- Change Password
- Forgot Password (Security Question)
- Account Lock after 3 Failed Attempts
- Unlock User
- Search User
- Delete User
- Activity Logging

## Concepts Used

- Authentication System
- Password Hashing
- File Handling
- Data Structures (Vector, Struct)
- Input Validation
- Logging System
- Basic System Design

## How to Run

```bash
g++ src/main.cpp -o system
./system





## How It Works

- **User data is stored in a file (`users.txt`)**
- **Passwords are stored as hashes (not plain text)**
- **Login compares hashed input with stored hash**
- **System tracks failed login attempts**
- **Account gets locked after multiple failures**
- **Logs are stored in `activity_log.txt` with timestamps**



## Future Improvements

- **GUI version (Qt / Web Interface)**
- **Database integration (MySQL / MongoDB)**
- **Strong password hashing (bcrypt / Argon2)**
- **Hidden password input**
- **Admin authentication system**



## Author

**Piyush Kumar Verma  
**CSE Undergrad Student | Aspiring Software Engineer 


 
