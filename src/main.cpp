#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Forward declaration
class Course;
class Category;

// Category Class
class Category {
private:
    string mCategoryName;
    float mWeight;
    int mPointsPossible;
    int mPointsEarned;

public:
    Category(const string& name, float weight, int pointsPossible, int pointsEarned)
        : mCategoryName(name), mWeight(weight), mPointsPossible(pointsPossible), mPointsEarned(pointsEarned) {}

    string getCategoryName() const {
        return mCategoryName;
    }

    float getWeightedScore() const {
        return (static_cast<float>(mPointsEarned) / mPointsPossible) * mWeight;
    }

    int getPointsNeededForGrade(float desiredPercentage) const {
        float pointsNeeded = desiredPercentage * mPointsPossible - getWeightedScore();
        return static_cast<int>(pointsNeeded);
    }   
};

// Course Class
class Course {
private:
    string mCourseName;
    vector<Category*> mCategories;

public:
    Course(const string& name) : mCourseName(name) {}

    void addCategory(Category* category) {
        mCategories.push_back(category);
    }

    vector<Category*>& getCategories() {
        return mCategories;
    }

    string getCourseName() const {
        return mCourseName;
    }
};

// Quarter Class
class Quarter {
private:
    vector<Course*> mCourses;

public:
    Quarter() {}

    vector<Course*> getCourses() const {
        return mCourses;
    }

    void addCourse(Course* course) {
        mCourses.push_back(course);
    }

    void removeCourse(const string& courseName) {
        mCourses.erase(remove_if(mCourses.begin(), mCourses.end(), [&](Course* course) {
            return course->getCourseName() == courseName;
        }), mCourses.end());
    }

    void printRequiredPointsForGrade(char desiredGrade) {
    for (const auto& course : mCourses) {
        cout << "Course: " << course->getCourseName() << endl;
        for (const auto& category : course->getCategories()) {
            float weightedScore = category->getWeightedScore();
            int requiredPoints = category->getPointsNeededForGrade(desiredGrade);
            cout << "Category: " << category->getCategoryName() << ", Weighted Score: " << weightedScore
                 << ", Required Points for " << desiredGrade << ": " << requiredPoints << endl;
        }
    }
}
};

// CSV Reader
class CSVReader {
public:
    static Quarter* readCSV(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Unable to open file " << filename << endl;
            return nullptr;
        }

        Quarter* quarter = new Quarter();
        string line;
        getline(file, line); // Read and ignore header line
        while (getline(file, line)) {
            stringstream ss(line);
            string courseName, categoryName;
            float weight;
            int pointsPossible, pointsEarned;

            getline(ss, courseName, ',');
            getline(ss, categoryName, ',');
            ss >> weight;
            ss.ignore(); // Ignore comma
            ss >> pointsPossible;
            ss.ignore(); // Ignore comma
            ss >> pointsEarned;

            Course* course = findOrCreateCourse(quarter, courseName);
            Category* category = new Category(categoryName, weight, pointsPossible, pointsEarned);
            course->addCategory(category);
        }

        file.close();
        return quarter;
    }

private:
    static Course* findOrCreateCourse(Quarter* quarter, const string& courseName) {
        for (auto& course : quarter->getCourses()) {
            if (course->getCourseName() == courseName) {
                return course;
            }
        }
        Course* newCourse = new Course(courseName);
        quarter->addCourse(newCourse);
        return newCourse;
    }
};

int main() {
    Quarter* quarter = CSVReader::readCSV("./src/grades.csv");
    if (quarter) {
        char desiredGrade;
        cout << "Enter the desired grade (A, B, C, D, F): ";
        cin >> desiredGrade;

        quarter->printRequiredPointsForGrade(desiredGrade);
    }

    return 0;
}