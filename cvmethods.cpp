/**
 * cvmethods.cpp
 *
 * @date Nov 4, 2015
 * @author Ryan Peach
 * @version v0.1
 */

#include "cvmethods.hpp"

// -------------- Feature Detection ----------------

Cnts findPolys(Mat img, double distTol) {
    // Find contours and heirarchy
    vector<cnt> contours, polys; vector<Vec4i> heirarchy; cnt temp;
    findContours(img, contours, heirarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // Return approximate polygons
    for (unsigned int i = 0; i < contours.size(); i++) {
        approxPolyDP(contours[i], temp, distTol, true);
        polys.push_back(temp);
    }

    // Return Cnts
    return Cnts(polys, heirarchy);
}

vector<Fp> findFocusPoints(Cnts polys, double angleTol, double distTol) {
    //Definitions
    vector<Fp> out; vector< vector<cnt> > cntV;
    vector<int> done; vector<cnt> contours; int k;
    cnt poly;

    for(unsigned int i = 0; i < polys.contours.size(); i++) {
        k=i; poly.clear();
        if(!contains<vector<int>,int>(done,(int)i)){        //Check that through navigation you haven't been here before
            done.push_back(i);

            // Navigate the heirarchy
            while (polys.heirarchy[k][2] != -1) {
                k=polys.heirarchy[k][2];
                done.push_back(k);
                contours.push_back(polys.contours[k]);
            }
            if (polys.heirarchy[k][2] != -1) {contours.push_back(polys.contours[k]);} //Add the last element

            // Check if there are enough polys to count as a potential focus point, append them to fp
            if (poly.size() >= 5) {cntV.push_back(contours);}
        }
    }

    // Filter the focus points for their innermost border
    for (unsigned int x = 0; x < cntV.size(); x++) {
        Fp tempFp = Fp(cntV[x],angleTol,distTol);
        if (tempFp.depth >= 0) { //Check that cntV[x] is a valid Fp
            out.emplace_back(tempFp.contours, tempFp.contour, tempFp.center, tempFp.depth, tempFp.shape);
        }
    }

    // Return the focus points
    return out;
}

vector<Fp> getCorners(vector<Fp> focusPoints, double angleTol, double distTol) {
    // Make fours a list of only size four Fp's
    // vector<Fp> fours = filter(focusPoints,[](Fp z){return z.shape == 4;});
    vector<Fp> fours;
    for (Fp z : focusPoints) {
            if (z.shape == 4) {
                    fours.push_back(z);
            }
    }

    // Classify corners as having 2 right angles
    vector<Fp> out;
    vector<double> a, temp;
    for (Fp f : fours) {
        a = angs(f.center,fours);
        //temp = filter(a,[](double d){return abs(d-90.0)<angleTol;});
        vector<double> temp;
        for (double d : a) {
            if (abs(d-90.0) < angleTol) {
                temp.push_back(d);
            }
        }
        if (temp.size()>=2 && !contains<vector<Fp>,Fp>(out,f)) {
            out.push_back(f);
        }
    }

    //Return their centroids
    vector<Fp> rect = hasRectangle(out, angleTol, distTol);
    if (rect.size() != 4) {return vector<Fp>();}
    else {return out;}
}

vector<Fp> sortCorners(vector<Fp> corners, Fp ref) {
    Point r = centroid(ref); vector<Fp> out = corners;
    for(int i = 0; i < 4 && centroid(corners[0]) != r; i++) {
        out = rotateVec(out);
    }
    return out;
}

cnt sortCorners(cnt corners) {
    //Variable Declaration
    Point cent = centroid(corners); vector<double> polar; int n; cnt out;

    //Calculate all the angles from the centroid, maintaining index
    for (Point p : corners) {polar.push_back(angle(p,cent));}
    vector<double> sorted = polar;
    sort(sorted.begin(),sorted.end());

    //Sort "corners" by the order of sorted "polar"
    for (double d : sorted) {
        n = index(polar, d);
        out.push_back(corners[n]); //Return sorted corners
    }
    return out;
}


Fp getRef(vector<Fp> fps) {
    Fp maxFp = fps[0]; int max = maxFp.depth; //sets default values
    for (Fp fp : fps) {
        if (fp.depth > max) {
            maxFp = fp;
            max = fp.depth;
        }
    }
    return maxFp;
}

Point getRef(cnt contour) {
    auto D = dists(contour); unsigned int a = 0; unsigned int b = 1;
    while (D[a]<=D[b]){
        a++; b++;
        if (b>=contour.size()){b=0;}
        if (a==contour.size()){return contour[0];}
    }
    return contour[a];
}


// ------------ Image Manipulation --------------
Mat importFilter(Mat img, int tol1, int tol2, int wSize) {
    //Testing & Declarations
    Mat gray, edges;
    while(wSize%2!=1) {wSize++;}	//wSize must be an odd number

    //Convert to gray if not already
    if (isColor(img)) {cvtColor(img,gray,COLOR_RGB2GRAY);}
    else {gray = img;}

    //Return Canny Edge Detection
    Canny(gray,edges,tol1,tol2,wSize);
    return edges;
}

Mat outputFilter(Mat img, int wSize, int C) {
    //Testing & Declarations
    Mat gray, out;
    while(wSize%2!=1) {wSize++;}	//wSize must be an odd number

    //Convert to gray if not already
    if (isColor(img)) {cvtColor(img,gray,COLOR_RGB2GRAY);}
    else {gray = img;}

    adaptiveThreshold(gray, out, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, wSize, C);
    return out;
}

Mat cropImage(Mat img, int R) {
    int sizeX = img.cols; int sizeY = img.rows;
    Mat out = img(Rect(R,R,sizeX,sizeY));
    return out;
}

Mat fixPerspective(Mat img, vector<Fp> border, Fp ref) {
    // Declare variables
    Point tl, tr, bl, br;
    Mat out;
    int n = 0;

    // Rotate the array until the reference is first
    while (border[0].center != ref.center && n < 4) {
        border = rotateVec(border);
        n++;
    }

    tl = border[0].center; tr = border[1].center; br = border[2].center; bl = border[3].center;

    // compute the width of the new image, which will be the
    // maximum distance between bottom-right and bottom-left
    // x-coordiates or the top-right and top-left x-coordinates
    auto widthA = sqrt(pow((double)(br.x - bl.x), 2.0) + pow((double)(br.y - bl.y), 2.0));
    auto widthB = sqrt(pow((double)(tr.x - tl.x), 2.0) + pow((double)(tr.y - tl.y), 2.0));
    int maxWidth = max(int(widthA), int(widthB));

    // compute the height of the new image, which will be the
    // maximum distance between the top-right and bottom-right
    // y-coordinates or the top-left and bottom-left y-coordinates
    auto heightA = sqrt(pow((double)(tr.x - br.x), 2.0) + pow((double)(tr.y - br.y), 2.0));
    auto heightB = sqrt(pow((double)(tl.x - bl.x), 2.0) + pow((double)(tl.y - bl.y), 2.0));
    int maxHeight = max((int)heightA, (int)heightB);

    // now that we have the dimensions of the new image, construct
    // the set of destination points to obtain a "birds eye view",
    // (i.e. top-down view) of the image, again specifying points
    // in the top-left, top-right, bottom-right, and bottom-left
    // order
    auto dst = vector<Point2f>({Point(0, 0),
    Point(maxWidth - 1, 0),
    Point(maxWidth - 1, maxHeight - 1),
    Point(0, maxHeight - 1)});
    auto src = vector<Point2f>({tl,tr,bl,br});

    //Return Perspective Transform
    auto M = getPerspectiveTransform(src, dst);
    warpPerspective(img, out, M, Size(maxWidth, maxHeight));
    return out;
}

bool isColor(Mat img) {
    if (img.channels()==3) {return true;}
    else {return false;}
}
