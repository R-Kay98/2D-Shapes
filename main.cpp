#include <math.h>
#include <vector>
#include <iostream>

class Point
{
    private:
        float xAxisLoc;
        float yAxisLoc;
    public:
        Point(float xLoc, float yLoc) : xAxisLoc(xLoc), yAxisLoc(yLoc){}

        float getXAxisLoc() {
            return xAxisLoc;
        };

        float getYAxisLoc() {
            return yAxisLoc;
        };
};

class Line
{
    private:
        Point pointOne;
        Point pointTwo;
   
    public:
        Line(Point pOne, Point pTwo) : pointOne(pOne), pointTwo(pTwo) {}

        Point getPointOne(){
            return pointOne;
        };

        Point getPointTwo(){
            return pointTwo;
        };

        /*
        This function retrieves the true line length from a given line vector which is
        used for the purpose of calculating the area of the rectangle.
        */
        float getPythagoreanDist(){
            float xCoord = pointOne.getXAxisLoc() - pointTwo.getXAxisLoc();
            float yCoord = pointOne.getYAxisLoc() - pointTwo.getYAxisLoc();
            return sqrt((xCoord*xCoord) + (yCoord*yCoord));
        }
};


//One of the two main base shape classes for 2D objects in the physics engine.
class Polygon
{
    private:
        std::vector<Point> points;
        std::vector<Line> lines;
        int numSides;
    public:
        Polygon(std::vector<Point> points, std::vector<Line> lines, int numSides) : points(points), lines(lines), numSides(numSides) {}

        int getNumSides() {
            return numSides;
        };

        virtual bool pointInShape() {
            return true;
        };
        
        virtual float calculateArea() {
            return 0;
        }
};

/*
The triangle class here serves the purpose as a helper-class to determine whether a point is inside a rectangle.
This is done by partitioning the rectangle into four triangles with the addition of the fifth point and checking
the total area against the rectangle area.
*/
class Triangle : public Polygon
{
    private:
        std::vector<Point> points;
        std::vector<Line> lines;
        float sideA;
        float sideB;
        float sideC;
        float semiPerimeter;
    public:
        Triangle(std::vector<Point> points, std::vector<Line> lines) : points(points), lines(lines), Polygon(points, lines, 3) {
            sideA = lines.at(0).getPythagoreanDist();
            sideB = lines.at(1).getPythagoreanDist();
            sideC = lines.at(2).getPythagoreanDist();
            semiPerimeter = (sideA + sideB + sideC)/2; //Used in Heron's formula.
        }

        //Uses Heron's formula to determine the area.
        float calculateArea(){
            return sqrt(semiPerimeter*(semiPerimeter - sideA)*(semiPerimeter - sideB)*(semiPerimeter - sideC));
        };

        /*
        The function below does not necessarily verify whether it's a triangle
        but rather verifies there are three lines in the shape. To do this for a
        triangle, we need to verify that the sum of all angles adds up to 180
        degrees.
        */
        bool isValidShape() {
            if(lines.size() == 3) {
                return true;
            };
            return false;
        };
};

class Rectangle : public Polygon
{
    private:
        std::vector<Point> points;
        std::vector<Line> lines;
        float length;
        float height;
    public:
        Rectangle(std::vector<Point> points, std::vector<Line> lines) : points(points), lines(lines), Polygon(points, lines, 4) {
            length = lines.at(0).getPythagoreanDist();
            height = lines.at(1).getPythagoreanDist();
        }

        //Copy constructor.
        Rectangle(Rectangle& rect) : points(rect.points), lines(rect.lines), Polygon(rect.points, rect.lines, rect.getNumSides()) {
            length = rect.lines.at(0).getPythagoreanDist();
            height = rect.lines.at(1).getPythagoreanDist();
        }

        float calculateArea(){
            return length*height;
        };

        /*
        The function below does not necessarily verify whether it's a rectangle
        but rather verifies there are four lines in the shape. To do this for a
        rectangle, we need to verify that any two connected lines form a 90 degree
        angle.
        */
        bool isValidShape() {
           
            if (lines.size() == 4) {
                return true;
            };
            return false;
        };

        /*
        Helper method for the below function. The index arguments reference point values
        in the rectangle which form a rectangle line, while the point argument referes to
        the object we're trying to determine is in the rectangle.
        */
        float generateAndCalcTriArea(int indexOne, int indexTwo, Point point){
            Line Al = Line(points.at(indexOne), points.at(indexTwo)); //Line in the rectangle
            Line Bl = Line(points.at(indexTwo), point); //Line formed by joining one end of A1 to the point
            Line Cl = Line(point, points.at(indexOne)); //Line formed by joining the other end of A1 to the point
            std::vector<Point> triPoints = {points.at(indexOne), points.at(indexTwo), point};
            std::vector<Line> triLines = {Al, Bl, Cl};
            Triangle triangle(triPoints, triLines);
            return triangle.calculateArea();
        }

        //Determines if the point argument passed is contained in the rectangle.
        bool pointInShape(Point point) {
            float triangleAArea = generateAndCalcTriArea(0, 1, point); 
            float triangleBArea = generateAndCalcTriArea(1, 2, point);
            float triangleCArea = generateAndCalcTriArea(2, 3, point);
            float triangleDArea = generateAndCalcTriArea(3, 0, point);
            float totalTriAreas = triangleAArea + triangleBArea + triangleCArea + triangleDArea;
            float rectArea = calculateArea();
            return abs(totalTriAreas - rectArea) < 0.001; //Floats are not completely accurate so allow for some margin of error
        };
};

