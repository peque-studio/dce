#include <dcore/graphics.h>
#include <dcore/graphics/internal.h>

static void createInstance(DCgState *state, uint32_t appVersion, const char *appName) {
	VkApplicationInfo applicationInfo = {0};
	applicationInfo.applicationVersion = appVersion;
	applicationInfo.pApplicationName = appName;
	applicationInfo.engineVersion = VK_MAKE_VERSION(
		DCE_VERSION_MAJOR,
		DCE_VERSION_MINOR,
		DCE_VERSION_PATCH
	);
	applicationInfo.pEngineName = "DCE";

	VkInstanceCreateInfo createInfo = {0};
	createInfo.pApplicationInfo = &applicationInfo;
}

void dcgInit(DCgState *state) {
	state->allocator = NULL;
	
	
}
