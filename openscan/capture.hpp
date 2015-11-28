/**
 * capture.hpp
 *
 * @date Nov 4, 2015
 * @author Ryan Peach
 * @version v0.1
 */

#ifndef CAPTURE
#define CAPTURE
#define DESKTOP

#include "filters.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <ctime>
#include <array>


class Capture {
 private:
    // Variable Declaration
    // These variables allow you to set global params for all process methods.
    Mat frame, edges;    // A variable to hold the most current processed frame
    Cnts polys;           // Just a temp variable to hold the polys from the most frame
    Fps fps;              // Just a temp variable to hold the Fp's from the most recent frame
    Point ref;            // The reference corner's location
    cnt border;           // The border of the scan
    vector<cnt> rects;    // Rectangles identified in the image

    int angleTol;          // The angle tolerance app-wide
    int distTol;           // The distance tolerance for small polygons (like focus pointss)
    int polyTol;           // The distance tolerance for universal polygons (like page borders)
    double aspectRatio;    // The aspect ratio of the page we are looking for.
    double sizeRatio;      // The min ratio of a detected page to the size of the image
    double ratioTol;       // The tolerance for two ratios to be considered equal

    //Import Parameters
    int etol1, etol2, eSize;

    // Preprocessing allows each process to share data,
    // so that nothing is calculated twice for the same image.
    bool changed = false;
    Mat getEdges();
    Cnts getPolys();
    Fps getFps();
    vector<cnt> getRects();

    cnt getBorder();

    void set(cnt corners);
    void set(Fps corners);
    bool validRect(cnt r);

    Mat drawInfo();
    void checkChanged();

 public:

    enum Method {fpcorners, strongborder, regular, automatic} sel;
    enum PageType {automatic, letter};
    enum Param {ANGLETOL, DISTTOL, POLYTOL, ASPECTRATIO, SIZERATIO, RATIOTOL, ETOL1, ETOL2, ESIZE, METHOD};


    /**
     * The main process. Finds the border of the page, filters it, warps it, returns the scan.
     * @return Returns a vector {The original image with a found border drawn, The scan in RGB}
     * @return If no border found, returns {The original image, NULL}.
     * @complexity O(?)
     */
    vector<Mat> process();

    Capture (int angleTol = 20, int distTol = 20, int polyTol = 5,
             PageType aspectRatio = automatic, double sizeRatio = .25, double ratioTol = .1,
             int etol1 = 100, int etol2 = 200, int eSize = 3, Method sel = fpcorners):
                angleTol(angleTol), distTol(distTol), polyTol(polyTol),
                sizeRatio(sizeRatio), ratioTol(ratioTol),
                etol1(etol1), etol2(etol2), eSize(eSize), sel(sel)
    {
        setAspectRatio(aspectRatio);
    }

    void Frame(Mat img);

    void setValue(Param param, double value) {
    	changed = true;
    	switch(param) {
    	case ANGLETOL: angleTol = (int)value; break;
    	case DISTTOL: distTol = (int)value; break;
    	case POLYTOL: polyTol = (int)value; break;
    	case ASPECTRATIO: setAspectRatio((PageType)((int)value)); break;
    	case SIZERATIO: sizeRatio = value; break;
    	case RATIOTOL: ratioTol = value; break;
    	case ETOL1: etol1 = (int)value; break;
    	case ETOL2: etol2 = (int)value; break;
    	case ESIZE: eSize = (int)eSize; break;
    	case METHOD: sel = (Method)((int)value); break;
    	case default: changed = false; break;
    	}
    }

    void setAspectRatio(PageType type = letter) {
        changed = true;
        switch (type) {
            case letter:
                aspectRatio = 8.5/11.0;
                break;
            case automatic:
                aspectRatio = 0;
                break;
        }
    }

};
#endif