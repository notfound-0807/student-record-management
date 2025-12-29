#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

struct Student {
    int roll;
    string name;  // Changed to string for flexibility
    float marks;
};

// Helper to clear input buffer
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Function to add student with validation
void addStudent(const string& filename) {
    Student s;
    cout << "Enter roll number (positive integer): ";
    while (!(cin >> s.roll) || s.roll <= 0) {
        cout << "Invalid roll. Enter a positive integer: ";
        clearInput();
    }
    clearInput();
    cout << "Enter name: ";
    getline(cin, s.name);
    cout << "Enter marks (0-100): ";
    while (!(cin >> s.marks) || s.marks < 0 || s.marks > 100) {
        cout << "Invalid marks. Enter 0-100: ";
        clearInput();
    }

    // Check for duplicate roll
    ifstream checkFile(filename, ios::binary);
    Student temp;
    bool duplicate = false;
    while (checkFile.read((char*)&temp, sizeof(Student))) {
        if (temp.roll == s.roll) {
            duplicate = true;
            break;
        }
    }
    checkFile.close();
    if (duplicate) {
        cout << "❌ Roll number already exists!\n";
        return;
    }

    ofstream outFile(filename, ios::binary | ios::app);
    if (!outFile) {
        cout << "❌ Error opening file!\n";
        return;
    }
    outFile.write((char*)&s, sizeof(Student));
    outFile.close();
    cout << "✅ Student added successfully!\n";
}

// Function to display students
void displayStudents(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "❌ Error opening file!\n";
        return;
    }
    Student s;
    cout << "\n---Student List---\n";
    cout << left << setw(10) << "Roll"
         << setw(25) << "Name"
         << setw(10) << "Marks" << endl;
    cout << "--------------------------------------\n";

    bool empty = true;
    while (inFile.read((char*)&s, sizeof(Student))) {
        cout << left << setw(10) << s.roll
             << setw(25) << s.name
             << setw(10) << s.marks << endl;
        empty = false;
    }
    if (empty) cout << "No records found!\n";
    inFile.close();
}

// Search student by roll
void searchStudent(const string& filename, int rollNo) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "❌ Error opening file!\n";
        return;
    }
    Student s;
    bool found = false;
    while (inFile.read((char*)&s, sizeof(Student))) {
        if (s.roll == rollNo) {
            cout << "Found Student - Name: " << s.name
                 << ", Marks: " << s.marks << endl;
            found = true;
            break;
        }
    }
    if (!found) cout << "❌ Student not found!\n";
    inFile.close();
}

// Delete student by roll
void deleteStudent(const string& filename, int rollNo) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "❌ Error opening file!\n";
        return;
    }
    ofstream tempFile("temp.dat", ios::binary);
    if (!tempFile) {
        cout << "❌ Error creating temp file!\n";
        inFile.close();
        return;
    }
    Student s;
    bool found = false;
    while (inFile.read((char*)&s, sizeof(Student))) {
        if (s.roll != rollNo) {
            tempFile.write((char*)&s, sizeof(Student));
        } else {
            found = true;
        }
    }
    inFile.close();
    tempFile.close();

    if (remove(filename.c_str()) != 0 || rename("temp.dat", filename.c_str()) != 0) {
        cout << "❌ Error updating file!\n";
        return;
    }

    if (found)
        cout << "✅ Student deleted successfully!\n";
    else
        cout << "❌ Student not found!\n";
}

// Sort students by marks (descending) using std::sort
void sortByMarks(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "❌ Error opening file!\n";
        return;
    }
    vector<Student> students;
    Student s;
    while (inFile.read((char*)&s, sizeof(Student))) {
        students.push_back(s);
    }
    inFile.close();

    sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
        return a.marks > b.marks;  // Descending
    });

    ofstream outFile(filename, ios::binary | ios::trunc);
    if (!outFile) {
        cout << "❌ Error opening file for writing!\n";
        return;
    }
    for (const auto& stu : students) {
        outFile.write((char*)&stu, sizeof(Student));
    }
    outFile.close();
    cout << "✅ Students sorted by marks (highest first)\n";
}

// Sort students by name (alphabetical, case-insensitive)
void sortByName(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "❌ Error opening file!\n";
        return;
    }
    vector<Student> students;
    Student s;
    while (inFile.read((char*)&s, sizeof(Student))) {
        students.push_back(s);
    }
    inFile.close();

    sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
        string nameA = a.name, nameB = b.name;
        transform(nameA.begin(), nameA.end(), nameA.begin(), ::tolower);
        transform(nameB.begin(), nameB.end(), nameB.begin(), ::tolower);
        return nameA < nameB;
    });

    ofstream outFile(filename, ios::binary | ios::trunc);
    if (!outFile) {
        cout << "❌ Error opening file for writing!\n";
        return;
    }
    for (const auto& stu : students) {
        outFile.write((char*)&stu, sizeof(Student));
    }
    outFile.close();
    cout << "✅ Students sorted by name (A-Z, case-insensitive)\n";
}

// Show top scorer(s)
void showTopScorer(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "❌ Error opening file!\n";
        return;
    }
    Student s;
    vector<Student> tops;
    float maxMarks = -1;
    while (inFile.read((char*)&s, sizeof(Student))) {
        if (s.marks > maxMarks) {
            maxMarks = s.marks;
            tops = {s};
        } else if (s.marks == maxMarks) {
            tops.push_back(s);
        }
    }
    inFile.close();

    if (tops.empty())
        cout << "No students found!\n";
    else {
        cout << "🏆 Top Scorer(s) - Marks: " << maxMarks << endl;
        for (const auto& top : tops) {
            cout << "  " << top.name << " | Roll: " << top.roll << endl;
        }
    }
}

// Show average marks
void showAverage(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "❌ Error opening file!\n";
        return;
    }
    Student s;
    float sum = 0;
    int count = 0;
    while (inFile.read((char*)&s, sizeof(Student))) {
        sum += s.marks;
        count++;
    }
    inFile.close();
    if (count == 0) cout << "No students found!\n";
    else cout << "Average Marks: " << fixed << setprecision(2) << sum / count << endl;
}

int main() {
    const string filename = "students.dat";
    int choice, rollNo;

    do {
        cout << "\n--- Student Record Management ---\n";
        cout << "1. Add Student\n2. Display Students\n3. Search Student\n4. Delete Student\n";
        cout << "5. Sort by Marks\n6. Sort by Name\n7. Show Top Scorer\n8. Show Average Marks\n9. Exit\n";
        cout << "Enter choice: ";
        while (!(cin >> choice)) {
            cout << "Invalid choice. Enter 1-9: ";
            clearInput();
        }

        switch(choice) {
            case 1: addStudent(filename); break;
            case 2: displayStudents(filename); break;
            case 3:
                cout << "Enter roll number to search: ";
                while (!(cin >> rollNo)) {
                    cout << "Invalid roll. Enter an integer: ";
                    clearInput();
                }
                searchStudent(filename, rollNo);
                break;
            case 4:
                cout << "Enter roll number to delete: ";
                while (!(cin >> rollNo)) {
                    cout << "Invalid roll. Enter an integer: ";
                    clearInput();
                }
                deleteStudent(filename, rollNo);
                break;
            case 5: sortByMarks(filename); break;
            case 6: sortByName(filename); break;
            case 7: showTopScorer(filename); break;
            case 8: showAverage(filename); break;
            case 9: cout << "Exiting...\n"; break;
            default: cout << "❌ Invalid choice!\n";
        }
    } while(choice != 9);

    return 0;
}