class Ellipse
{
    private:
        float semiMajorAxis;
        float semiMinorAxis;
        Point centerPoint;
    public:
        Ellipse(float semiMajorAxis, float semiMinorAxis, Point centerPoint) : semiMajorAxis(semiMajorAxis), semiMinorAxis(semiMinorAxis), centerPoint(centerPoint) {}
        
        virtual float getArea() {
            return atan(1)*4*semiMinorAxis*semiMajorAxis;
        };
};

class Circle : public Ellipse
{
    private:
        float radius;
    public:
        Circle(float radius, Point centerPoint) : radius(radius), Ellipse(radius, radius, centerPoint) {}
        
        float getArea() {
            return atan(1)*4*radius*radius;
        };
};

bool testRectArea(Rectangle rect, float expectedArea){
    return abs(rect.calculateArea() - expectedArea) < 0.001;
};

bool testIfRectIsValid(Rectangle rect){
    return rect.isValidShape();
};

bool testIfPointIsInShape(Rectangle rect, Point point){
    return rect.pointInShape(point);
};

void performUnitTests() {
    Point firstPoint(8, 0);
    Point secondPoint(4, 0);
    Point thirdPoint(4, 4);
    Point fourthPoint(8, 4);
    Point fifthPoint(0, 0);
    Point sixthPoint(6, 2);
    Point seventhPoint(4, 0);
    Point eighthPoint(4.1, 0.1);
    Point ninthPoint(3.9, -0.1);
    Line firstLine(firstPoint, secondPoint);
    Line secondLine(secondPoint, thirdPoint);
    Line thirdLine(thirdPoint, fourthPoint);
    Line fourthLine(fourthPoint, firstPoint);
    std::vector<Point> points = {firstPoint, secondPoint, thirdPoint, fourthPoint};
    std::vector<Line> lines = {firstLine, secondLine, thirdLine, fourthLine};
    Rectangle first(points, lines);
    Rectangle second(first);

    if (testRectArea(first, 16)) {
        std::cout << "Test 1 Passes - areas match.\n";
    }else{
        std::cout << "Test 1 Fails - areas don't match.\n";
    }

    if (testRectArea(second, 16)) {
        std::cout << "Test 2 Passes - areas match.\n";
    }else{
        std::cout << "Test 2 Fails - areas don't match.\n";
    }

    if (testRectArea(first, 17)) {
        std::cout << "Test 3 Fails - areas match.\n";
    }else{
        std::cout << "Test 3 Passes - areas shouldn't match.\n";
    }

    if (testIfRectIsValid(first)) {
        std::cout << "Test 4 Passes - rectangle has 4 sides.\n";
    }else{
        std::cout << "Test 4 Fails - rectangle doesn't have 4 sides.\n";
    }

    if (testIfPointIsInShape(first, fifthPoint)){
        std::cout << "Test 5 Fails - point detected in rectangle.\n";
    }else{
        std::cout << "Test 5 Passes - point not detected in rectangle.\n";
    }

    if (testIfPointIsInShape(first, sixthPoint)){
        std::cout << "Test 6 Passes - point detected in rectangle.\n";
    }else{
        std::cout << "Test 6 Fails - point not detected in rectangle.\n";
    }

    if (testIfPointIsInShape(first, seventhPoint)){
        std::cout << "Test 7 Passes - point detected in rectangle.\n";
    }else{
        std::cout << "Test 7 Fails - point not detected in rectangle.\n";
    }

    if (testIfPointIsInShape(first, eighthPoint)){
        std::cout << "Test 8 Passes - point detected in rectangle.\n";
    }else{
        std::cout << "Test 8 Fails - point not detected in rectangle.\n";
    }

    if (testIfPointIsInShape(first, ninthPoint)){
        std::cout << "Test 9 Fails - point detected in rectangle.\n";
    }else{
        std::cout << "Test 9 Passes - point not detected in rectangle.\n";
    }

    Point tenthPoint(0, 2);
    Point eleventhPoint(1, 3);
    Point thelfthPoint(2, 2);
    Point thirteenthPoint(1, 1);
    Point fourteenthPoint(1, 2);
    Point fifteenthPoint(0, 0);
    Line fifthLine(tenthPoint, eleventhPoint);
    Line sixthLine(eleventhPoint, thelfthPoint);
    Line seventhLine(thelfthPoint, thirteenthPoint);
    Line eighthLine(thirteenthPoint, tenthPoint);
    std::vector<Point> pointsTwo = {tenthPoint, eleventhPoint, thelfthPoint, thirteenthPoint};
    std::vector<Line> linesTwo = {fifthLine, sixthLine, seventhLine, eighthLine};
    Rectangle third(pointsTwo, linesTwo);

    if (testIfPointIsInShape(third, fourteenthPoint)){
        std::cout << "Test 10 Passes - point detected in rectangle.\n";
    }else{
        std::cout << "Test 10 Fails - point not detected in rectangle.\n";
    }

    if (testIfPointIsInShape(third, fifteenthPoint)){
        std::cout << "Test 11 Fails - point detected in rectangle.\n";
    }else{
        std::cout << "Test 11 Passes - point not detected in rectangle.\n";
    }

};

int main() {
    performUnitTests();
    return 0;
}