#include "fae/fae.h"

// empty app
//int main()
//{
//	fae::application app;
//	app.plugins.emplace(fae::rendering_plugin);
//	app.run();
//}

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

//#include "lerp_visualizer/lerp_visualizer.h"
//int main()
//{
//	lerp_visualizer app;
//	app.run();
//}

#include "fluid/fluid.h"
int main()
{
	fluid app;
	app.run();
}