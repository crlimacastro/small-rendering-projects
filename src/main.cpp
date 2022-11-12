#include "fae/fae.h"

using namespace std;
using namespace fae;

// empty app
int main()
{
	application app;
	app.plugins.emplace(rendering_plugin);
	app.run();
}

//#include "sandbox/sandbox.h"
//// sandbox (cellular automata)
//int main()
//{
//	sandbox_application app;
//	app.run();
//}

//#include "rope/rope.h"
//// rope simulation
//int main()
//{
//	rope_simulation app;
//	app.run();
//}

//#include "perlin/perlin.h"
//// perlin noise visualizer
//int main()
//{
//	perlin app;
//	app.run();
//}