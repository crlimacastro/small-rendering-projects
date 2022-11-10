#include "fae/fae.h"

using namespace std;
using namespace fae;

//// empty app
//int main()
//{
//	application app;
//	app.plugins.emplace(rendering_plugin);
//	app.run();
//}

#include "sandbox/sandbox.h"
// sandbox
int main()
{
	sandbox_application app;
	app.run();
}