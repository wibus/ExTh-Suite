#include <exception>
#include <iostream>
using namespace std;

#include <QApplication>

#include "ExCompositorWindow.h"


int main(int argc, char** argv) try
{
    QApplication app(argc, argv);

    ExCompositorWindow window;
    window.show();

    return app.exec();
}
catch(exception& e)
{
    cerr << "Exception caught : " << e.what() << endl;
}
catch(...)
{
    cerr << "Exception passed through.." << endl;
    throw;
}
