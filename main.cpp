#include "PPP/Simple_window.h" // get access to our window librar y
#include "PPP/Graph.h"         // get access to our graphics librar y facilities
int main()
{
    using namespace Graph_lib;                 // our graphics facilities are in Graph_lib
    Application app;                           // star t a Graphics/GUI application
    Point tl{900, 500};                        // to become top left corner of window
    Simple_window win{tl, 600, 400, "Canvas"}; // make a simple window
    Polygon poly;                              // make a shape (a polygon)
    poly.add(Point{300, 200});                 // add a point
    poly.add(Point{350, 100});                 // add another point
    poly.add(Point{400, 200});                 // add a third point
    poly.set_color(Color::red);                // adjust properties of poly
    win.attach(poly);                          // connect poly to the window
   win.wait_for_button();                     // give control to the display engine
    Circle c{Point{100, 200}, 50};
    Ellipse e{Point{100, 200}, 75, 25};
    e.set_color(Color::dark_red);
    Mark m{Point{100, 200}, 'x'};
    m.set_color(Color::red);
    ostringstream oss;
    oss << "screen size: " << x_max() << "*" << y_max()
        << "; window siz e: " << win.x_max() << "*" << win.y_max();
    Text sizes{Point{100, 20}, oss.str()};
    Image scan{Point{275, 225}, "scandinavia.jfif"};
    scan.scale(150, 200);
    win.attach(c);
    win.attach(m);
    win.attach(e);
    win.attach(sizes);
    win.attach(scan);
    win.set_label("Final!");
    win.wait_for_button();
}